/*  Serialization and parsing of basic types via templates.
*   These functions are intended to be used to serialize the data to send
*   and to parse the data received in the message payload.
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    January 2023
*/


/****************
* SERIALIZATION *
*****************/

template <typename T>
void serialize(T& data, uint8_t* buffer)
{
    memcpy(buffer, (uint8_t*)(&data), sizeof(T));
}

/****************
*    PARSING    *
*****************/

template <typename T>
T parse(uint8_t* data)
{
    return *reinterpret_cast<T*>(data);
}

/* Special case: Strings */
/* TODO: Decide what to do with this specific case.
   For now use Arduino String class (inefficient but ok).
*/
template <>
void serialize<String>(String& data, uint8_t* buffer)
{
    // Use provided getBytes function:
    data.getBytes(buffer, data.length() + 1);
}

template <>
String parse<String>(uint8_t* data)
{
    return String((char*)data);
}
