/* Pose type definition and serialization.
*  This type contains a Vector3 for translation and a Quaternion for orientation.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#ifndef ARDUCOMM__POSE_H
#define ARDUCOMM__POSE_H

#include <arducomm/serialization.h>
#include <arducomm/types/vector3.h>
#include <arducomm/types/quaternion.h>

class Pose
{
public:
    Vector3 position;
    Quaternion orientation;
    // Payload size
    static constexpr uint8_t size = Quaternion::size + Vector3::size;

    Pose() : position(), orientation()
    {
    }

    Pose(const Vector3& position, const Quaternion& orientation)
    {
        this->position = position;
        this->orientation = orientation;
    }

    ~Pose(){}
};

namespace serialization
{

template<>
class Serializer<Pose>
{
public:
    inline static void serialize(const Pose& msg, uint8_t* buffer)
    {
        Serializer<Vector3>::serialize(msg.position, buffer);
        Serializer<Quaternion>::serialize(msg.orientation, buffer + Vector3::size);
    }
    inline static void parse(uint8_t* buffer, Pose& msg)
    {
        Serializer<Vector3>::parse(buffer, msg.position);
        Serializer<Quaternion>::parse(buffer + Vector3::size, msg.orientation);
    }
    inline static Pose parse(uint8_t* buffer)
    {
        Pose msg;
        Serializer<Pose>::parse(buffer, msg);
        return msg;
    }
};

} // namespace serialization

#endif