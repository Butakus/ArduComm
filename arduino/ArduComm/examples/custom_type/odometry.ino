/* This file defines a custom type and its serialization methods.
*
* The custom "complex" type is Odometry, defined in the odometry.ino file.
* The Odometry object is composed by a Pose defining a pose,
* a Vector3 for linear velocity and a Vector3 for angular velocity.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#include <ArduComm.h>
#include <arducomm/serialization.h>
#include <arducomm/types/pose.h>

class Odometry
{
public:
    Pose pose;
    Vector3 linear_vel;
    Vector3 angular_vel;
    // Payload size
    const static uint8_t size = Pose::size + 2*Vector3::size;

    Odometry() : pose(), linear_vel(), angular_vel()
    {
    }

    Odometry(const Pose& pose, const Vector3& linear_vel, const Vector3& angular_vel)
    {
        this->pose = pose;
        this->linear_vel = linear_vel;
        this->angular_vel = angular_vel;
    }

    ~Odometry(){}
};

/* Below are defined the serialization and parsing methods for the Odometry class.
   Note that we can reuse the serialization methods already implemented for other clases.
*/
namespace serialization
{

template<>
class Serializer<Odometry>
{
public:
    inline static void serialize(const Odometry& msg, uint8_t* buffer)
    {
        Serializer<Pose>::serialize(msg.pose, buffer);
        Serializer<Vector3>::serialize(msg.linear_vel, buffer + Pose::size);
        Serializer<Vector3>::serialize(msg.angular_vel, buffer + Pose::size + Vector3::size);
    }
    inline static void parse(uint8_t* buffer, Odometry& msg)
    {
        Serializer<Pose>::parse(buffer, msg.pose);
        Serializer<Vector3>::parse(buffer + Pose::size, msg.linear_vel);
        Serializer<Vector3>::parse(buffer + Pose::size + Vector3::size, msg.angular_vel);
    }
    inline static Odometry parse(uint8_t* buffer)
    {
        Odometry msg;
        Serializer<Odometry>::parse(buffer, msg);
        return msg;
    }
};

} // namespace serialization

