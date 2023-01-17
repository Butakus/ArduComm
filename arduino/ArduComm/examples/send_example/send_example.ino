/* Simple example to send data with and without payload using ArduComm library.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#include <ArduComm.h>

ArduComm comms;
const uint8_t data_size = 4;
uint8_t data_arr[data_size] = {5, 48, 100, 150};

void setup()
{
    // Initialize connection
    comms.begin(57600);
    delay(1000);
    // Send the single command 0x03 without any payload
    comms.send(0x03);
    delay(1000);
    // Send the command 6, with the payload bytes defined in data_arr
    comms.send(0x06, data_size, data_arr);
}
void loop() {}
