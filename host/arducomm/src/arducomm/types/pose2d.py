from arducomm.serialization import Serializable, serialize, parse

class Pose2D(Serializable):
    """ Pose2D type definition and serialization """
    # Size in bytes
    size = 12

    def __init__(self, x=0.0, y=0.0, theta=0.0):
        self.x = x
        self.y = y
        self.theta = theta
    
    def __repr__(self):
        return F"[{self.x}, {self.y} | {self.theta}]"

    def serialize(self):
        """ Serialize the data contained in self.
            Return a bytes-like object with the serialized data.
        """
        return (serialize(self.x, dtype='float') +
                serialize(self.y, dtype='float') +
                serialize(self.theta, dtype='float'))

    def parse(self, buffer):
        """ Parse (deserialize) the bytes from a buffer.
            Update the instance object and return self.
        """
        self.x = parse(buffer[:4], dtype='float')
        self.y = parse(buffer[4:8], dtype='float')
        self.theta = parse(buffer[8:], dtype='float')
        return self
