from arducomm import ArdPiComm, ACKFrame, START_FLAG
from time import time

last_rx_time = 0
def test_callback(command, payload):
    global last_rx_time
    print(F"\t\tTime since last packet: {time() - last_rx_time}")
    last_rx_time = time()

    print(F"\t\tCommand received: {command}")
    print("\t\tPayload:")
    print(F"\t\t{payload}")


if __name__ == '__main__':
    # Test
    comm = ArdPiComm(test_callback)
    comm.start()

    """ Test TX max speed
    t1 = time()
    while time() - t1 < 10:
        t2 = time()
        comm.send(0x06, [1, 4, 70, 65])
        print("Packet TX time: {}".format(time() - t2))
    """

    while True:
        command = raw_input()
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
    comm.stop()
