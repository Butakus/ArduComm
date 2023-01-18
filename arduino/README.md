ArduComm - Arduino library and examples
=======================================

This is the Arduino library for ArduComm. In addition to the library's source code, different examples and tests are provided in [examples](ArduComm/examples).

## Installation
To install the Arduino library, just copy the folder [ArduComm](ArduComm) to your Arduino libraries' directory.

If you are using a Linux computer you can also run the script [install_arduino_libraries](install_arduino_libraries), which will copy the library for you.

```sh
./install_arduino_libraries
```

## How to use
The main library header `ArduComm.h` includes the `ArduComm` class that is used for the transmissions. Instances of this class can be initialized in two ways: 1. Use the function `begin`, which will use the default `Serial` port at the specified baudrate. 2. Initialize the Stream that will be used for communications and use the function `set_stream(Stream* port)`:

```C++
#include <ArduComm.h>
ArduComm comms;
void setup() {
    // Initialize serial port and comms object
    comms.begin(57600);
    // Alternatively, open a Serial port (or any Stream) and pass it by reference
    Serial2.begin(57600);
    comms.set_stream(&Serial2);
    // Continue setup...
}
```

Afterwards, the ArduComm object can be used to send and receive frames.

### How to send data
The function `send(uint8_t command, uint8_t payload_size, uint8_t payload[])` is used to send a message frame.

Below is an extract of [send_example](ArduComm/examples/send_example) that shows this functionality:
```C++
// Send the single command 0x03 without any payload
comms.send(0x03);

// Send the command 0x06, with the payload bytes defined in data_arr
const uint8_t data_size = 4;
uint8_t data_arr[data_size] = {5, 48, 100, 150};
comms.send(0x06, data_size, data_arr);
```

This function also returns an error code defined in `enum tx_error`:
```C++
typedef enum tx_error
{
    NO_ERROR = 0,  // Frame was sent successfully
    TIMEOUT_ERROR, // No ACK received after the timeout
    ACK_ERROR      // Frame was received with errors multiple times (max. number of tx retries exceeded)
} TXError;
```

### How to receive data
In order to receive a frame, the `read()` function must be called to read the serial port and update the internal buffer. This function will return 1 if there is a full message frame available. Afterwards, this message can be obtained using the functions `get_command()` and `get_payload(uint8_t payload[])`:

Below is an extract of the [led_switch](ArduComm/examples/led_switch) example, which shows this functionality:

```C++
void loop() {
    if (comms.read())
    {
        // If there is a frame available, get the command and the payload
        uint8_t command = comms.get_command();
        uint8_t payload_size = comms.get_payload(payload);
        if (command == 0x03 || command == 0x7E)
        {
            digitalWrite(13, HIGH);
        }
        else if (command == 0x00)
        {
            digitalWrite(13, LOW);
        }
        // Do something with payload
    }
}
```

Additionally, one can define callback functions to process a message from a given command.
With this approach, all messages from this command must have the same type. The function `add_callback<T>(command, callback_function)` is used to associate a given command with a callback_function, automatically deserializing the payload into the given type `T`.

Remember that the `read()` function must still be called in order to read the serial port and update the frame buffer. Otherwise incoming bytes will not be processed and callbacks will not be called.

Below is an extract from [callback_example](ArduComm/examples/callback_example) that shows this functionality:

```C++
void cmd_callback(const float& data)
{
    // Process float data
}

ArduComm comms;
void setup() {
    comms.begin(57600);
    comms.add_callback<float>(0x03, &cmd_callback);
}

void loop () {
    comms.read();
}
```

*Note: Please keep callbacks as light as possible and avoid using multiple communication functions from inside of them.

### Serialization
In order to work with payloads containing other types than pure bytes (aka uint8_t, aka unsigned char), these types must be serialized (converted to bytes) before being transmitted. Similarly, the array of bytes that is received in the payload must be parsed (deserialized) to reconstruct these types.

To this end, a serialization module is included in this library ([serialization.h](ArduComm/src/arducomm/serialization.h)), with support for basic types (char, String, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, and float). Additional "complex" types are also defined in the types directory to provide more options with types such as 2D and 3D vectors, quaternions, or 2D/3D poses.

You can find the full list of supported types and how to serialize/parse them in [serialization_test](ArduComm/examples/test/serialization_test/serialization_test.ino) and [parser_test](ArduComm/examples/test/parser_test/parser_test.ino).

Finally, you can also define your own types and use the provided serialization functions as it is done in the [custom_type](ArduComm/examples/custom_type) example.
