/* Vector2 type definition and serialization.
*  This type contains 2 floats (x, y).
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#ifndef ARDUCOMM__VECTOR2_H
#define ARDUCOMM__VECTOR2_H

#include <arducomm/serialization.h>

class Vector2
{
public:
    float x;
    float y;
    // Payload size
    const static uint8_t size = 8;

    Vector2()
    {
        x = 0.0;
        y = 0.0;
    }

    Vector2(const float& x, const float& y)
    {
        this->x = x;
        this->y = y;
    }

    ~Vector2(){}
};

namespace serialization
{

template<>
class Serializer<Vector2>
{
public:
    inline static void serialize(const Vector2& msg, uint8_t* buffer)
    {
        memcpy(buffer,                 (uint8_t*)(&msg.x), sizeof(float));
        memcpy(buffer + sizeof(float), (uint8_t*)(&msg.y), sizeof(float));
    }
    inline static void parse(uint8_t* buffer, Vector2& msg)
    {
        msg.x = *reinterpret_cast<float*>(buffer);
        msg.y = *reinterpret_cast<float*>(buffer + sizeof(float));
    }
    inline static Vector2 parse(uint8_t* buffer)
    {
        Vector2 msg;
        Serializer<Vector2>::parse(buffer, msg);
        return msg;
    }
};

} // namespace serialization

#endif