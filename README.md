# Adafruit LIS3DH [![Build Status](https://github.com/adafruit/Adafruit_LIS3DH/workflows/Arduino%20Library%20CI/badge.svg)](https://github.com/adafruit/Adafruit_LIS3DH/actions)[![Documentation](https://github.com/adafruit/ci-arduino/blob/master/assets/doxygen_badge.svg)](http://adafruit.github.io/Adafruit_LIS3DH/html/index.html)

<a href="https://www.adafruit.com/products/2809"><img src="assets/image.jpg?raw=true" width="500px" /></a>

This is the Arduino library for the Adafruit LIS3DH 3-axis accelerometers.

## Supported Hardware
* [LIS3DH 3-Axis Accelerometer](https://www.adafruit.com/products/2809)

This sensor communicates over I2C or SPI (our library code supports both) so you can share it with a bunch of other sensors on the same I2C bus.
There's an address selection pin so you can have two accelerometers share an I2C bus.

Check out the links above for our tutorials and wiring diagrams

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

# Installation
To install, use the Arduino Library Manager and search for "Adafruit-MLX90614-Library" and install the library.

## Dependencies
 * [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO)
 * [Adafruit Unified Sensor Driver](https://github.com/adafruit/Adafruit_Sensor)

# Contributing

Contributions are welcome! Please read our [Code of Conduct](https://github.com/adafruit/Adafruit_LIS3DH/blob/master/CODE_OF_CONDUCT.md>)
before contributing to help this project stay welcoming.

## Documentation and doxygen
Documentation is produced by doxygen. Contributions should include documentation for any new code added.

Some examples of how to use doxygen can be found in these guide pages:

https://learn.adafruit.com/the-well-automated-arduino-library/doxygen

https://learn.adafruit.com/the-well-automated-arduino-library/doxygen-tips

## Formatting and clang-format
This library uses [`clang-format`](https://releases.llvm.org/download.html) to standardize the formatting of `.cpp` and `.h` files. 
Contributions should be formatted using `clang-format`:

The `-i` flag will make the changes to the file.
```bash
clang-format -i *.cpp *.h
```
If you prefer to make the changes yourself, running `clang-format` without the `-i` flag will print out a formatted version of the file. You can save this to a file and diff it against the original to see the changes.

Note that the formatting output by `clang-format` is what the automated formatting checker will expect. Any diffs from this formatting will result in a failed build until they are addressed. Using the `-i` flag is highly recommended.

### clang-format resources
  * [Binary builds and source available on the LLVM downloads page](https://releases.llvm.org/download.html)
  * [Documentation and IDE integration](https://clang.llvm.org/docs/ClangFormat.html)

## About this Library
Written by Bryan Siepert / K. Townsend / Limor Fried for Adafruit Industries.
BSD license, check license.txt for more information
All text above must be included in any redistribution

To install, use the Arduino Library Manager and search for "Adafruit LIS3DH" and install the library.
