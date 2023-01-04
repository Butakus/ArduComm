#include <ArduParser.h>
#include <ArduComm.h>


ArduComm comms;
uint8_t data[64];


void setup()
{
    Serial.begin(BAUDRATE);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);

}

void loop()
{
    uint8_t error = NO_ERROR;
    uint8_t success;
    // Read
    if (comms.read())
    {
        uint8_t command = comms.get_command();
        uint8_t payload_size = comms.get_payload(data);

        switch (command)
        {
            case 10:
            {
                // Char test
                char c = parse_char(&data[0]);
                success = (uint8_t)(c == 'A');
                error = comms.send(command, 1, &success);
                break;
            }
            case 2:
            {
                // String test
                char s[10];
                uint8_t length = parse_string(&data[0], s);
                success = (strcmp(s, "test") == 0) && (length == 5);
                error = comms.send(command, 1, &success);
                break;
            }
            case 3:
            {
                // int8 test
                int8_t i8 = parse_int8(&data[0]);
                success = (uint8_t)(i8 == -100);
                error = comms.send(command, 1, &success);
                break;
            }
            case 4:
            {
                // int16 test
                int16_t i16 = parse_int16(&data[0]);
                success = (uint8_t)(i16 == -1000);
                error = comms.send(command, 1, &success);
                break;
            }
            case 5:
            {
                // uint16 test
                int16_t ui16 = parse_uint16(&data[0]);
                success = (uint8_t)(ui16 == 1000);
                error = comms.send(command, 1, &success);
                break;
            }
            case 6:
            {
                // int32 test
                int32_t i32 = parse_int32(&data[0]);
                success = (uint8_t)(i32 == -100000);
                error = comms.send(command, 1, &success);
                break;
            }
            case 7:
            {
                // uint32 test
                uint32_t ui32 = parse_uint32(&data[0]);
                success = (uint8_t)(ui32 == 100000);
                error = comms.send(command, 1, &success);
                break;
            }
            case 8:
            {
                // float test
                float f = parse_float(&data[0]);
                success = (uint8_t)(f == 3.14159265);
                error = comms.send(command, 1, &success);
                break;
            }
            default:
                success = 0;
                error = comms.send(command, 1, &success);
                break;
        }
    }

    // Handle error
    // Blink a led if there was an error in the communication
    switch (error)
    {
        case TIMEOUT_ERROR: blink(1000); break;
        case ACK_ERROR: blink(2000); break;
        default: break;
    }
    //delay(10);

}

void blink(unsigned long t)
{
    digitalWrite(13, HIGH);
    delay(t);
    digitalWrite(13, LOW);
    delay(t);
}
