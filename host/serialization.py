""" Module to serialize and parse the bytes read with ArduComm into common types """

import struct
from abc import ABC, abstractmethod

class Serializable(ABC):
    """ TODO: Docstring """
    def __init__(self):
        """ TODO: Docstring """
        raise NotImplemented()

    @abstractmethod
    def serialize(self):
        pass

    @abstractmethod
    def parse(self, buffer):
        pass

_num_fmt = {
    'uint8': '<B',
    'int8': '<b',
    'uint16': '<h',
    'int16': '<H',
    'uint32': '<i',
    'int32': '<I',
    'float': '<f',
}
def serialize_num(data, dtype):
    """ Convert a number into a bytes object """
    return struct.pack(_num_fmt[dtype], data)

def parse_num(buffer, dtype):
    """ Convert byte list into a number """
    return struct.unpack(_num_fmt[dtype], bytes(buffer))[0]

def serialize_char(data):
    """ Convert the given char into a bytes object """
    return bytes(data, 'utf8')

def serialize_str(data):
    """ Convert the given string into a bytes object """
    # Sanitize the input first
    if data[-1] != '\x00':
        data += '\x00'
    return bytes(data, 'utf8')

def parse_str(buffer):
    """ Convert the given byte list into a string (char array)
        The string data might be null terminated, but not necessary
    """
    return buffer.decode('utf8').rstrip('\x00')


def serialize(data, dtype=Serializable):
    if dtype is not Serializable:
        if dtype in _mun_fmt:
            return serialize_num(data, dtype)
        elif dtype == 'char':
            return serialize_char(data)
        elif dtype == 'str':
            return serialize_str(data)
        else:
            raise ValueError(F"Unknown dtype: '{dtype}'")
    else:
        # Assume data is a Serializable object and provides 'serialize' method
        return data.serialize()

def parse(buffer, dtype=Serializable):
    if dtype is not Serializable:
        if dtype in _mun_fmt:
            return parse_num(buffer, dtype)
        elif dtype == 'str' or dtype == 'char':
            return parse_str(buffer)
        else:
            raise ValueError(F"Unknown dtype: '{dtype}'")
    else:
        # Assume data is a Serializable object and provides 'parse' method
        return data.parse(buffer)
