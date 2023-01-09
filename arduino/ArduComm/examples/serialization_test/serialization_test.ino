#include <ArduComm.h>
#include <arducomm/serialization.h>


ArduComm comms;
uint8_t payload[64];

uint8_t error;

void setup()
{
    Serial.begin(57600);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    blink(1000);

    char c = 'A';
    serialize(c, payload);
    error = comms.send(10, 1, payload);
    handle_error(error);
    delay(1000);

    String s = "test";
    serialize(s, payload);
    error = comms.send(2, 5, payload);
    handle_error(error);
    delay(1000);

    int8_t i8 = -100;
    serialize(i8, payload);
    error = comms.send(3, 1, payload);
    handle_error(error);
    delay(1000);

    int16_t i16 = -1000;
    serialize(i16, payload);
    error = comms.send(4, 2, payload);
    handle_error(error);
    delay(1000);

    uint16_t ui16 = 1000;
    serialize(ui16, payload);
    error = comms.send(5, 2, payload);
    handle_error(error);
    delay(1000);

    int32_t i32 = -100000;
    serialize(i32, payload);
    error = comms.send(6, 4, payload);
    handle_error(error);
    delay(1000);

    uint32_t ui32 = 100000;
    serialize(ui32, payload);
    error = comms.send(7, 4, payload);
    handle_error(error);
    delay(1000);

    float f = 3.14159265;
    serialize(f, payload);
    error = comms.send(8, 4, payload);
    handle_error(error);
    delay(1000);

    blink(200);
    blink(200);
    blink(200);
}

void loop()
{
    delay(100);
}

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
