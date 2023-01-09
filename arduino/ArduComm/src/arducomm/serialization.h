/* Serialization and parsing of basic types via templates.
*  These functions are intended to be used to serialize the data to send
*  and to parse the data received in the message payload.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/

#ifndef ARDUCOMM__SERIALIZATION_H
#define ARDUCOMM__SERIALIZATION_H

namespace serialization
{

/* Base Serializer class
   New types must implement a specialization of this class
   i.e.: class Serializer<NewType> {...};
   and define the serialize and parse methods.
*/
template<class Serializable>
class Serializer
{
public:
    inline static void serialize(const Serializable& data, uint8_t* buffer);
    inline static void parse(uint8_t* buffer, Serializable& data);
    inline static Serializable parse(uint8_t* buffer);
};

/* Specializations of the Serializer class for basic types */
#define ARDUCOMM_CREATE_BASIC_SERIALIZER(T) \
template<> class Serializer<T> \
{ \
public: \
    inline static void serialize(const T& data, uint8_t* buffer) \
    { \
        memcpy(buffer, (uint8_t*)(&data), sizeof(T)); \
    } \
    \
    inline static void parse(uint8_t* buffer, T& data) \
    { \
        data = *reinterpret_cast<T*>(buffer); \
    } \
    \
    inline static T parse(uint8_t* buffer) \
    { \
        return *reinterpret_cast<T*>(buffer); \
    } \
};

ARDUCOMM_CREATE_BASIC_SERIALIZER(char)
ARDUCOMM_CREATE_BASIC_SERIALIZER(uint8_t)
ARDUCOMM_CREATE_BASIC_SERIALIZER(int8_t)
ARDUCOMM_CREATE_BASIC_SERIALIZER(uint16_t)
ARDUCOMM_CREATE_BASIC_SERIALIZER(int16_t)
ARDUCOMM_CREATE_BASIC_SERIALIZER(uint32_t)
ARDUCOMM_CREATE_BASIC_SERIALIZER(int32_t)
ARDUCOMM_CREATE_BASIC_SERIALIZER(float)

/* Special case: Strings */
/* TODO: Decide what to do with this specific case.
   For now use Arduino String class (inefficient but ok).
*/
template<> class Serializer<String>
{
public:
    inline static void serialize(const String& data, uint8_t* buffer)
    {
        // Use provided getBytes function:
        data.getBytes(buffer, data.length() + 1);
    }
    inline static void parse(uint8_t* buffer, String& data)
    {
        data = String((char*)buffer);
    }
    inline static String parse(uint8_t* buffer)
    {
        return String((char*)buffer);
    }
};

} // namespace serialization

/* Serialization and parsing functions
   These are the functions that will be directly used.
*/
template <typename T>
void serialize(const T& data, uint8_t* buffer)
{
    serialization::Serializer<T>::serialize(data, buffer);
}

template <typename T>
void parse(uint8_t* buffer, T& data)
{
    return serialization::Serializer<T>::parse(buffer, data);
}

template <typename T>
T parse(uint8_t* buffer)
{
    return serialization::Serializer<T>::parse(buffer);
}

#endif
