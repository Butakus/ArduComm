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

class ArduComm {

public:
    ArduComm();
    ~ArduComm();

    /* Initialize the default port (Serial). */
    void begin(unsigned long baud);
    /* Set the inner Stream object (Serial). The stream must be open (.begin(...)) before using the interface */
    void set_stream(Stream* serial);

    /* Read the bytes from the serial buffer and check if a new frame is available.
    *  Return 1 if a new packet is available, else 0.
    *  Note: The reading will stop when a full frame is found.
    *        The serial buffer might not be completely read.
    */
    uint8_t read();

    /* Send a packet given the command and the payload */
    TXError send(uint8_t command, uint8_t payload_size, uint8_t payload[]);
    /* Send a packet given with just a command */
    TXError send(uint8_t command);

    /* Get the data from the last packet received */
    uint8_t get_command() const;
    uint8_t get_payload(uint8_t payload[]) const;

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
    uint8_t in_buffer_[64];
    uint8_t buffer_index_;
    uint8_t escape_received_;

    uint8_t command_;
    uint8_t payload_[64];
    uint8_t payload_size_;
    uint8_t sent_seq_;
    ACKValue last_ack_;
    uint8_t retries_;

    arducomm::Subscriber_* subscribers_[256];

    /* Get the frame sequence from the buffer, extract the frame data and process it */
    uint8_t process_frame();

    /* Wait and read until a new ACK arrives 
    *  Returns TIMEOUT_ERROR if TIMEOUT is exceded, NO_ERROR otherwise.
    */
    TXError wait_ack();

    /* Send an ACK response */
    void send_ack(uint8_t seq_number);

    /* Write a byte to the serial interface, escaping if necessary */
    void write_escaped(uint8_t data_byte);

    /* Compute the 16bit Fletcher's checksum of the data */
    uint16_t checksum(uint8_t seq_number, uint8_t command, uint8_t payload_size, uint8_t payload[]);
};

#endif