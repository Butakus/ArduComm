""" Read the serial port to test the library """
import serial
from time import time, sleep

BAUDRATE = 9600
PORT = "/dev/pts/2"

def main(args):
    print(F"Serial port: {args.port}")
    print(F"Baudrate: {args.baudrate}")
    print("Opening serial port...")
    try:
        ser = serial.Serial(port=PORT, baudrate=BAUDRATE)
        # Wait for the serial port to open
        while not ser.isOpen():
            sleep(0.1)
    except serial.serialutil.SerialException:
        print("\nSerial device not connected. Program aborted.\n")
        sys.exit(1)
    except ValueError as ve:
        print("\nSerial parameters not valid.\n")
        raise ve
    else:
        print("Connected!\n")

    try:
        while(True):
            if ser.inWaiting() > 0:
                data_bytes = ser.read(1)
                print(F"{data_bytes}\t-> {[i for i in data_bytes]}")
    except KeyboardInterrupt:
        print("\nStopping reader...")
    finally:
        ser.close()
    print("Done!")

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description="Serial port reader")
    parser.add_argument("port", type=str)
    parser.add_argument("-b", "--baudrate", default=BAUDRATE, type=int,
                        help=F"Baudrate. Default: {BAUDRATE}")
    args = parser.parse_args()
    main(args)
