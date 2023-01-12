from arducomm.serialization import Serializable, serialize, parse

class Quaternion(Serializable):
    """ Quaternion type definition and serialization """
    # Size in bytes
    size = 16

    def __init__(self, x=0.0, y=0.0, z=0.0, w=1.0):
        self.x = x
        self.y = y
        self.z = z
        self.w = w
    
    def __repr__(self):
        return F"[{self.x}, {self.y}, {self.z}, {self.w}]"

    def serialize(self):
        """ Serialize the data contained in self.
            Return a bytes-like object with the serialized data.
        """
        return (serialize(self.x, dtype='float') +
                serialize(self.y, dtype='float') +
                serialize(self.z, dtype='float') +
                serialize(self.w, dtype='float'))

    def parse(self, buffer):
        """ Parse (deserialize) the bytes from a buffer.
            Update the instance object and return self.
        """
        self.x = parse(buffer[:4], dtype='float')
        self.y = parse(buffer[4:8], dtype='float')
        self.z = parse(buffer[8:12], dtype='float')
        self.w = parse(buffer[12:], dtype='float')
        return self
