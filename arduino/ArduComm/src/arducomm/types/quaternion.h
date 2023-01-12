/* Quaternion type definition and serialization.
*  This type contains 4 floats (x, y, z, w).
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#ifndef ARDUCOMM__QUATERNION_H
#define ARDUCOMM__QUATERNION_H

#include <arducomm/serialization.h>

class Quaternion
{
public:
    float x;
    float y;
    float z;
    float w;
    // Payload size
    const static uint8_t size = 4*sizeof(float);

    Quaternion()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
        w = 1.0;
    }

    Quaternion(const float& x, const float& y,
               const float& z, const float& w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    ~Quaternion(){}
};

namespace serialization
{

template<>
class Serializer<Quaternion>
{
public:
    inline static void serialize(const Quaternion& msg, uint8_t* buffer)
    {
        memcpy(buffer,                   (uint8_t*)(&msg.x), sizeof(float));
        memcpy(buffer + sizeof(float),   (uint8_t*)(&msg.y), sizeof(float));
        memcpy(buffer + 2*sizeof(float), (uint8_t*)(&msg.z), sizeof(float));
        memcpy(buffer + 3*sizeof(float), (uint8_t*)(&msg.w), sizeof(float));
    }
    inline static void parse(uint8_t* buffer, Quaternion& msg)
    {
        msg.x = *reinterpret_cast<float*>(buffer);
        msg.y = *reinterpret_cast<float*>(buffer + sizeof(float));
        msg.z = *reinterpret_cast<float*>(buffer + 2*sizeof(float));
        msg.w = *reinterpret_cast<float*>(buffer + 3*sizeof(float));
    }
    inline static Quaternion parse(uint8_t* buffer)
    {
        Quaternion msg;
        Serializer<Quaternion>::parse(buffer, msg);
        return msg;
    }
};

} // namespace serialization

#endif