# LED Control Arduino Project

An Arduino project that automatically controls LED devices via infrared signals based on scheduled times using a DS3231 Real-Time Clock (RTC) module.

## Features

- **Scheduled LED Control**: Automatically sends IR signals to control LEDs at 7:00 AM and 8:00 PM daily
- **Real-Time Clock Integration**: Uses DS3231 RTC module for accurate timekeeping
- **Infrared Communication**: Sends NEC protocol IR signals to control LED devices
- **Serial Monitoring**: Displays current time and sends status messages via serial port

## Hardware Requirements

- Arduino board (Uno, Nano, etc.)
- DS3231 RTC module
- IR transmitter LED
- Connecting wires

## Pin Configuration

- **IR Transmitter Pin**: Digital pin 3
- **DS3231 Module**: Connected via I2C (SDA/SCL pins)

## Dependencies

This project requires the following Arduino libraries:

- `TinyIRSender` - For infrared signal transmission
- `microDS3231` - For DS3231 RTC module communication

## Installation

1. Install required libraries through Arduino IDE Library Manager
2. Connect hardware components according to pin configuration
3. Upload the sketch to your Arduino board

## Usage

The system automatically:
- Sets the RTC time to compilation time on first run
- Continuously displays current time via Serial Monitor (9600 baud)
- Sends LED control signals at scheduled times (7:00 AM and 8:00 PM)
- Detects and reports RTC power loss

## IR Signal Details

- **Protocol**: NEC
- **Address**: 0x00
- **Command**: 0x40
- **Repeats**: 0

## Serial Output

The program outputs time information and status messages to the serial port at 9600 baud rate for monitoring and debugging purposes.