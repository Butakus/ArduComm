/* Library that implements a data-layer communication protocol between the Arduino and a computer via serial.
*  
*  The communication protocol is based on HLDLC with ARQ-StopAndWait error control.
*
*  Note: Works only on version 1.0 and above of Arduino IDE. (only arduino.h)
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    February 2017
*/

#include "ArduComm.h"


ArduComm::ArduComm()
{
    command_ = 0;
    buffer_index_ = 0;
    start_time_ = 0;
    payload_size_ = 0;
    sent_seq_ = 0;
    last_ack_ = ACK_UNSET;
    retries_ = 0;
}

ArduComm::~ArduComm(){}

/* Initialize the serial object. serial.begin() must be called before this */
void ArduComm::begin(Stream *port)
{
    serial_ = port;
}

/* Read all bytes in the serial buffer and check if a new frame is available.
*  Return 1 if a new packet is available, else 0
*/
uint8_t ArduComm::read()
{
    // Read all bytes in the serial buffer
    uint8_t new_packet = 0;
    while (serial_->available())
    {
        // Read first byte into the buffer
        in_buffer_[buffer_index_++] = serial_->read();
        if (in_buffer_[0] != START_FLAG)
        {
            // Broken frame. The buffer will never contain a full packet.
            // Drop bytes until a START_FLAG arrives
            buffer_index_ = 0;
        }
        else if (in_buffer_[buffer_index_ - 1] == START_FLAG)
        {
            // Get number of bytes between start flags to skip ghost frames (between end_flag and start_flag)
            if (buffer_index_ > 3)
            {
                // There is a complete frame in the buffer. Process it.
                new_packet = process_frame();
                // After processing the frame, clear the index to reuse the buffer
                in_buffer_[0] = 0;
                buffer_index_ = 0;
                // Exit if a new packet is received to avoid overwriting packets
                if (new_packet) break;
            }
            else
            {
                // We received a start flag but the buffer is too small to contain a full frame
                // Reset the buffer and assume last FLAG is the start of a frame, and not the end
                in_buffer_[0] = START_FLAG;
                buffer_index_ = 1;
            }
        }
    }
    return new_packet;
}

/* Write a byte to the serial interface, escaping if necessary */
void ArduComm::write_escaped(uint8_t data_byte)
{
    if (data_byte == START_FLAG || data_byte == ESCAPE_FLAG)
    {
        serial_->write(ESCAPE_FLAG);
        serial_->write(data_byte ^ (1 << 5));
    }
    else
    {
        serial_->write(data_byte);
    }
}

/* Send a packet given the command and the payload */
TXError ArduComm::send(uint8_t command, uint8_t payload_size, uint8_t payload[])
{
    sent_seq_ = (sent_seq_ + 1) % 256;

    // Reset last ack
    last_ack_ = ACK_UNSET;

    // Compute the checksum of the data to send
    uint16_t computed_checksum = checksum(sent_seq_, command, payload_size, payload);

    // Send all bytes, escaping if necessary
    // Start flag
    serial_->write(START_FLAG);
    
    // Sequence number
    write_escaped(sent_seq_);
    
    // Command
    write_escaped(command);
    
    // Payload size
    write_escaped(payload_size);

    // Payload
    for (uint8_t i = 0; i < payload_size; ++i)
    {
        write_escaped(payload[i]);
    }

    // Checksum
    write_escaped((computed_checksum & 0xFF00) >> 8); // Checksum MSB
    write_escaped(computed_checksum & 0x00FF); // Checksum LSB

    // End flag
    serial_->write(START_FLAG);

    // Wait for the ACK
    TXError error = wait_ack();
    if (error == NO_ERROR)
    {
        // Get ACK
        if (last_ack_ == ACK_RETRY)
        {
            // Retry
            if (++retries_ >= MAX_RETRIES)
            {
                error = ACK_ERROR;
            }
            else
            {
                sent_seq_ = (sent_seq_ - 1) % 256;
                error = send(command, payload_size, payload);
            }
        }
        else
        {
            // ACK OK
            error = NO_ERROR;
            retries_ = 0;
        }
    }
    return error;
}

/* Send a packet with just a command */
TXError ArduComm::send(uint8_t command)
{
    return send(command, 0, NULL);
}

uint8_t ArduComm::get_command()
{
    return command_;
}

uint8_t ArduComm::get_payload(uint8_t payload[])
{
	for (int i = 0; i < payload_size_; ++i)
	{
	    payload[i] = payload_[i];
	}
    return payload_size_;
}

/* Get the frame sequence from the buffer, extract the frame data and process it */
uint8_t ArduComm::process_frame()
{
    uint8_t new_packet = 0;
    // Array to store the buffer data after removing escape flags
    uint8_t escaped_data[buffer_index_];
    uint8_t total_size = 0;
    // Unescape the data
    for (int i = 0; i < buffer_index_; ++i)
    {
        uint8_t b = in_buffer_[i];
        if (b == ESCAPE_FLAG)
        {
            b = in_buffer_[++i] ^ (1 << 5);
        }
        escaped_data[total_size++] = b;
    }

    if (escaped_data[COMMAND] == ACK_COMMAND)
    {
        // Received an ACK frame. Set the ack value.
        if (escaped_data[SEQ_NUMBER] == sent_seq_)
        {
            last_ack_ = ACK_RETRY;
        }
        else
        {
            last_ack_ = ACK_OK;
        }
    }
    else
    {
        // Received a packet frame. Update the command and payload fields
        uint8_t retry = 0;

        // Copy frame data
        command_ = escaped_data[COMMAND];
        payload_size_ = escaped_data[PAYLOAD_SIZE];
        for (int i = 0; i < payload_size_; ++i)
        {
            payload_[i] = escaped_data[i+4];
        }

        // Check size and checksum
        if (payload_size_ != total_size - 7)
        {
            retry = 1;
        }
        else
        {
            uint16_t received_checksum = (escaped_data[total_size - 3] << 8) | escaped_data[total_size - 2];
            uint16_t computed_checksum = checksum(escaped_data[SEQ_NUMBER], command_, payload_size_, payload_);

            if (received_checksum != computed_checksum) retry = 1;
        }

        // Send ACK
        if (retry)
        {
            send_ack(escaped_data[SEQ_NUMBER]);
        }
        else
        {
            // Packet RX ok
            new_packet = 1;
            send_ack((escaped_data[SEQ_NUMBER] + 1) % 256);
        }
    }
    return new_packet;
}

/* Wait and read until a new ACK arrives 
*  Returns TIMEOUT_ERROR if TIMEOUT is exceded, NO_ERROR otherwise.
*/
TXError ArduComm::wait_ack()
{
    // TODO: Check possible issue where current frame stored might be overwritten when reading.
    unsigned long start_time = millis();
    TXError error = NO_ERROR;
    while (last_ack_ == ACK_UNSET)
    {
        if ((millis() - start_time) > TIMEOUT)
        {
            error = TIMEOUT_ERROR;
            break;
        }
        read();
    }
    return error;
}

/* Send an ACK response */
void ArduComm::send_ack(uint8_t seq_number)
{
    // Send all bytes, escaping if necessary
    // Start flag
    serial_->write(START_FLAG);
    
    // Sequence number
    write_escaped(seq_number);
    
    // Command
    serial_->write(ACK_COMMAND);

    // End flag
    serial_->write(START_FLAG);
}

/* Compute the 16bit Fletcher's checksum of the data */
uint16_t ArduComm::checksum(uint8_t seq_number, uint8_t command, uint8_t payload_size, uint8_t payload[])
{
    uint16_t lsb = 0;
    uint16_t  msb = 0;

    lsb += seq_number;
    msb += lsb;
    lsb += command;
    msb += lsb;
    lsb += payload_size;
    msb += lsb;

    for (int i = 0; i < payload_size; ++i)
    {
        lsb += payload[i];
        msb += lsb;
        if (i % 16 == 0)
        {
            // Do a reduction each 16 bytes;
            lsb = (lsb & 0xFF) + (lsb >> 8);
            msb = (msb & 0xFF) + (msb >> 8);
        }
    }
    // Last double reduction to add the carry;

    lsb = (lsb & 0xFF) + (lsb >> 8);
    lsb = (lsb & 0xFF) + (lsb >> 8);
    msb = (msb & 0xFF) + (msb >> 8);
    msb = (msb & 0xFF) + (msb >> 8);

    return (msb << 8) | lsb;
}
