# Bare-Metal UART Telemetry Runner

This repository contains a C implementation for initializing and interacting with a UART interface via the Linux `termios` API. It is designed as a foundational proof-of-concept for extracting bare-metal compliance telemetry (like the RISC-V `tohost` interface) over a serial connection.

## Architecture
The program handles:
* Non-blocking port initialization (`O_NDELAY`).
* Baud rate configuration (115200 8N1).
* Disabling canonical processing, echo, and software/hardware flow control to ensure raw, uncorrupted byte streams.
* Asynchronous data reception utilizing `select()` for timeout management.

## Prerequisites
* A Linux environment (or WSL).
* `gcc` and `make`.
* `socat` (for local simulation).

## Build Instructions
Clone the repo and run `make`:
```bash
make