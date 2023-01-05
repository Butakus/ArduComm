""" Module to parse the bytes read with ArduComm into common types """

import struct


""" Parsers: byte list to common types: """


def parse_char(data):
    """ Convert the given 1-size byte list into a char """
    return chr(data[0])

def parse_str(data):
    """ Convert the given byte list into a string (char array)
        The string data must be ended in \0
    """
    return data.decode('utf8').rstrip('\x00')


def parse_int8(data, big_endian=False):
    """ Convert 1-size byte list into a 8 bit signed int """
    byte_format = '>b' if big_endian else '<b'
    return struct.unpack(byte_format, bytes(data))[0]

def parse_int16(data, big_endian=False):
    """ Convert 2 bytes into a 16 bit int """
    byte_format = '>h' if big_endian else '<h'
    return struct.unpack(byte_format, bytes(data))[0]

def parse_uint16(data, big_endian=False):
    """ Convert 2 bytes into an unsigned 16 bit int """
    byte_format = '>H' if big_endian else '<H'
    return struct.unpack(byte_format, bytes(data))[0]

def parse_int32(data, big_endian=False):
    """ Convert 4 bytes into a 32 bit int """
    byte_format = '>i' if big_endian else '<i'
    return struct.unpack(byte_format, bytes(data))[0]

def parse_uint32(data, big_endian=False):
    """ Convert 4 bytes into an unsigned 32 bit int """
    byte_format = '>I' if big_endian else '<I'
    return struct.unpack(byte_format, bytes(data))[0]

def parse_float(data, big_endian=False):
    """ Convert 4 bytes into a 32 bit float """
    byte_format = '>f' if big_endian else '<f'
    return struct.unpack(byte_format, bytes(data))[0]


""" Serializers: common types to byte list: """


def serialize_char(data):
    """ Convert the given char into a bytes object """
    return bytes(data, 'utf8')

def serialize_str(data):
    """ Convert the given string into a bytes object """
    # Sanitize the input first
    if data[-1] != '\x00':
        data += '\x00'
    return bytes(data, 'utf8')

def serialize_int8(data, big_endian=False):
    """ Convert a 8 bit signed int into a bytes object """
    byte_format = '>b' if big_endian else '<b'
    return struct.pack(byte_format, data)

def serialize_int16(data, big_endian=False):
    """ Convert a 16 bit int into 2 bytes """
    byte_format = '>h' if big_endian else '<h'
    return struct.pack(byte_format, data)

def serialize_uint16(data, big_endian=False):
    """ Convert an unsigned 16 bit int into 2 bytes """
    byte_format = '>H' if big_endian else '<H'
    return struct.pack(byte_format, data)

def serialize_int32(data, big_endian=False):
    """ Convert a 32 bit int into 4 bytes """
    byte_format = '>i' if big_endian else '<i'
    return struct.pack(byte_format, data)

def serialize_uint32(data, big_endian=False):
    """ Convert an unsigned 32 bit int into 4 bytes """
    byte_format = '>I' if big_endian else '<I'
    return struct.pack(byte_format, data)

def serialize_float(data, big_endian=False):
    """ Convert a 32 bit float into 4 bytes """
    byte_format = '>f' if big_endian else '<f'
    return struct.pack(byte_format, data)
