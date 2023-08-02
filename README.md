# Refactor of [NanoELS H4](https://github.com/kachurovskiy/nanoels/tree/main/h4) by Maxim Kachurovskiy to PlatformIO

The original code is here: [NanoELS H4](https://github.com/kachurovskiy/nanoels/tree/main/h4)

## Why
The ArduinoIDE is great for simple projects, but this wonderful controller is a bit more substantial.

* The ArduinoIDE re-compiles everything for the smallest change. Turnaround is much quicker with PlatformIO.
* PlatformIO allows you to use the amazing Arduino library ecosystem with a much better IDE that supports code completion, symbol lookup etc. One can even easily jump into the 3rd party library function definitions.
* Splitting the large file into multiple smaller ones should make it more maintainable.
* PlatformIO will automatically download libraries and set the correct MCU configurations.
* PlatformIO supports onboard JTAG debugging with breakpoints over the ESP32-S3 USB port.
* ArduinoIDE can still be supported through a small .ino file for people who are more comfortable with ArduinoIDE.

No code changes have been made. Just a refactoring into multiple files.