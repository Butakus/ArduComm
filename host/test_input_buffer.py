import serial
from time import time, sleep
from arducomm import ArduComm, PacketFrame

BAUDRATE = 9600


def test_callback(command, payload):
    print("New packet received:")
    print(F"\tCommand received: {command}")
    print("\tPayload:")
    print(F"\t{payload}")

def main(args):
    print(F"Serial port A: {args.port_a}")
    print(F"Serial port B: {args.port_b}")
    print(F"Baudrate: {args.baudrate}")

    # Start serial port A
    serial_A = serial.Serial(port=args.port_a, baudrate=args.baudrate)

    # Create a packet frame to send byte by byte
    frame = PacketFrame(1, 5, [55, 40, 33])
    frame_bytes = frame.serialize()
    print(F"Serialized frame: {frame_bytes}")

    # Send some bytes
    serial_A.write(frame_bytes[:3])
    sleep(0.1)

    # Start ArduComm in port B to receive data
    comm = ArduComm(test_callback, port=args.port_b, baudrate=args.baudrate)
    comm.start()
    sleep(0.5)

    # Send the rest of the bytes
    print(F"Writing byte: {frame_bytes[3:]}")
    serial_A.write(frame_bytes[3:])
    sleep(0.1)
    # Send the full packet again
    serial_A.write(frame_bytes)
    sleep(0.1)

    # Send only the start flag
    print(F"Writing byte: {frame_bytes[0:1]}")
    serial_A.write(frame_bytes[0:1])
    sleep(0.1)
    # Send the full packet again
    serial_A.write(frame_bytes)
    sleep(0.1)

    serial_A.close()
    comm.stop()


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description="Serial port reader")
    parser.add_argument("port_a", type=str)
    parser.add_argument("port_b", type=str)
    parser.add_argument("-b", "--baudrate", default=BAUDRATE, type=int,
                        help=F"Baudrate. Default: {BAUDRATE}")
    args = parser.parse_args()
    main(args)
