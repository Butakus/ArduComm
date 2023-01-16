/* Define a custom data type and transmit it with ArduComm.
*
*  The custom "complex" type is Odometry, defined in the odometry.ino file.
*
*  This program will wait until an initial Odometry object is received with the command 0x02.
*  Then it will update the odometry position and send the updates with the command 0x05.

*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#include <ArduComm.h>
#include <arducomm/serialization.h>
#include "odometry.h"

ArduComm comms;
// Payload buffer to store the received data
uint8_t payload[64];
bool first_odom_received = false;
Odometry odom_msg;

void setup()
{
    // Initialize serial port and comms object
    Serial.begin(57600);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);

    delay(1000);
}

void loop()
{
    uint8_t error = NO_ERROR;

    // Read and set the odometry object
    if (comms.read())
    {
        uint8_t command = comms.get_command();
        uint8_t payload_size = comms.get_payload(payload);

        switch (command)
        {
            case 2:
            {
                // Odometry initialization test
                odom_msg = parse<Odometry>(payload);
                first_odom_received = true;
                break;
            }
        }
    }

    // If we have already received the initial odom object, update its pose and send the update
    if (first_odom_received)
    {
        odom_msg.pose.position.x += odom_msg.linear_vel.x * 0.1; // Position increment in 10ms
        serialize(odom_msg, payload);
        error = comms.send(5, Odometry::size, payload);
        handle_error(error);
    }

    // Handle error
    // Blink a led if there was an error in the communication
    switch (error)
    {
        case TIMEOUT_ERROR: blink(1000); break;
        case ACK_ERROR: blink(2000); break;
        default: break;
    }

    delay(100);
}

/* Handle error:
   Blink a led if there was an error in the communication
*/
void handle_error(uint8_t error)
{
    switch (error)
    {
        case TIMEOUT_ERROR: blink(1000); break;
        case ACK_ERROR: blink(2000); break;
        default: break;
    }    
}

void blink(unsigned long t)
{
    digitalWrite(13, HIGH);
    delay(t);
    digitalWrite(13, LOW);
    delay(t);
}
