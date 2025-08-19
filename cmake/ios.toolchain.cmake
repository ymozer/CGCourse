# Set the target system name for cross-compiling
set(CMAKE_SYSTEM_NAME iOS)

# Define target architectures for the iOS Simulator
# On Apple Silicon Macs, this is 'arm64'. On Intel Macs, 'x86_64'.
# You can include both to create a universal binary for the simulator.
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architectures for iOS Simulator")

# Set the minimum iOS version you want to support
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum iOS deployment version")

# --- Find and set the iOS Simulator SDK ---
# This is the correct way to run a command inside CMake and get its output
execute_process(
  COMMAND xcrun --sdk iphonesimulator --show-sdk-path
  OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Using iOS Simulator SDK: ${CMAKE_OSX_SYSROOT}")

# Set properties to ensure CMake finds libraries and headers within the SDK
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)