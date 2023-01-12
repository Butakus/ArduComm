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
uint8_t arr[4] = {5, 48, 100, 150};

void setup(){
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
    uint8_t error = comms.send(0x06, 4, arr);
    // Blink a led if there was an error in the communication
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