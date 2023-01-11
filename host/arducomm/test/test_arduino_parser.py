""" Test the parsing functions from the parser module.
    This test is paired with the arduino "parser_test.ino" example
    where the arduino tests the parsing of the messages.

    Host serializes -> Arduino parses.
"""

from arducomm import ArduComm, serialization
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

    data = 'A'
    print(F"Sending cmd 10, char: '{data}'...")
    comm.send(10, serialization.serialize(data, dtype='char'))
    sleep(1)

    data = 'test'
    print(F"Sending cmd 2, string: '{data}'...")
    comm.send(2, serialization.serialize(data, dtype='str'))
    sleep(1)

    data = -100
    print(F"Sending cmd 3, int8: {data}...")
    comm.send(3, serialization.serialize(data, dtype='int8'))
    sleep(1)

    data = -1000
    print(F"Sending cmd 4, int16: {data}...")
    comm.send(4, serialization.serialize(data, dtype='int16'))
    sleep(1)

    data = 1000
    print(F"Sending cmd 5, uint16: {data}...")
    comm.send(5, serialization.serialize(data, dtype='uint16'))
    sleep(1)

    data = -100000
    print(F"Sending cmd 6, int32: {data}...")
    comm.send(6, serialization.serialize(data, dtype='int32'))
    sleep(1)

    data = 100000
    print(F"Sending cmd 7, uint32: {data}...")
    comm.send(7, serialization.serialize(data, dtype='uint32'))
    sleep(1)

    data = 3.14159265
    print(F"Sending cmd 8, float: {data}...")
    comm.send(8, serialization.serialize(data, dtype='float'))
    sleep(1)

    # data = [5.0, 1.0, 0.0]
    # print(F"Sending cmd 9, Vector3: {data}...")
    # comm.send(9, struct.pack('<3f', data))
    # sleep(1)

    print("Waiting 5 seconds until all responses are received...")
    sleep(5)

    comm.stop()

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description="Serial port reader")
    parser.add_argument("port", type=str)
    parser.add_argument("-b", "--baudrate", default=BAUDRATE, type=int,
                        help=F"Baudrate. Default: {BAUDRATE}")
    args = parser.parse_args()
    main(args)
