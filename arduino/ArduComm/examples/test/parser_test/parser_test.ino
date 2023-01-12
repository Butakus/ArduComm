#include <ArduComm.h>
#include <arducomm/serialization.h>
#include <arducomm/types/types.h>

ArduComm comms;
uint8_t payload[64];


void setup()
{
    Serial.begin(57600);
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
        uint8_t payload_size = comms.get_payload(payload);

        switch (command)
        {
            case 2:
            {
                // Char test
                char c = parse<char>(payload);
                success = (uint8_t)(c == 'A');
                error = comms.send(command, 1, &success);
                break;
            }
            case 3:
            {
                // String test
                String s = parse<String>(payload);
                success = s == "test";
                error = comms.send(command, 1, &success);
                break;
            }
            case 4:
            {
                // int8 test
                int8_t i8 = parse<int8_t>(payload);
                success = (uint8_t)(i8 == -100);
                error = comms.send(command, 1, &success);
                break;
            }
            case 5:
            {
                // int16 test
                int16_t i16 = parse<int16_t>(payload);
                success = (uint8_t)(i16 == -1000);
                error = comms.send(command, 1, &success);
                break;
            }
            case 6:
            {
                // uint16 test
                int16_t ui16 = parse<uint16_t>(payload);
                success = (uint8_t)(ui16 == 1000);
                error = comms.send(command, 1, &success);
                break;
            }
            case 7:
            {
                // int32 test
                int32_t i32 = parse<int32_t>(payload);
                success = (uint8_t)(i32 == -100000);
                error = comms.send(command, 1, &success);
                break;
            }
            case 8:
            {
                // uint32 test
                uint32_t ui32 = parse<uint32_t>(payload);
                success = (uint8_t)(ui32 == 100000);
                error = comms.send(command, 1, &success);
                break;
            }
            case 9:
            {
                // float test
                float f = parse<float>(payload);
                success = (uint8_t)(f == 3.14159265);
                error = comms.send(command, 1, &success);
                break;
            }
            case 10:
            {
                // FloatArray test
                // TODO: Problem: We must know the array size at compile time.
                FloatArray<5> fa = parse<FloatArray<5> >(payload);
                success = (uint8_t)(fa[0] == 1.1 && fa[4] == 5.5);
                error = comms.send(command, 1, &success);
                break;
            }
            case 11:
            {
                // Vector2 test
                Vector2 v2 = parse<Vector2>(payload);
                success = (uint8_t)(v2.x = 1.2 && v2.y == 1.3);
                error = comms.send(command, 1, &success);
                break;
            }
            case 12:
            {
                // Vector3 test
                Vector3 v3 = parse<Vector3>(payload);
                success = (uint8_t)(v3.x = 2.2 && v3.y == 2.3 && v3.z == 2.4);
                error = comms.send(command, 1, &success);
                break;
            }
            case 13:
            {
                // Quaternion test
                Quaternion q = parse<Quaternion>(payload);
                success = (uint8_t)(q.x == 0.0 && q.y == 0.0 && q.z == 0.707 && q.w == 0.707);
                error = comms.send(command, 1, &success);
                break;
            }
            case 14:
            {
                // Pose2D parsing test
                Pose2D p = parse<Pose2D>(payload);
                success = (uint8_t)(p.x == 4.4 && p.theta == 0.6);
                error = comms.send(command, 1, &success);
                break;
            }
            case 15:
            {
                // Pose parsing test
                Pose p = parse<Pose>(payload);
                success = (uint8_t)(p.position.x == 5.3 && p.orientation.w == 1.0);
                error = comms.send(command, 1, &success);
                break;
            }
            case 16:
            {
                // Imu parsing test
                Imu imu_rec = parse<Imu>(payload);
                success = (uint8_t)(imu_rec.angular_vel.z == 6.6 && imu_rec.linear_accel.x == 2.0);
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
