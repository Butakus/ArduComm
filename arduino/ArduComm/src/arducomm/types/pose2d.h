/* Pose2D type definition and serialization.
*  This type contains 3 floats (x, y, theta).
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#ifndef ARDUCOMM__POSE2D_H
#define ARDUCOMM__POSE2D_H

#include <arducomm/serialization.h>

class Pose2D
{
public:
    float x;
    float y;
    float theta;
    // Payload size
    static constexpr uint8_t size = 3*sizeof(float);

    Pose2D()
    {
        x = 0.0;
        y = 0.0;
        theta = 0.0;
    }

    Pose2D(const float& x, const float& y, const float& theta)
    {
        this->x = x;
        this->y = y;
        this->theta = theta;
    }

    ~Pose2D(){}
};

namespace serialization
{

template<>
class Serializer<Pose2D>
{
public:
    inline static void serialize(const Pose2D& msg, uint8_t* buffer)
    {
        memcpy(buffer,     (uint8_t*)(&msg.x), sizeof(float));
        memcpy(buffer + 4, (uint8_t*)(&msg.y), sizeof(float));
        memcpy(buffer + 8, (uint8_t*)(&msg.theta), sizeof(float));
    }
    inline static void parse(uint8_t* buffer, Pose2D& msg)
    {
        msg.x = *reinterpret_cast<float*>(buffer);
        msg.y = *reinterpret_cast<float*>(buffer + 4);
        msg.theta = *reinterpret_cast<float*>(buffer + 8);
    }
    inline static Pose2D parse(uint8_t* buffer)
    {
        Pose2D msg;
        Serializer<Pose2D>::parse(buffer, msg);
        return msg;
    }
};

} // namespace serialization

#endif