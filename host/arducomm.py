""" Module to implement the communication protocol between the Raspberry Pi
    and the Arduino in Roberto over a serial port.

    The communication protocol is based on HLDLC with ARQ-StopAndWait error control.
"""

import serial
import sys
from time import sleep, time
from threading import Thread

BAUDRATE = 57600
MAX_RETRIES = 3
TIMEOUT = 3.0 # seconds

ACK_COMMAND = 0x01

START_FLAG = 0x7E
ESCAPE_FLAG = 0x7D


def check_flag_conflict(val):
    """ Check if the byte conflicts with one of the flags """
    return val == START_FLAG or val == ESCAPE_FLAG


def invert_bit_5(val):
    """ Invert the 5th bit """
    return val ^ (1 << 5)


class PacketFrame(object):
    """ Class to implement the packet frame functionality """
    def __init__(self, seq_number, command, payload):
        self.seq_number = seq_number & 0xFF
        self.command = command & 0xFF
        if len(payload) > 255:
            raise ValueError(F"Payload cannot have more than 255 bytes. Current payload length is {len(payload)}")
        self.payload_length = len(payload) & 0xFF
        self.payload = payload


    def serialize(self):
        """ Convert the object to a byte string to send it over the serial port and add the checksum """
        data = []
        # Start
        data.append(START_FLAG)

        # Sequence number
        if check_flag_conflict(self.seq_number):
            data.append(ESCAPE_FLAG)
            data.append(invert_bit_5(self.seq_number))
        else:
            data.append(self.seq_number)

        # Command
        if check_flag_conflict(self.command):
            data.append(ESCAPE_FLAG)
            data.append(invert_bit_5(self.command))
        else:
            data.append(self.command)

        # Payload length
        if check_flag_conflict(self.payload_length):
            data.append(ESCAPE_FLAG)
            data.append(invert_bit_5(self.payload_length))
        else:
            data.append(self.payload_length)

        # Payload
        for payload_b in self.payload:
            if check_flag_conflict(payload_b):
                data.append(ESCAPE_FLAG)
                data.append(invert_bit_5(payload_b))
            else:
                data.append(payload_b)

        # Checksum
        checksum_msb, checksum_lsb = self.checksum()

        if check_flag_conflict(checksum_msb):
            data.append(ESCAPE_FLAG)
            data.append(invert_bit_5(checksum_msb))
        else:
            data.append(checksum_msb)
        if check_flag_conflict(checksum_lsb):
            data.append(ESCAPE_FLAG)
            data.append(invert_bit_5(checksum_lsb))
        else:
            data.append(checksum_lsb)

        # End
        data.append(START_FLAG)

        return data


    def checksum(self):
        """ Compute the 16bit Fletcher's checksum of the data """
        lsb = 0
        msb = 0
        lsb += self.seq_number
        msb += lsb
        lsb += self.command
        msb += lsb
        lsb += self.payload_length
        msb += lsb
        for i in xrange(self.payload_length):
            lsb += self.payload[i]
            msb += lsb
            if i % 32 == 0:
                # Do a reduction each 16 bytes
                lsb = (lsb & 0xFF) + (lsb >> 8)
                msb = (msb & 0xFF) + (msb >> 8)
        # Last double reduction to add the carry
        lsb = (lsb & 0xFF) + (lsb >> 8)
        lsb = (lsb & 0xFF) + (lsb >> 8)
        msb = (msb & 0xFF) + (msb >> 8)
        msb = (msb & 0xFF) + (msb >> 8)
        return msb, lsb


class ACKFrame(PacketFrame):
    """ Class to implement the ACK frame functionality """
    def __init__(self, seq_number):
        self.seq_number = seq_number
        self.command = ACK_COMMAND

    def serialize(self):
        """ Convert the object to a byte string to send it over the serial port and add the checksum """
        data = []
        # Start
        data.append(START_FLAG)

        # Sequence number
        if check_flag_conflict(self.seq_number):
            data.append(ESCAPE_FLAG)
            data.append(invert_bit_5(self.seq_number))
        else:
            data.append(self.seq_number)

        # Command
        data.append(self.command)

        # End
        data.append(START_FLAG)
 
        return bytearray(data)


class ArdPiComm(Thread):
    """ Class to handle the serial object and implement the communication protocol """
    
    def __init__(self, message_callback, port='/dev/ttyACM0', baudrate=BAUDRATE):
        self.sent_seq = 0
        self.last_ack = None
        self.retries = 0
        self.serial_lock = False
        self.callback = message_callback

        print("Connecting to serial port...")
        try:
            self.ser = serial.Serial(port=port, baudrate=baudrate)
            # Wait for the arduino to init
            sleep(2)
        except serial.serialutil.SerialException:
            print("\nSerial device not connected. Program aborted.\n")
            sys.exit(1)
        except ValueError as ve:
            print("\nSerial parameters not valid.\n")
            raise ve
        else:
            print("Connected!\n")

        Thread.__init__(self)
        self.daemon = True
        self.running = True


    def run(self):
        """ Receive all incoming bytes and pack them in frames
            If the frame is an ack message, put it in the ACK buffer
            If it is another message, process it with the message callback
        """
        in_buffer = bytearray()
        while (self.running):
            if self.ser.inWaiting() > 0:
                try:
                    b = self.ser.read(1)
                except SerialException as se:
                    # TODO
                    print("Error: Could not read the serial port!")
                    continue
                in_buffer += b
                if b[0] == START_FLAG:
                    # TODO: Verify that this is correct. Looks weird.
                    # Get number of bytes between start flags to skip ghost frames (between end_flag and start_flag)
                    if (len(in_buffer) - 1) > 2:
                        # Process frame
                        frame_data = in_buffer[:]
                        self.process_frame(frame_data)
                        # Clear the current buffer
                        in_buffer = []
            else:
                sleep(0.01) # 10ms sleep

        """ Code to test raw data received (without frame formatting)
        in_buffer = bytearray()
        while (self.running):
            if self.ser.inWaiting() > 0:
                b = self.ser.read(1)
                in_buffer += b
                print("Buffer: {}".format(in_buffer))
            sleep(0.01)
        """


    def process_frame(self, data):
        """ Get the byte array, create its corresponding frame object and process it """
        # Restore the escaped bytes
        escaped_data = []
        data_iter = data.__iter__()
        for i in data_iter:
            b = i if i != ESCAPE_FLAG else invert_bit_5(data_iter.next())
            escaped_data.append(b)

        seq_number = escaped_data[1]
        command = escaped_data[2]

        if command == ACK_COMMAND:
            # Create ACKFrame and store it
            self.last_ack = ACKFrame(seq_number)
        else:
            retry = False
            payload_length = escaped_data[3]
            payload = escaped_data[4:-3]
            # Verify length
            if len(payload) != payload_length:
                retry = True
                print("Packet payload size mismatch")
            else:
                packet = PacketFrame(seq_number, command, payload)

                # Check checksum
                received_checksum = (escaped_data[-3], escaped_data[-2])
                computed_checksum = packet.checksum()
                if received_checksum[0] != computed_checksum[0] or received_checksum[1] != computed_checksum[1]:
                    retry = True
                    print("Packet checksum mismatch")
            
            # Send ACK
            if retry:
                # Reset the ack packet number to indicate retransmission
                self.send_frame(ACKFrame(seq_number))
            else:
                self.send_frame(ACKFrame((seq_number + 1) % 256))

                # Process the packet in a different thread to avoid blocking the main (receiving) thread
                t = Thread(target=self.callback, args=((command, payload)))
                t.daemon = True
                t.start()


    def stop(self):
        # Stop the thread and close the serial port
        self.running = False

        if self.ser.isOpen():
            print("\nClosing serial port...")
            self.ser.close()
            print("Serial port closed.")
        else:
            print("Serial port is already closed.")


    def send_frame(self, frame):
        """ Send a frame object to the serial port """
        # Reset the last ACK
        if frame.command != ACK_COMMAND:
            self.last_ack = None

        # Serialize the Packet data
        data = frame.serialize()

        # Wait until the lock is released
        while self.serial_lock:
            sleep(0.005)
        try:
            self.serial_lock = True
            # Split data in 64 bytes blocks
            while len(data) > 64:
                self.ser.write(data[:64])
                data = data[64:]
                # Wait for the arduino to read the buffer, so it does not overflow
                sleep(0.05)
            # Write the rest of the data
            self.ser.write(data)
            # Release the serial lock
        finally:
            self.serial_lock = False

        # Do not wait for ACK after sending an ACK frame
        if frame.command == ACK_COMMAND:
            return True

        # Wait for the ACK
        sent_time = time()
        while not self.last_ack:
            # Check for the timeout, so it does not stay here forever
            if (time() - sent_time) >= TIMEOUT:
                print("Timeout exceeded.")
                print("Did not received the ACK for packet {n} with command {c}".format(n=frame.seq_number, c=frame.command))
                return False
            sleep(0.01)
        # Check the ACK and retry if needed
        if self.last_ack.seq_number == frame.seq_number:
            # Retry
            self.retries += 1
            if self.retries >= MAX_RETRIES:
                print('Could not send packet {n} with command {c}'.format(c=frame.command, n=frame.seq_number))
                return False
            else:
                print("Received ACK with retry code. Retrying packet {n}...".format(n=frame.seq_number))
                return self.send_frame(frame)
        else:
            # ACK OK
            self.retries = 0
            return True


    def send(self, command, payload=[]):
        """ Send a packet given the command and the payload """
        if command > 255:
            print("Error: Command number > 255. Frame cannot be sent")
            return False
        if len(payload) > 255:
            print("Error: Payload length exceded. Frame cannot be sent")
            return False

        self.sent_seq = (self.sent_seq + 1) % 256

        # Create the Packet frame
        frame = PacketFrame(self.sent_seq, command, payload)
        print(F"Sending packet {self.sent_seq} with command {command}")

        return self.send_frame(frame)
