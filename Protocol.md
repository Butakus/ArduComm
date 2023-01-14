# Communication Protocol

The communication protocol is based on the [High-Level Data Link Control (HLDLC)](https://en.wikipedia.org/wiki/High-Level_Data_Link_Control) protocol with asynchronous framing.

In addition, a simple [Stop-and-Wait ARQ](https://en.wikipedia.org/wiki/Stop-and-wait_ARQ) mechanism is used to increase the robustness of the communication. This process uses ACKs to confirm the correct reception of every packet, and includes an error control mechanism to retry faulty packet.

## Frame structure

The transmitted frames have the following data structure:

| Flag | Seq Num | Cmd | Payload | FCS | Flag|
|:----:|:-------:|:---:|:-------:|:---:|:---:|
| 8 bits | 8 bits  | 8 bits | Variable length, 8xN bits | 16 bits | 8 bits |

* **Flag**: The flag byte 0x7E is always sent to mark the beginning and the end of a frame.
* **Seq Num**: This byte represents the sequence number, which is used in the error control mechanism by ACK frames to acknowledge the reception or requesting the retry of a frame.
* **Cmd**: A byte that represents the command of the frame. This is defined by the application but one command is expected to always have the same type of payload data. A frame must always include a command, even if the payload is empty.
* **Payload**: N bytes with the transmitted data (application specific, can also be empty).

    *Note*: Currently the library for Arduino is adjusted for the Atmega328 buffer size (64 bytes), so please keep the payload size <= 58 bytes when sending data to the Arduino.
* **FCS**: Frame check sequence, based on the 16-bit [Fletcher's checksum](https://en.wikipedia.org/wiki/Fletcher%27s_checksum). This is used to verify the proper reception of the data.
* **Escape Flag**: The flag byte 0x7D is used as the escape flag for byte stuffing. Whenever a flag byte (0x7E or 0x7D) is going to be transmitted, the escape flag is added before it and 5th bit of the byte-to-send is inverted.

## Error control
TODO: Error control documentation and ARQ examples (with images).
