from arducomm import ArdPiComm, ACKFrame
from time import time

BAUDRATE = 9600


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
    comm = ArdPiComm(test_callback, port=args.port, baudrate=args.baudrate)
    comm.start()

    t1 = time()
    while time() - t1 < 5:
        t2 = time()
        comm.send(0x06, [1, 4, 70, 65])
        print("Packet TX time: {}".format(time() - t2))
    comm.stop()


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description="Serial port reader")
    parser.add_argument("port", type=str)
    parser.add_argument("-b", "--baudrate", default=BAUDRATE, type=int,
                        help=F"Baudrate. Default: {BAUDRATE}")
    args = parser.parse_args()
    main(args)
