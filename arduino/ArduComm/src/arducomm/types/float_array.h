/* FloatArray type definition and serialization.
*  This type contains an array of floats of dynamic size.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#ifndef ARDUCOMM__FLOAT_ARRAY_H
#define ARDUCOMM__FLOAT_ARRAY_H

#include <arducomm/serialization.h>

template <uint8_t DATA_SIZE>
class FloatArray
{
public:
    float data[DATA_SIZE];
    // Payload size
    static constexpr uint8_t size = DATA_SIZE * sizeof(float);

    FloatArray() {}

    FloatArray(float* data)
    {
        for (uint8_t i = 0; i < DATA_SIZE; i++)
        {
            this->data = data[i];
        }
    }

    float& operator[](const uint8_t& index)
    {
        return data[index];
    }

    ~FloatArray(){}
};

namespace serialization
{

template<uint8_t DATA_SIZE>
class Serializer<FloatArray<DATA_SIZE> >
{
public:
    inline static void serialize(const FloatArray<DATA_SIZE>& msg, uint8_t* buffer)
    {
        memcpy(buffer, (uint8_t*)(msg.data), DATA_SIZE * sizeof(float));
    }
    inline static void parse(uint8_t* buffer, FloatArray<DATA_SIZE>& msg)
    {
        for (uint8_t i = 0; i < DATA_SIZE; i++)
        {
            msg.data[i] = *reinterpret_cast<float*>(buffer + (i*sizeof(float)));
        }
    }
    inline static FloatArray<DATA_SIZE> parse(uint8_t* buffer)
    {
        FloatArray<DATA_SIZE> msg;
        Serializer<FloatArray<DATA_SIZE> >::parse(buffer, msg);
        return msg;
    }
};

} // namespace serialization

#endif