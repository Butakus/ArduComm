/* Simple example to use ArduComm library to turn on and off a led.
*
*  Commands 0x03 and 0x7E turn the led ON.
*  Command 0x00 turns the led OFF.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    February 2017
*/

#include <ArduComm.h>

ArduComm comms;
// Payload buffer to store the received data
uint8_t payload[64];

void setup()
{
    // Initialize serial port and comms object
    Serial.begin(57600);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);

    // Send an initialization command
    comms.send(0x03);

    delay(1000);
}

void loop()
{
    if (comms.read())
    {
        // If there is a frame available, get the command and the payload
        uint8_t command = comms.get_command();
        uint8_t payload_size = comms.get_payload(payload);
        if (command == 0x03 || command == 0x7E)
        {
            digitalWrite(13, HIGH);
        }
        else if (command == 0x00)
        {
            digitalWrite(13, LOW);
        }
    }
    delay(5);
}