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

#ifndef ARDUCOMM_H
#define ARDUCOMM_H

#include <Arduino.h>
#include "arducomm/subscriber.h"

constexpr uint16_t TIMEOUT = 3000;
constexpr uint8_t MAX_RETRIES = 3;

constexpr uint8_t START_FLAG = 0x7E;
constexpr uint8_t ESCAPE_FLAG = 0x7D;
constexpr uint8_t ACK_COMMAND = 0x01;

typedef enum tx_error : uint8_t
{
    NO_ERROR = 0,  // Frame was sent successfully
    TIMEOUT_ERROR, // No ACK received after the timeout
    ACK_ERROR      // Frame was received with errors multiple times (max. number of tx retries exceeded)
} TXError;

typedef enum ack_value : uint8_t
{
    ACK_UNSET = 0,
    ACK_OK,
    ACK_RETRY
} ACKValue;

enum frame_index : uint8_t
{
    FLAG = 0,
    SEQ_NUMBER = 1,
    COMMAND = 2,
    PAYLOAD = 3
};

/*  Template parameters:
    BUFFER_SIZE: Size to store incoming frames. This will limit the max. frame size allowed.
        The minimum buffer size is 6 (allowing only command frames without payload).
    NUM_CALLBACKS: Maximum number of callbacks.
        This number cannot be smaller than the highest possible command value.
        For example, if the possible commands to be received are {3, 9, 27},
        The number of callbacks must be at least 28.
*/
template <size_t BUFFER_SIZE = 128, size_t NUM_CALLBACKS = 256>
class ArduCommT {

public:
    ArduCommT()
    {
        command_ = 0;
        buffer_index_ = 0;
        escape_received_ = 0;
        payload_size_ = 0;
        sent_seq_ = 0;
        last_ack_ = ACK_UNSET;
        retries_ = 0;
    }
    ~ArduCommT(){}

    /* Initialize the default port (Serial). */
    void begin(unsigned long baud)
    {
        Serial.begin(baud);
        set_stream(&Serial);
    }

    /* Set the inner Stream object (Serial). The stream must be open (.begin(...)) before using the interface */
    void set_stream(Stream* port)
    {
        serial_ = port;
    }

    /* Read the bytes from the serial buffer and check if a new frame is available.
    *  Return 1 if a new packet is available, else 0.
    *  Note: The reading will stop when a full frame is found.
    *        The serial buffer might not be completely read.
    */
    uint8_t read()
    {
        // Read all bytes in the serial buffer
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
            else if (in_buffer_[buffer_index_ - 1] == ESCAPE_FLAG)
            {
                // Received a escape flag. Get ready to invert the next byte
                escape_received_ = 1;
                buffer_index_--;
            }
            else if (in_buffer_[buffer_index_ - 1] == START_FLAG)
            {
                // Get number of bytes between start flags to skip ghost frames (between end_flag and start_flag)
                if (buffer_index_ > 3)
                {
                    // There is a complete frame in the buffer. Process it and return if new_packet is available
                    return process_frame();
                }
                else
                {
                    // We received a start flag but the buffer is too small to contain a full frame
                    // Reset the buffer and assume last FLAG is the start of a frame, and not the end
                    in_buffer_[0] = START_FLAG;
                    buffer_index_ = 1;
                }
            }
            else if (escape_received_)
            {
                in_buffer_[buffer_index_ - 1] ^= (1 << 5);
                escape_received_ = 0;
            }
        }
        // If no more bytes available to read, return 0 (no full frame available yet)
        return 0;
    }

    /* Send a packet given the command and the payload */
    TXError send(uint8_t command, uint8_t payload_size, uint8_t payload[])
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

    /* Send a packet given with just a command */
    TXError send(uint8_t command)
    {
        return send(command, 0, NULL);
    }

    /* Get the data from the last packet received */
    inline uint8_t get_command() const {return command_;}
    uint8_t get_payload(uint8_t payload[]) const
    {
        for (int i = 0; i < payload_size_; ++i)
        {
            payload[i] = payload_[i];
        }
        return payload_size_;
    }

    /* Add a new callback to process received messages.
       All packet frames with the specified command will be parsed and processed by the given callback function.
       The template param T defines the type of the messages for this callback, necessary for the parsing.
     */
    template <typename T>
    void add_callback(const uint8_t& command, typename arducomm::Subscriber<T>::f_callback_t callback)
    {
        subscribers_[command] = new arducomm::Subscriber<T>(callback);
    }

private:
    Stream *serial_;
    uint8_t in_buffer_[BUFFER_SIZE];
    uint8_t buffer_index_;
    uint8_t escape_received_;

    uint8_t command_;
    uint8_t payload_[BUFFER_SIZE - 4];
    uint8_t payload_size_;
    uint8_t sent_seq_;
    ACKValue last_ack_;
    uint8_t retries_;

    arducomm::Subscriber_* subscribers_[NUM_CALLBACKS];

    /* Get the frame sequence from the buffer, extract the frame data and process it */
    uint8_t process_frame()
    {
        uint8_t new_packet = 0;

        if (in_buffer_[COMMAND] == ACK_COMMAND)
        {
            // Received an ACK frame. Set the ack value.
            if (in_buffer_[SEQ_NUMBER] == sent_seq_)
            {
                last_ack_ = ACK_RETRY;
            }
            else
            {
                last_ack_ = ACK_OK;
            }
            new_packet = 1;
        }
        else
        {
            // Received a packet frame. Update the command and payload fields
            // Copy frame data
            command_ = in_buffer_[COMMAND];
            // Maybe the frame size is smaller than 6 (frames without checksum like ACKs, should never happen)
            payload_size_ = buffer_index_ > 6 ? buffer_index_ - 6 : 0;
            for (int i = 0; i < payload_size_; ++i)
            {
                payload_[i] = in_buffer_[PAYLOAD + i];
            }

            // Check checksum
            uint16_t received_checksum = (in_buffer_[buffer_index_ - 3] << 8) | in_buffer_[buffer_index_ - 2];
            uint16_t computed_checksum = checksum(in_buffer_[SEQ_NUMBER], command_, payload_size_, payload_);

            // Send ACK
            if (received_checksum != computed_checksum)
            {
                // Retry
                send_ack(in_buffer_[SEQ_NUMBER]);
            }
            else
            {
                // Packet RX ok
                new_packet = 1;
                send_ack((in_buffer_[SEQ_NUMBER] + 1) % 256);
            }
        }

        // After processing the frame, clear the index to reuse the buffer
        in_buffer_[0] = 0;
        buffer_index_ = 0;

        // If the packet frame is ok and we have a subscriber registered for this command, execute the callback function.
        // Use the command in the buffer instead of command_,
        // because we might be processing an ACK and command_ will have the value of the latest command
        if (new_packet && subscribers_[in_buffer_[COMMAND]])
        {
            subscribers_[command_]->callback(payload_);
        }

        return new_packet;
    }

    /* Wait and read until a new ACK arrives 
    *  Returns TIMEOUT_ERROR if TIMEOUT is exceded, NO_ERROR otherwise.
    */
    TXError wait_ack()
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
    void send_ack(uint8_t seq_number)
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

    /* Write a byte to the serial interface, escaping if necessary */
    void write_escaped(uint8_t data_byte)
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

    /* Compute the 16bit Fletcher's checksum of the data */
    uint16_t checksum(uint8_t seq_number, uint8_t command, uint8_t payload_size, uint8_t payload[])
    {
        uint16_t lsb = 0;
        uint16_t  msb = 0;

        lsb += seq_number;
        msb += lsb;
        lsb += command;
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
};

// Default definition of ArduComm class, with a buffer size of 128 bytes and 256 callbacks
typedef ArduCommT<128, 256> ArduComm;

#endif