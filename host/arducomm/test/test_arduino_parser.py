""" Test the parsing functions from the parser module.
    This test is paired with the arduino "parser_test.ino" example
    where the arduino tests the parsing of the messages.

    Host serializes -> Arduino parses.
"""

from arducomm import ArduComm, serialization
from arducomm.types import *
from time import sleep

BAUDRATE = 57600


def test_callback(command, payload):
    print(F"\tCommand received: {command}")
    print(F"\tPayload: {payload}")


def main(args):
    print(F"Serial port: {args.port}")
    print(F"Baudrate: {args.baudrate}")
    comm = ArduComm(test_callback, port=args.port, baudrate=args.baudrate)
    comm.start()

    sleep(3)

    # char test
    data = 'A'
    print(F"Sending cmd 2, char: '{data}'...")
    comm.send(2, serialization.serialize(data, dtype='char'))
    sleep(1)

    # string test
    data = 'test'
    print(F"Sending cmd 3, string: '{data}'...")
    comm.send(3, serialization.serialize(data, dtype='str'))
    sleep(1)

    # int8 test
    data = -100
    print(F"Sending cmd 4, int8: {data}...")
    comm.send(4, serialization.serialize(data, dtype='int8'))
    sleep(1)

    # int16 test
    data = -1000
    print(F"Sending cmd 5, int16: {data}...")
    comm.send(5, serialization.serialize(data, dtype='int16'))
    sleep(1)

    # uint16 test
    data = 1000
    print(F"Sending cmd 6, uint16: {data}...")
    comm.send(6, serialization.serialize(data, dtype='uint16'))
    sleep(1)

    # int32 test
    data = -100000
    print(F"Sending cmd 7, int32: {data}...")
    comm.send(7, serialization.serialize(data, dtype='int32'))
    sleep(1)

    # uint32 test
    data = 100000
    print(F"Sending cmd 8, uint32: {data}...")
    comm.send(8, serialization.serialize(data, dtype='uint32'))
    sleep(1)

    # float test
    data = 3.14159265
    print(F"Sending cmd 9, float: {data}...")
    comm.send(9, serialization.serialize(data, dtype='float'))
    sleep(1)
    # FloatArray test
    data = [(1.1 + 1.1*i) for i in range(5)]
    fa = FloatArray(data)
    print(F"Sending cmd 10, FloatArray: {fa}...")
    comm.send(10, serialization.serialize(fa))
    sleep(1)

    # Vector2 test
    v2 = Vector2(x=1.2, y=1.3)
    print(F"Sending cmd 11, Vector2: {v2}...")
    comm.send(11, serialization.serialize(v2))
    sleep(1)

    # Vector3 test
    v3 = Vector3(x=2.2, y=2.3, z=2.4)
    print(F"Sending cmd 12, Vector3: {v3}...")
    comm.send(12, serialization.serialize(v3))
    sleep(1)

    # Quaternion test
    q = Quaternion(z=0.707, w=0.707)
    print(F"Sending cmd 13, Quaternion: {q}...")
    comm.send(13, serialization.serialize(q))
    sleep(1)

    # Pose2D test
    p2d = Pose2D(x=4.4, y=2.3, theta=0.6)
    print(F"Sending cmd 14, Pose2D: {p2d}...")
    comm.send(14, serialization.serialize(p2d))
    sleep(1)

    # Pose test
    p = Pose(position=Vector3(5.3, 1.0, -7.4))
    print(F"Sending cmd 15, Pose: {p}...")
    comm.send(15, serialization.serialize(p))
    sleep(1)

    # Imu test
    imu = Imu()
    imu.angular_vel.z = 6.6
    imu.linear_accel.x = 2.0
    print(F"Sending cmd 16, Imu: {imu}...")
    comm.send(16, serialization.serialize(imu))
    sleep(1)

    print("Waiting 3 seconds until all responses are received...")
    sleep(3)

    comm.stop()

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description="Serial port reader")
    parser.add_argument("port", type=str)
    parser.add_argument("-b", "--baudrate", default=BAUDRATE, type=int,
                        help=F"Baudrate. Default: {BAUDRATE}")
    args = parser.parse_args()
    main(args)
