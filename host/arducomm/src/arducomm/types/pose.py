from arducomm.serialization import Serializable, serialize, parse
from .vector3 import Vector3
from .quaternion import Quaternion

class Pose(Serializable):
    """ Pose type definition and serialization """
    # Size in bytes
    size = Vector3.size + Quaternion.size

    def __init__(self, position=Vector3(), orientation=Quaternion()):
        self.position = position
        self.orientation = orientation
    
    def __repr__(self):
        return F"{self.position.__repr__()} | {self.orientation.__repr__()}"

    def serialize(self):
        """ Serialize the data contained in self.
            Return a bytes-like object with the serialized data.
        """
        return serialize(self.position) + serialize(self.orientation)

    def parse(self, buffer):
        """ Parse (deserialize) the bytes from a buffer.
            Update the instance object and return self.
        """
        self.position.parse(buffer[:Vector3.size])
        self.orientation.parse(buffer[Vector3.size:])
        return self
