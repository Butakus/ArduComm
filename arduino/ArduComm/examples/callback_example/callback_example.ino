/* Example on how to use callback functions to process the received mesages.
*  
*  Two callbacks are created, one with a float from command 0x03 and another with a Pose from command 0x11.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#include <ArduComm.h>
#include <arducomm/types/pose.h>

ArduComm comms;


void cmd_callback(const float& data)
{
    uint8_t success = (uint8_t) (data == 4.0);
    comms.send(0x05, 1, &success);
}

void pose_callback(const Pose& pose_msg)
{
    uint8_t success = (uint8_t) (pose_msg.position.x == 2.0 &&
                                 pose_msg.orientation.w == 1.0);
    comms.send(0x06, 1, &success);
}

void setup()
{
    comms.begin(57600);
    comms.add_callback<float>(0x03, &cmd_callback);
    comms.add_callback<Pose>(0x11, &pose_callback);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);
}

void loop()
{
    if (comms.read())
    {
        digitalWrite(13, (comms.get_command() == 0x11));
    }
    delay(10);
}
