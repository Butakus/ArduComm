""" Simple example to use ArduComm library to turn on and off a led.
    - Commands 0x03 and 0x7E turn the led ON.
    - Command 0x00 turns the led OFF.
"""
from arducomm import ArduComm
from arducomm.arducomm import START_FLAG, ESCAPE_FLAG
from time import time
import logging
logging.basicConfig(level=logging.INFO)

BAUDRATE = 57600


last_rx_time = 0
def test_callback(command, payload):
    global last_rx_time
    print(F"\t\tTime since last packet: {time() - last_rx_time}")
    last_rx_time = time()

    print(F"\t\tCommand received: {command}")
    print("\t\tPayload:")
    print(F"\t\t{payload}")

def main(args):
    print(F"Serial port: {args.port}")
    print(F"Baudrate: {args.baudrate}")
    comm = ArduComm(test_callback, port=args.port, baudrate=args.baudrate)
    comm.start()

    print("* Type 's' to turn the led OFF")
    print("* Type 'f' or 'e' to turn the led ON")
    print("* Type 'q' to exit")
    try:
        while True:
            command = input()
            if command == 's':
                print('Sending OFF command')
                comm.send(0x00)
            elif command == 'f':
                print('Sending ON command')
                comm.send(0x03, [55, 50])
            elif command == 'e':
                print('Sending flag (ON) command')
                comm.send(START_FLAG, [ESCAPE_FLAG])
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
