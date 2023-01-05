from arducomm import ArduComm, ACKFrame, START_FLAG
from time import time

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

    try:
        while True:
            command = input()
            if command == 's':
                print('Sending stop command')
                comm.send(0x00)
            elif command == 'f':
                print('Sending forward command')
                comm.send(0x03, [55, 50])
            elif command == 'e':
                print('Sending flag command')
                comm.send(START_FLAG, [125])
            elif command == 'a':
                print('Sending ACK')
                comm.send_frame(ACKFrame(1))
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
