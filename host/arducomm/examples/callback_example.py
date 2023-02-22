""" Example on how to use callback functions in arduino to process the received mesages.
    This script is paired with callback_example.ino sketch.
"""
from arducomm import ArduComm
from arducomm.serialization import serialize
from arducomm.types import Pose
from time import time
import logging
logging.basicConfig(level=logging.INFO)

BAUDRATE = 57600

last_rx_time = 0
def msg_callback(command, payload):
    global last_rx_time
    print(F"\t\tTime since last packet: {time() - last_rx_time}")
    last_rx_time = time()

    print(F"\t\tCommand received: {command}")
    print("\t\tPayload:")
    print(F"\t\t{payload}")

def main(args):
    print(F"Serial port: {args.port}")
    print(F"Baudrate: {args.baudrate}")
    comm = ArduComm(msg_callback, port=args.port, baudrate=args.baudrate)
    comm.start()

    print("* Type 'f' to send command 0x03 with a float")
    print("* Type 'p' to send command 0x11 with a Pose")
    print("* Type 'q' to exit")
    try:
        while True:
            command = input()
            if command == 'f':
                data = 4.0
                print(F"Sending command 0x03 with float: {data}")
                comm.send(0x03, serialize(data, dtype='float'))
            elif command == 'p':
                data = Pose()
                data.position.x = 2.0
                print(F"Sending command 0x11 with Pose: {data}")
                comm.send(0x11, serialize(data))
            elif command == 'q':
                break
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
