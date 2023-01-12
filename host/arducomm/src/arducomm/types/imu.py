from arducomm.serialization import Serializable, serialize, parse
from .vector3 import Vector3
from .quaternion import Quaternion

class Imu(Serializable):
    """ Imu type definition and serialization """
    # Size in bytes
    size = Quaternion.size + 2*Vector3.size

    def __init__(self, orientation=Quaternion(), angular_vel=Vector3(), linear_accel=Vector3()):
        self.orientation = orientation
        self.angular_vel = angular_vel
        self.linear_accel = linear_accel
    
    def __repr__(self):
        return F"{self.orientation.__repr__()} | {self.angular_vel.__repr__()} | {self.linear_accel.__repr__()}]"

    def serialize(self):
        """ Serialize the data contained in self.
            Return a bytes-like object with the serialized data.
        """
        return (serialize(self.orientation) +
                serialize(self.angular_vel) +
                serialize(self.linear_accel))

    def parse(self, buffer):
        """ Parse (deserialize) the bytes from a buffer.
            Update the instance object and return self.
        """
        self.orientation.parse(buffer[:Quaternion.size])
        self.angular_vel.parse(buffer[Quaternion.size:(Quaternion.size + Vector3.size)])
        self.linear_accel.parse(buffer[(Quaternion.size + Vector3.size):])
        return self
