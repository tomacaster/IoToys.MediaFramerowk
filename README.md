# IoToys.MultimediaFramework

A C++23 project using GStreamer. This project demonstrates basic GStreamer usage and is built with CMake.

## Prerequisites
- C++23 compatible compiler (e.g., GCC 13+)
- CMake 3.16+
- GStreamer development libraries (`libgstreamer1.0-dev`, `libgstreamer-plugins-base1.0-dev`)

## Build Instructions

1. Install dependencies (on Ubuntu/Debian):
   ```bash
   sudo apt update
   sudo apt install build-essential cmake libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
   ```

2. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

3. Run the example:
   ```bash
   ./IoToysMultimediaFramework
   ```

## Project Structure
- `main.cpp`: Example GStreamer usage
- `CMakeLists.txt`: Build configuration

## Notes
- Make sure GStreamer is installed and working on your system.
