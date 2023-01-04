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

#include <ArduComm.h>

ArduComm comms;
uint8_t payload[64];

void setup()
{
    Serial.begin(BAUDRATE);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);

    comms.send(0x03);

    delay(1000);

    uint8_t arr[4] = {5, 10, 15, 20};
    comms.send(0x06, 4, arr);
}

void loop()
{
    if (comms.read())
    {
        uint8_t command = comms.get_command();
        uint8_t payload_size = comms.get_payload(payload);
        if (command == 0x03 || command == 0x7E)
        {
            if (payload_size == 2)
            {
                if (payload[0] > payload[1])
                {
                    digitalWrite(13, HIGH);
                }
            }
            else
            {
                digitalWrite(13, HIGH);
            }
        }
        else if (command == 0x00)
        {
            digitalWrite(13, LOW);
        }
    }
    delay(5);
}