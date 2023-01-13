""" Define a custom data type and transmit it with ArduComm.

The custom "complex" type is Odometry, defined in the odometry.ino file.

This program will send an initial Odometry object with the command 0x02.
Then it will receive the updates from the arduino with the command 0x05.

"""
from arducomm import ArduComm
from arducomm.serialization import Serializable, parse, serialize
from arducomm.types import Pose, Vector3
from time import time, sleep

BAUDRATE = 57600

class Odometry(Serializable):
    """ Odometry type definition and serialization """
    # Size in bytes
    size = Pose.size + 2*Vector3.size

    def __init__(self, pose=Pose(), linear_vel=Vector3(), angular_vel=Vector3()):
        self.pose = pose
        self.linear_vel = linear_vel
        self.angular_vel = angular_vel
    
    def __repr__(self):
        return F"{self.pose.__repr__()} - {self.linear_vel.__repr__()} - {self.angular_vel.__repr__()}]"

    def serialize(self):
        """ Serialize the data contained in self.
            Return a bytes-like object with the serialized data.
        """
        return serialize(self.pose) + serialize(self.linear_vel) + serialize(self.angular_vel)

    def parse(self, buffer):
        """ Parse (deserialize) the bytes from a buffer.
            Update the instance object and return self.
        """
        self.pose.parse(buffer[:Pose.size])
        self.linear_vel.parse(buffer[Pose.size:(Pose.size + Vector3.size)])
        self.angular_vel.parse(buffer[(Pose.size + Vector3.size):])
        return self


last_rx_time = 0
def comms_callback(command, payload):
    # Debug info
    global last_rx_time
    print(F"\t\tTime since last packet: {time() - last_rx_time}")
    last_rx_time = time()
    print(F"\t\tCommand received: {command}")
    print("\t\tPayload:")
    print(F"\t\t{payload}")

    # Process message
    if command == 0x05:
        odom = Odometry().parse(payload)
        # Alternative:
        # odom = parse(payload, dtype=Odometry)
        print("##########")
        print(F"Received an odometry update:\n{odom}")

def main(args):
    print(F"Serial port: {args.port}")
    print(F"Baudrate: {args.baudrate}")
    comm = ArduComm(comms_callback, port=args.port, baudrate=args.baudrate)
    comm.start()
    sleep(2)

    # Initial odometry object that will be updated by the arduino
    initial_odom = Odometry()
    initial_odom.pose.position.x = 2.0
    initial_odom.linear_vel.x = 0.1
    print(F"Initial odometry:\n{initial_odom}")
    comm.send(0x02, serialize(initial_odom))
    print("Initial object sent. Ready to receive updates.")
    print("Press ctrl + c to exit")
    try:
        while True:
            sleep(1)
    except KeyboardInterrupt:
        print("\nStopping...")
    finally:
        comm.stop()

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description="Serial port reader")
    parser.add_argument("port", type=str)
    parser.add_argument("-b", "--baudrate", default=BAUDRATE, type=int,
                        help=F"Baudrate. Default: {BAUDRATE}")
    args = parser.parse_args()
    main(args)
