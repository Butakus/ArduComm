# Communication Protocol

The communication protocol is based on the [High-Level Data Link Control (HLDLC)](https://en.wikipedia.org/wiki/High-Level_Data_Link_Control) protocol with asynchronous framing.

In addition, a simple [Stop-and-Wait ARQ](https://en.wikipedia.org/wiki/Stop-and-wait_ARQ) mechanism is used to increase the robustness of the communication. This process uses ACKs to confirm the correct reception of every packet, and includes an error control mechanism to retry faulty packet.

TODO: Add packet structure, byte start and escape flags, Checksum info and ARQ examples (with images).
