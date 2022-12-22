""" Module to implement the communication protocol between the Raspberry Pi
    and the Arduino in Roberto over a serial port.

    The communication protocol is based on HLDLC with ARQ-StopAndWait error control.
"""

import serial
import sys
from time import sleep, time
from threading import Thread, Lock

# Default baudrate
BAUDRATE = 57600
# Maximum number of retries if the packet ACK returns with an error 
MAX_RETRIES = 3
# Max timeout (seconds) to wait for an ACK
TIMEOUT = 3.0
# Time (seconds) to wait before checking if the ACK has been received
ACK_POLL_TIME = 0.001
# Time (seconds) to wait between each ser.inWaiting() check
PACKET_POLL_TIME = 0.001 # seconds
# Time (seconds) required by the Arduino to read its input buffer (64 bytes)
ARDUINO_READ_TIME = 0.05

ACK_COMMAND = 0x01

START_FLAG = 0x7E
ESCAPE_FLAG = 0x7D

CRC16_LUT = [ 
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
]


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



    def crc16(self):
        """ CRC-16 (CCITT) implemented with a precomputed lookup table.
            Currently not used as 16-bit Fletcher's is faster.
        """
        crc = 0xFFFF
        crc = (crc << 8) ^ CRC16_LUT[(crc >> 8) ^ self.seq_number]
        crc &= 0xFFFF # important, crc must stay 16bits all the way through
        crc = (crc << 8) ^ CRC16_LUT[(crc >> 8) ^ self.command]
        crc &= 0xFFFF
        crc = (crc << 8) ^ CRC16_LUT[(crc >> 8) ^ self.payload_length]
        crc &= 0xFFFF
        for payload_byte in self.payload:
            crc = (crc << 8) ^ CRC16_LUT[(crc >> 8) ^ payload_byte]
            crc &= 0xFFFF
        return crc

    def checksum(self):
        """ Compute the 16bit Fletcher's checksum of the data """
        lsb = 0
        msb = 0
        data = [self.seq_number, self.command, self.payload_length] + self.payload
        for i in range(len(data)):
            lsb += data[i]
            msb += lsb
            if i % 16 == 0:
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
        self.serial_lock = Lock()
        self.callback = message_callback

        print("Connecting to serial port...")
        try:
            self.ser = serial.Serial(port=port, baudrate=baudrate)
            # Wait for the arduino to init
            open_time = time()
            while not self.ser.isOpen():
                sleep(0.1)
                if time() - open_time > TIMEOUT:
                    print("\nConnection timeout.")
                    raise TimeoutError("Connection timeout")
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
                        in_buffer = bytearray()
            else:
                sleep(PACKET_POLL_TIME)

        """ Code to test raw data received (without frame formatting)
        in_buffer = bytearray()
        while (self.running):
            if self.ser.inWaiting() > 0:
                b = self.ser.read(1)
                in_buffer += b
                print("Buffer: {}".format(in_buffer))
            sleep(PACKET_POLL_TIME)
        """


    def process_frame(self, data):
        """ Get the byte array, create its corresponding frame object and process it """
        # Restore the escaped bytes
        escaped_data = []
        data_iter = data.__iter__()
        for i in data_iter:
            b = i if i != ESCAPE_FLAG else invert_bit_5(next(data_iter))
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

        # Use a mutex to avoid conflict between packet sends and ACK replies (different threads)
        with self.serial_lock:
            # Split data in 64 bytes blocks (Arduino buffer size)
            while len(data) > 64:
                self.ser.write(data[:64])
                data = data[64:]
                # Wait for the arduino to read the buffer, so it does not overflow
                sleep(ARDUINO_READ_TIME)
            # Write the rest of the data
            self.ser.write(data)

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
            sleep(ACK_POLL_TIME)
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
