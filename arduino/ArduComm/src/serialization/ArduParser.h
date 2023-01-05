/* Module to parse and serialize the data from/into the bytes received/to send
*
*  Note: Works only on version 1.0 and above of Arduino IDE. (only arduino.h)
*
*  Author:  Francisco Miguel Moreno
*  Email:   butakus@gmail.com
*  Version: 0.2
*  Date:    April 2017
*/

#ifndef ArduParser_h
#define ArduParser_h

#include <Arduino.h>

/* Parsers: byte arrays (data) to common types: */

// Convert the first byte of the array into a char
char parse_char(uint8_t* data);
/* Convert the given byte array into a string (char array)
*   The string data must be ended in \0
*   Returns: Length of the string
*/
uint8_t parse_string(uint8_t* data, char* out_buffer);

// Convert the first byte of the array into a 8 bit signed int
int8_t parse_int8(uint8_t* data);
// Convert the first 2 bytes of the array into a 16 bit unsigned int
uint16_t parse_uint16(uint8_t* data);
// Convert the first 2 bytes of the array into a 16 bit signed int
int16_t parse_int16(uint8_t* data);
// Convert the first 4 bytes of the array into a 32 bit unsigned int
uint32_t parse_uint32(uint8_t* data);
// Convert the first 4 bytes of the array into a 32 bit signed int
int32_t parse_int32(uint8_t* data);

// Convert the first 4 bytes of the array into a 32 bit float
float parse_float(uint8_t* data);


/* Serializers: common types (data) to byte arrays (buffer): */

// Copy the given char byte into the buffer
void serialize_char(char& data, uint8_t* buffer);
// Copy the given char array (string) into the buffer. The string data must be ended in \0
void serialize_string(char* data, uint8_t* buffer);

// Copy the given 8 bit signed int into the buffer
void serialize_int8(int8_t& data, uint8_t* buffer);
// Copy the given 16 bit unsigned int into the buffer
void serialize_uint16(uint16_t& data, uint8_t* buffer);
// Copy the given 16 bit signed int into the buffer
void serialize_int16(int16_t& data, uint8_t* buffer);
// Copy the given 32 bit unsigned int into the buffer
void serialize_uint32(uint32_t& data, uint8_t* buffer);
// Copy the given 32 bit signed int into the buffer
void serialize_int32(int32_t& data, uint8_t* buffer);

// Copy the given 32 bit float into the buffer
void serialize_float(float& data, uint8_t* buffer);

#endif