# Host-side ArduComm python library and examples
This is the host-side library implementation of ArduComm, written in Python. In addition to the library's source code, different examples and tests are provided in [examples](examples) and [test](test).

## Installation
### PYPI installation
The easiest way to install the package is using `pip`:
```bash
python3 -m pip install arducomm
```

### Manual installation
Alternatively, this package can also be manually installed using Python's `build` module and `pip`.

First, install the `build` package if this hasn't been done before:
```bash
python3 -m pip install build
```
Then, build the package and install it with pip:
```bash
python3 -m build
python3 -m pip install dist/arducomm-*.whl --force-reinstall
```

## How to use
The class `ArduComm` is provided in the package and can be used to send and receive data. A callback function must be provided to process the received messages. Note that this implementation handles the incoming messages in a different thread, so there are no reading methods. The provided callback will be called when a new message is available.

```Python
from arducomm import ArduComm

def recv_callback(command, payload):
    print(F"Command received: {command}")
    print(F"Payload:\n{payload}")

comms = ArduComm(recv_callback, port='/dev/ttyUSB0', baudrate=57600)
comms.start() # Open the serial port
# Use comms object...
comms.stop() # Close the serial port
```

### How to send data
The function `send(command, payload)` can be used to send a message with the given command and payload. The payload must be a list of integers, a bytearray, or a bytes object.

```Python
comms.send(0x07) # Send command 0x07 with no payload
comms.send(0x03, [55, 50]) # Send command 0x03 with payload bytes [55, 50]
```

### Serialization
In order to work with payloads containing other types than pure bytes (aka uint8_t, aka unsigned char), these types must be serialized (converted to bytes) before being transmitted. Similarly, the array of bytes that is received in the payload must be parsed (deserialized) to reconstruct these types.

To this end, a serialization module is also included in this library, following the implementation of the Arduino library, with support for basic types (char, str, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, and float). Additional "complex" types are also defined in the types directory to provide more options with types such as 2D and 3D vectors, quaternions, or 2D/3D poses.

You can find the full list of supported types and how to serialize/parse them in [serialization_test](test/test_arduino_parser.py) and [parser_test](test/test_arduino_serialization.py).

Finally, you can also define your own types and use the provided serialization functions as it is done in the [custom_type](examples/custom_type.py) example.
