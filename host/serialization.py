""" Module to serialize and parse the bytes read with ArduComm into common types """

import struct
from abc import ABC, abstractmethod

class Serializable(ABC):
    """ Base class that defines the required serialization methods.
        All classes meant to be serialized and sent whtough ArduComm must
        inherit from this base class and implement its abstract methods.
    """
    def __init__(self):
        """ TODO: Docstring """
        raise NotImplemented()

    @abstractmethod
    def serialize(self):
        """ Serialize the data contained in self.
            Return a bytes-like object with the serialized data.
        """
        pass

    @abstractmethod
    def parse(self, buffer):
        """ Parse (deserialize) the bytes from a buffer.
            Return a bytes-like object with the serialized data.
        """
        pass


""" Serialization of numbers.
"""
_num_fmt = {
    'uint8': '<B',
    'int8': '<b',
    'uint16': '<H',
    'int16': '<h',
    'uint32': '<I',
    'int32': '<i',
    'float': '<f',
}

def serialize_num(data, dtype):
    """ Convert a number into a bytes object """
    return struct.pack(_num_fmt[dtype], data)

def parse_num(buffer, dtype):
    """ Convert byte list into a number """
    return struct.unpack(_num_fmt[dtype], bytes(buffer))[0]


""" Serialization of chars and strings (null terminated) """
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
        if dtype in _num_fmt:
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
        if dtype in _num_fmt:
            return parse_num(buffer, dtype)
        elif dtype == 'str' or dtype == 'char':
            return parse_str(buffer)
        else:
            raise ValueError(F"Unknown dtype: '{dtype}'")
    else:
        # Assume data is a Serializable object and provides 'parse' method
        return data.parse(buffer)
