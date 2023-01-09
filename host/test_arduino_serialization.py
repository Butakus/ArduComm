""" Test the serialization functions from the parser module.
    This test is paired with the arduino "serialization_test.ino" example
    where the arduino tests the serialization of the messages.

    Arduino serializes -> Host parses.
"""
import arduparser
from arducomm import ArduComm
from time import sleep
import struct

BAUDRATE = 57600


def test_callback(command, payload):
    print("------------------------------------------------")
    print(F"Command received: {command}")
    print(F"Payload: {payload}")
    print("###")

    if command == 10:
        data = arduparser.parse_char(payload)
        print(F"Received char: '{data}'...")
        print(F"Match 'A': {(data == 'A')}")

    elif command == 2:
        data = arduparser.parse_str(payload)
        print(F"Received string: '{data}'...")
        print(F"Match 'test': {(data == 'test')}")

    elif command == 3:
        data = arduparser.parse_int8(payload)
        print(F"Received int8: '{data}'...")
        print(F"Match -100: {(data == -100)}")

    elif command == 4:
        data = arduparser.parse_int16(payload)
        print(F"Received int16: '{data}'...")
        print(F"Match -1000: {(data == -1000)}")

    elif command == 5:
        data = arduparser.parse_uint16(payload)
        print(F"Received uint16: '{data}'...")
        print(F"Match 1000: {(data == 1000)}")

    elif command == 6:
        data = arduparser.parse_int32(payload)
        print(F"Received int32: '{data}'...")
        print(F"Match -100000: {(data == -100000)}")

    elif command == 7:
        data = arduparser.parse_uint32(payload)
        print(F"Received uint32: '{data}'...")
        print(F"Match 100000: {(data == 100000)}")

    elif command == 8:
        data = arduparser.parse_float(payload)
        print(F"Received float: '{data}'...")
        print(F"Match 3.14159265: {(data == 3.14159265)}")
        print(F"Float error: {(abs(data - 3.14159265))}")

    elif command == 9:
        # data = arduparser.parse_float(payload)
        print(F"Received Vector3: '{struct.unpack('<3f', bytes(payload))}'...")


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
