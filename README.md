ArduComm
============
A data link layer protocol and library for advanced serial communication between a host computer and 8-bit Arduino boards.

## Context and purpose of this project
This project comes from the necessity of a robust and reliable communication system to share data between a host computer and an 8-bit Arduino board over a serial interface.
In particular, this project is intended to be used in a [ROS2](https://docs.ros.org) ecosystem where 8-bit Arduino microcontrollers are used, even though the current development has moved to [microROS](https://micro.ros.org) (which is the best solution but only has support for 32-bit boards). Nevertheless, this is a standalone library that can also be used in systems without ROS.

While there are multiple ways to share information over a serial interface, most implementations do "fire and forget" or can only be used in very specific use cases.
The purpose of ArduComm is to provide a robust data link protocol, which uses checksums and ACK frames to increase the reliability of the transmission, all while keeping the microcontroller side as simple and lightweight as possible to reduce memory usage and overhead.

ArduComm is composed of a C++ library for Arduino and a Python library for the host computer.

These libraries include code for data transmission and also for the serialization of objects, including a bunch of predefined types similar to the ones already defined in ROS (Vector3, Pose, Quaternion, etc.).

## What's inside?
The Arduino library and its documentation can be found in the [arduino](arduino) directory.

The Python library and its documentation can be found in the [host](host) directory.

In addition to the library code, each section contains simple tests and examples that show some of the features implemented.

## Communication protocol
More details about the implementation of the communication protocol can be found in [Protocol.md](Protocol.md).

## How to install / How to use
Please refer to the READMEs of each section ([arduino](arduino) / [host](host)) for these instructions.
