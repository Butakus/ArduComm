""" Test the serialization functions from the parser module.
    This test is paired with the arduino "serialization_test.ino" example
    where the arduino tests the serialization of the messages.

    Arduino serializes -> Host parses.
"""

from arducomm import ArduComm, serialization
from arducomm.types import *
from time import sleep
from math import isclose
import logging
logging.basicConfig(level=logging.INFO)

BAUDRATE = 57600


def test_callback(command, payload):
    print("------------------------------------------------")
    print(F"Command received: {command}")
    print(F"Payload: {payload}")
    print("###")

    # char test
    if command == 2:
        data = serialization.parse(payload, dtype='char')
        print(F"Received char: '{data}'...")
        print(F"Match 'A': {(data == 'A')}")
    # string test
    elif command == 3:
        data = serialization.parse(payload, dtype='str')
        print(F"Received string: '{data}'...")
        print(F"Match 'test': {(data == 'test')}")
    # int8 test
    elif command == 4:
        data = serialization.parse(payload, dtype='int8')
        print(F"Received int8: '{data}'...")
        print(F"Match -100: {(data == -100)}")
    # int16 test
    elif command == 5:
        data = serialization.parse(payload, dtype='int16')
        print(F"Received int16: '{data}'...")
        print(F"Match -1000: {(data == -1000)}")
    # uint16 test
    elif command == 6:
        data = serialization.parse(payload, dtype='uint16')
        print(F"Received uint16: '{data}'...")
        print(F"Match 1000: {(data == 1000)}")
    # int32 test
    elif command == 7:
        data = serialization.parse(payload, dtype='int32')
        print(F"Received int32: '{data}'...")
        print(F"Match -100000: {(data == -100000)}")
    # uint32 test
    elif command == 8:
        data = serialization.parse(payload, dtype='uint32')
        print(F"Received uint32: '{data}'...")
        print(F"Match 100000: {(data == 100000)}")
    # float test
    elif command == 9:
        data = serialization.parse(payload, dtype='float')
        print(F"Received float: '{data}'...")
        print(F"Match 3.14159265: {isclose(data, 3.14159265, rel_tol=1e-5)}")
    # FloatArray test
    elif command == 10:
        fa_data = serialization.parse(payload, dtype=FloatArray)
        print(F"Received FloatArray: '{fa_data}'")
        print(F"data[0] == 1.1: {isclose(fa_data.data[0], 1.1, rel_tol=1e-5)}")
        print(F"data[4] == 5.5: {isclose(fa_data.data[4], 5.5, rel_tol=1e-5)}")
    # Vector2 test
    elif command == 11:
        v2 = serialization.parse(payload, dtype=Vector2)
        print(F"Received Vector2: '{v2}'")
        print(F"v2.x == 1.2: {isclose(v2.x, 1.2, rel_tol=1e-5)}")
        print(F"v2.y == 1.3: {isclose(v2.y, 1.3, rel_tol=1e-5)}")
    # Vector3 test
    elif command == 12:
        v3 = Vector3()
        v3.parse(payload)
        # v3 = serialization.parse(payload, dtype=Vector3)
        print(F"Received Vector3: '{v3}'")
        print(F"v3.x == 2.2: {isclose(v3.x, 2.2, rel_tol=1e-5)}")
        print(F"v3.y == 2.3: {isclose(v3.y, 2.3, rel_tol=1e-5)}")
        print(F"v3.z == 2.4: {isclose(v3.z, 2.4, rel_tol=1e-5)}")
    # Quaternion test
    elif command == 13:
        q = serialization.parse(payload, dtype=Quaternion)
        print(F"Received Quaternion: '{q}'")
        print(F"q.z == 0.707: {isclose(q.z, 0.707, rel_tol=1e-5)}")
        print(F"q.w == 0.707: {isclose(q.w, 0.707, rel_tol=1e-5)}")
    # Pose2D test
    elif command == 14:
        p = serialization.parse(payload, dtype=Pose2D)
        print(F"Received Pose2D: '{p}'")
        print(F"p.x == 4.4: {isclose(p.x, 4.4, rel_tol=1e-5)}")
        print(F"p.theta == 0.6: {isclose(p.theta, 0.6, rel_tol=1e-5)}")
    # Pose test
    elif command == 15:
        p = serialization.parse(payload, dtype=Pose)
        print(F"Received Pose: '{p}'")
        print(F"p.position.x == 5.3: {isclose(p.position.x, 5.3, rel_tol=1e-5)}")
        print(F"p.orientation.w == 1.0: {isclose(p.orientation.w, 1.0, rel_tol=1e-5)}")
    # Imu test
    elif command == 16:
        imu = serialization.parse(payload, dtype=Imu)
        print(F"Received Imu: '{imu}'")
        print(F"imu.angular_vel.z == 6.6: {isclose(imu.angular_vel.z, 6.6, rel_tol=1e-5)}")
        print(F"imu.linear_accel.x == 2.0: {isclose(imu.linear_accel.x, 2.0, rel_tol=1e-5)}")



def main(args):
    print(F"Serial port: {args.port}")
    print(F"Baudrate: {args.baudrate}")
    comm = ArduComm(test_callback, port=args.port, baudrate=args.baudrate)
    comm.start()

    print("Press Enter to exit...")
    try:
        command = input()
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
