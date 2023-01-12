from arducomm.serialization import Serializable, serialize, parse

class Vector2(Serializable):
    """ Vector2 type definition and serialization """
    # Size in bytes
    size = 8

    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y
    
    def __repr__(self):
        return F"[{self.x}, {self.y}]"

    def serialize(self):
        """ Serialize the data contained in self.
            Return a bytes-like object with the serialized data.
        """
        return serialize(self.x, dtype='float') + serialize(self.y, dtype='float')

    def parse(self, buffer):
        """ Parse (deserialize) the bytes from a buffer.
            Update the instance object and return self.
        """
        self.x = parse(buffer[:4], dtype='float')
        self.y = parse(buffer[4:], dtype='float')
        return self
