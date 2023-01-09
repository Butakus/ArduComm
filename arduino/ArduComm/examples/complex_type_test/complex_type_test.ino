#include <ArduComm.h>
#include <arducomm/serialization.h>
#include <arducomm/types/vector3.h>

ArduComm comms;
uint8_t data[64];

uint8_t error;

void setup()
{
    Serial.begin(57600);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    blink(1000);


    Vector3 v(5.3, 1.0, -7.4);
    serialize(v, data);
    error = comms.send(9, Vector3::size, data);
    handle_error(error);
    delay(1000);

    blink(200);
    blink(200);
    blink(200);
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
            case 9:
            {
                // Vector3 parsing test
                Vector3 vv = parse<Vector3>(data);
                success = (uint8_t)(vv.x == 5.0);
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
