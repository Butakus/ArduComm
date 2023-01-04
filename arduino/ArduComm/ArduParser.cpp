/* Module to parse and serialize the data from/into the bytes received/to send
*
*  Note: Works only on version 1.0 and above of Arduino IDE. (only arduino.h)
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    April 2017
*/

#include "ArduParser.h"


char parse_char(uint8_t* data)
{
    return (char) *data;
}

uint8_t parse_string(uint8_t* data, char* out_buffer)
{
    int i = 0;
    while (data[i] != 0)
    {
        out_buffer[i] = (char) data[i];
        ++i;
    }
    out_buffer[i] = 0;
    return i + 1;
}


int8_t parse_int8(uint8_t* data)
{
    return (uint8_t) *data;
}

uint16_t parse_uint16(uint8_t* data)
{
    return *(uint16_t*) data;
}

int16_t parse_int16(uint8_t* data)
{
    return *(int16_t*) data;
}

uint32_t parse_uint32(uint8_t* data)
{
    return *(uint32_t*) data;
}

int32_t parse_int32(uint8_t* data)
{
    return *(int32_t*) data;
}


float parse_float(uint8_t* data)
{
    return *(float*) data;
}


void serialize_char(char& data, uint8_t* buffer)
{
    buffer[0] = (uint8_t)data;
}

void serialize_string(char* data, uint8_t* buffer)
{
    int i = 0;
    while (data[i] != 0)
    {
        buffer[i] = (uint8_t)data[i];
        ++i;
    }
    buffer[i] = 0;
}


void serialize_int8(int8_t& data, uint8_t* buffer)
{
    buffer[0] = (uint8_t)data;
}

void serialize_uint16(uint16_t& data, uint8_t* buffer)
{
    memcpy(buffer, (unsigned char*)(&data), 2);
}

void serialize_int16(int16_t& data, uint8_t* buffer)
{
    memcpy(buffer, (unsigned char*)(&data), 2);
}

void serialize_uint32(uint32_t& data, uint8_t* buffer)
{
    memcpy(buffer, (unsigned char*)(&data), 4);
}

void serialize_int32(int32_t& data, uint8_t* buffer)
{
    memcpy(buffer, (unsigned char*)(&data), 4);
}


void serialize_float(float& data, uint8_t* buffer)
{
    memcpy(buffer, (unsigned char*)(&data), 4);
}