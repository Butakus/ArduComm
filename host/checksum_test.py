from arducomm import PacketFrame
import struct
from time import time

if __name__ == '__main__':
    packet = PacketFrame(1, 5, [i for i in range(5)])
    t0 = time()
    cs_msb, cs_lsb = packet.checksum()
    t1 = time()
    print(F"Fletcher time: {t1 - t0}")
    cs = struct.unpack('>H', bytearray([cs_msb, cs_lsb]))
    print(F"Fletcher's checksum: {cs_msb}, {cs_lsb} -> {(cs_msb << 8) + cs_lsb}")
    print(cs)
    t0 = time()
    crc16 = packet.crc16()
    t1 = time()
    print(F"CRC16 time: {t1 - t0}")
    crc16_bytes = struct.pack('>H', crc16)
    print(F"CRC16-CCITT: {crc16}")
    print(F"CRC16-CCITT bytes: {[i for i in crc16_bytes]}")
    # ser_packet = packet.serialize()
    # print(ser_packet)
