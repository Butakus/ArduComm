/* Vector3 type definition and serialization.
*  This type contains 3 floats (x, y, z).
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#ifndef ARDUCOMM__VECTOR3_H
#define ARDUCOMM__VECTOR3_H

#include <arducomm/serialization.h>

class Vector3
{
public:
    float x;
    float y;
    float z;
    // Payload size
    const static uint8_t size = 3*sizeof(float);

    Vector3()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }

    Vector3(const float& x, const float& y, const float& z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    ~Vector3(){}
};

namespace serialization
{

template<>
class Serializer<Vector3>
{
public:
    inline static void serialize(const Vector3& msg, uint8_t* buffer)
    {
        memcpy(buffer,                   (uint8_t*)(&msg.x), sizeof(float));
        memcpy(buffer + sizeof(float),   (uint8_t*)(&msg.y), sizeof(float));
        memcpy(buffer + 2*sizeof(float), (uint8_t*)(&msg.z), sizeof(float));
    }
    inline static void parse(uint8_t* buffer, Vector3& msg)
    {
        msg.x = *reinterpret_cast<float*>(buffer);
        msg.y = *reinterpret_cast<float*>(buffer + sizeof(float));
        msg.z = *reinterpret_cast<float*>(buffer + 2*sizeof(float));
    }
    inline static Vector3 parse(uint8_t* buffer)
    {
        Vector3 msg;
        Serializer<Vector3>::parse(buffer, msg);
        return msg;
    }
};

} // namespace serialization

#endif