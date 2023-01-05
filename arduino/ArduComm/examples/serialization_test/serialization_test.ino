#include <ArduComm.h>
#include <serialization/ArduParser.h>


ArduComm comms;
uint8_t data[64];

uint8_t error;

void setup()
{
    Serial.begin(57600);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    blink(1000);

    char c = 'A';
    serialize_char(c, data);
    error = comms.send(10, 1, data);
    handle_error(error);
    delay(1000);

    char s[5] = "test";
    serialize_string(s, data);
    error = comms.send(2, 5, data);
    handle_error(error);
    delay(1000);

    int8_t i8 = -100;
    serialize_int8(i8, data);
    error = comms.send(3, 1, data);
    handle_error(error);
    delay(1000);

    int16_t i16 = -1000;
    serialize_int16(i16, data);
    error = comms.send(4, 2, data);
    handle_error(error);
    delay(1000);

    uint16_t ui16 = 1000;
    serialize_uint16(ui16, data);
    error = comms.send(5, 2, data);
    handle_error(error);
    delay(1000);

    int32_t i32 = -100000;
    serialize_int32(i32, data);
    error = comms.send(6, 4, data);
    handle_error(error);
    delay(1000);

    uint32_t ui32 = 100000;
    serialize_uint32(ui32, data);
    error = comms.send(7, 4, data);
    handle_error(error);
    delay(1000);

    float f = 3.14159265;
    serialize_float(f, data);
    error = comms.send(8, 4, data);
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
