/* Imu type definition and serialization.
*  This type contains:
*       - Quaternion for orientation
*       - Vector3 for angular velocities (rad/s)
*       - Vector3 for linear acceleration (m/s^2)
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#ifndef ARDUCOMM__IMU_H
#define ARDUCOMM__IMU_H

#include <arducomm/serialization.h>
#include <arducomm/types/vector3.h>
#include <arducomm/types/quaternion.h>

class Imu
{
public:
    Quaternion orientation;
    Vector3 angular_vel;
    Vector3 linear_accel;
    // Payload size
    static constexpr uint8_t size = Quaternion::size + 2*Vector3::size;

    Imu() : orientation(), angular_vel(), linear_accel()
    {
    }

    Imu(const Quaternion& orientation,
        const Vector3& angular_vel,
        const Vector3& linear_accel)
    {
        this->orientation = orientation;
        this->angular_vel = angular_vel;
        this->linear_accel = linear_accel;
    }

    ~Imu(){}
};

namespace serialization
{

template<>
class Serializer<Imu>
{
public:
    inline static void serialize(const Imu& msg, uint8_t* buffer)
    {
        Serializer<Quaternion>::serialize(msg.orientation, buffer);
        Serializer<Vector3>::serialize(msg.angular_vel, buffer + Quaternion::size);
        Serializer<Vector3>::serialize(msg.linear_accel, buffer + Quaternion::size + Vector3::size);
    }
    inline static void parse(uint8_t* buffer, Imu& msg)
    {
        Serializer<Quaternion>::parse(buffer, msg.orientation);
        Serializer<Vector3>::parse(buffer + Quaternion::size, msg.angular_vel);
        Serializer<Vector3>::parse(buffer + Quaternion::size + Vector3::size, msg.linear_accel);
    }
    inline static Imu parse(uint8_t* buffer)
    {
        Imu msg;
        Serializer<Imu>::parse(buffer, msg);
        return msg;
    }
};

} // namespace serialization

#endif