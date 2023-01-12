from arducomm.serialization import Serializable, serialize, parse
import struct

class FloatArray(Serializable):
    """ FloatArray type definition and serialization """
    def __init__(self, data=[]):
        self.data = data
    
    def __repr__(self):
        return self.data.__repr__()

    def serialize(self):
        """ Serialize the data contained in self.
            Return a bytes-like object with the serialized data.
        """
        return struct.pack(F"<{len(self.data)}f", *self.data)

    def parse(self, buffer):
        """ Parse (deserialize) the bytes from a buffer.
            Update the instance object and return self.
        """
        size = int(len(buffer) / 4)
        self.data = list(struct.unpack(F"<{size}f", buffer))
        return self
