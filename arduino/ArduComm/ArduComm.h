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

#ifndef ArduComm_h
#define ArduComm_h

#include <Arduino.h>

#define BAUDRATE 57600
#define TIMEOUT 3000
#define MAX_RETRIES 3

#define START_FLAG 0x7E
#define ESCAPE_FLAG 0x7D
#define ACK_COMMAND 0x01

typedef enum tx_error
{
    NO_ERROR = 0,
    TIMEOUT_ERROR,
    ACK_ERROR
} TXError;

typedef enum ack_value
{
    ACK_UNSET = 0,
    ACK_OK,
    ACK_RETRY
} ACKValue;

enum frame_index
{
    FLAG = 0,
    SEQ_NUMBER = 1,
    COMMAND = 2,
    PAYLOAD_SIZE = 3
};

class ArduComm{

public:
    ArduComm();
    ~ArduComm();

    /* Initialize the serial object. serial.begin() must be called before this */
    void begin(Stream* serial);

    /* Read all bytes in the serial buffer and check if a new frame is available.
    *  Return 1 if a new packet is available, else 0
    */
    uint8_t read();

    /* Send a packet given the command and the payload */
    TXError send(uint8_t command, uint8_t payload_size, uint8_t payload[]);
    /* Send a packet given with just a command */
    TXError send(uint8_t command);

    /* Get the data from the last packet received */
    uint8_t get_command();
    uint8_t get_payload(uint8_t payload[]);
    
private:
    Stream *serial_;
    uint8_t in_buffer_[64];
    uint8_t buffer_index_;

    uint8_t command_;
    uint8_t payload_[64];
    uint8_t payload_size_;
    uint8_t sent_seq_;
    ACKValue last_ack_;
    uint8_t retries_;
    uint32_t start_time_;

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