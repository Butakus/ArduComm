#include <ArduComm.h>
#include <arducomm/serialization.h>
#include <arducomm/types/types.h>

ArduComm comms;
uint8_t payload[64];

uint8_t error;

void setup()
{
    Serial.begin(57600);
    comms.begin(&Serial);

    pinMode(13, OUTPUT);
    blink(1000);

    // char serialization test
    {
        char c = 'A';
        serialize(c, payload);
        error = comms.send(2, 1, payload);
        handle_error(error);
        delay(1000);
    }
    // String serialization test
    {
        String s = "test";
        serialize(s, payload);
        error = comms.send(3, 5, payload);
        handle_error(error);
        delay(1000);
    }
    // int8 serialization test
    {
        int8_t i8 = -100;
        serialize(i8, payload);
        error = comms.send(4, 1, payload);
        handle_error(error);
        delay(1000);
    }
    // int16 serialization test
    {
        int16_t i16 = -1000;
        serialize(i16, payload);
        error = comms.send(5, 2, payload);
        handle_error(error);
        delay(1000);
    }
    // uint16 serialization test
    {
        uint16_t ui16 = 1000;
        serialize(ui16, payload);
        error = comms.send(6, 2, payload);
        handle_error(error);
        delay(1000);
    }
    // int32 serialization test
    {
        int32_t i32 = -100000;
        serialize(i32, payload);
        error = comms.send(7, 4, payload);
        handle_error(error);
        delay(1000);
    }
    // uint32 serialization test
    {
        uint32_t ui32 = 100000;
        serialize(ui32, payload);
        error = comms.send(8, 4, payload);
        handle_error(error);
        delay(1000);
    }
    // float serialization test
    {
        float f = 3.14159265;
        serialize(f, payload);
        error = comms.send(9, 4, payload);
        handle_error(error);
        delay(1000);
    }
    // FloatArray serialization test
    {
        const uint8_t fa_size = 5;
        FloatArray<fa_size> fa;
        fa[0] = 1.1;
        fa[4] == 5.5;
        serialize(fa, payload);
        error = comms.send(10, fa_size, payload);
        handle_error(error);
        delay(1000);
    }
    // Vector2 serialization test
    {
        Vector2 v2(1.2, 1.3);
        serialize(v2, payload);
        error = comms.send(11, Vector2::size, payload);
        handle_error(error);
        delay(1000);
    }
    // Vector3 serialization test
    {
        Vector3 v3(2.2, 2.3, 2.4);
        serialize(v3, payload);
        error = comms.send(12, Vector3::size, payload);
        handle_error(error);
        delay(1000);
    }
    // Quaternion serialization test
    {
        Quaternion q(0.0, 0.0, 0.707, 0.707);
        serialize(q, payload);
        error = comms.send(13, Quaternion::size, payload);
        handle_error(error);
        delay(1000);
    }
    // Pose2D serialization test
    {
        Pose2D p2(4.4, 0.0, 0.6);
        serialize(p2, payload);
        error = comms.send(14, Pose2D::size, payload);
        handle_error(error);
        delay(1000);
    }
    // Pose serialization test
    {
        Vector3 trans(5.3, 1.0, -7.4);
        Quaternion rot(0.0, 0.0, 0.0, 1.0);
        Pose p(trans, rot);
        serialize(p, payload);
        error = comms.send(15, Pose::size, payload);
        handle_error(error);
        delay(1000);
    }
    // Imu serialization test
    {
        Imu imu;
        imu.angular_vel.z = 6.6;
        imu.linear_accel.x = 2.0;
        serialize(imu, payload);
        error = comms.send(16, Imu::size, payload);
        handle_error(error);
        delay(1000);
    }

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
