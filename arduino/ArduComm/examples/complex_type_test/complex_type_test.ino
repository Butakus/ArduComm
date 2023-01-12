#include <ArduComm.h>
#include <arducomm/serialization.h>
#include <arducomm/types/vector3.h>
#include <arducomm/types/quaternion.h>
#include <arducomm/types/pose.h>
#include <arducomm/types/float_array.h>
#include <arducomm/types/imu.h>

ArduComm comms;
uint8_t data[64];

uint8_t error;

void setup()
{
    Serial.begin(57600);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    blink(1000);

    // Pose serialization test
    Vector3 trans(5.3, 1.0, -7.4);
    Quaternion rot(0.0, 0.0, 0.0, 1.0);
    Pose p(trans, rot);

    serialize(p, data);
    error = comms.send(9, Pose::size, data);
    handle_error(error);
    delay(1000);

    // FloatArray serialization test
    const uint8_t fa_size = 5;
    FloatArray<fa_size> fa;
    fa[0] = 4.4;

    serialize(fa, data);
    error = comms.send(10, fa_size, data);
    handle_error(error);
    delay(1000);

    // Imu serialization test
    Imu imu;
    imu.angular_vel.z = 6.6;
    imu.linear_accel.x = 2.0;

    serialize(imu, data);
    error = comms.send(11, Imu::size, data);
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
                // Pose parsing test
                Pose pp = parse<Pose>(data);
                success = (uint8_t)(pp.position.x == 5.0);
                error = comms.send(command, 1, &success);
                break;
            }
            case 10:
            {
                // FloatArray parsing test
                // TODO: Problem: We must know the array size at compile time.
                FloatArray<5> ffaa = parse<FloatArray<5> >(data);
                success = (uint8_t)(ffaa[0] == 4.4);
                error = comms.send(command, 1, &success);
                break;
            }
            case 11:
            {
                // Imu parsing test
                Imu imu_rec = parse<Imu>(data);
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
