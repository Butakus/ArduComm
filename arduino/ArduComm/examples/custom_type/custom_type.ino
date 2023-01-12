/* Define a custom data type and transmit it with ArduComm.
*
*  The custom "complex" type is Odometry, defined in the odometry.ino file.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#include <ArduComm.h>

ArduComm comms;
// Payload buffer to store the received data
uint8_t payload[64];

void setup()
{
    // Initializa serial port and comms object
    Serial.begin(57600);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);

    // TODO

    delay(1000);
}

void loop()
{
    // TODO
    delay(5);
}