""" Test the parsing functions from the parser module.
    This test is paired with the arduino "parser_test.ino" example
    where the arduino tests the parsing of the messages.

    Host serializes -> Arduino parses.
"""

import arduparser
from arducomm import ArduComm
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
    comm.send(10, arduparser.serialize_char(data))
    sleep(1)

    data = 'test'
    print(F"Sending cmd 2, string: '{data}'...")
    comm.send(2, arduparser.serialize_str(data))
    sleep(1)

    data = -100
    print(F"Sending cmd 3, int8: {data}...")
    comm.send(3, arduparser.serialize_int8(data))
    sleep(1)

    data = -1000
    print(F"Sending cmd 4, int16: {data}...")
    comm.send(4, arduparser.serialize_int16(data))
    sleep(1)

    data = 1000
    print(F"Sending cmd 5, uint16: {data}...")
    comm.send(5, arduparser.serialize_uint16(data))
    sleep(1)

    data = -100000
    print(F"Sending cmd 6, int32: {data}...")
    comm.send(6, arduparser.serialize_int32(data))
    sleep(1)

    data = 100000
    print(F"Sending cmd 7, uint32: {data}...")
    comm.send(7, arduparser.serialize_uint32(data))
    sleep(1)

    data = 3.14159265
    print(F"Sending cmd 8, float: {data}...")
    comm.send(8, arduparser.serialize_float(data))
    sleep(1)

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
