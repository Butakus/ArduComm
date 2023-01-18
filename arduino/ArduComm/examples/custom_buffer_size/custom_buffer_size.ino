/* Example on how to configure ArduComm class to use custom buffer sizes
*  
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#include <ArduComm.h>
#include <arducomm/types/pose.h>

/* To configure the template parameters for the sizes, we must use the ArduCommT class
*
*  We set the buffer size to 64 bytes, which is enough to receive a message with a pose (28 bytes + header).
*
*  The number of callbacks is set to 16. The received command will be 10, so this number
*  must be at least 11.
*  This way we can optimize the buffer sizes and reduce the amount of memory preallocated
*  within the ArduComm object.
*
*  Try to modify these values to see the difference in the usage of dynamic memory.
*/
ArduCommT<64, 12> comms;

void pose_callback(const Pose& pose_msg)
{
    uint8_t success = (uint8_t) (pose_msg.position.x == 2.0 &&
                                 pose_msg.orientation.w == 1.0);
    comms.send(0x06, 1, &success);
}

void setup()
{
    comms.begin(57600);
    comms.add_callback<Pose>(0x0A, &pose_callback);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);
}

void loop()
{
    comms.read();
    delay(10);
}
