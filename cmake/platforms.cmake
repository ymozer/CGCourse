# cmake/platforms.cmake
# This module detects the target platform and sets up convenient variables.
include_guard() # Prevents this module from being included more than once

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(PLATFORM_IS_EMSCRIPTEN TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(PLATFORM_IS_ANDROID TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(PLATFORM_IS_IOS TRUE)
    set(PLATFORM_IS_APPLE TRUE)
elseif(APPLE)
    set(PLATFORM_IS_MACOS TRUE)
    set(PLATFORM_IS_APPLE TRUE)
    set(PLATFORM_IS_DESKTOP TRUE)
elseif(WIN32)
    set(PLATFORM_IS_WINDOWS TRUE)
    set(PLATFORM_IS_DESKTOP TRUE)
elseif(UNIX)
    # Note: Emscripten also defines UNIX, so it must be checked first.
    set(PLATFORM_IS_LINUX TRUE)
    set(PLATFORM_IS_DESKTOP TRUE)
else()
    message(FATAL_ERROR "Unsupported platform detected!")
endif()

# Convenience variable for all mobile platforms
if(PLATFORM_IS_ANDROID OR PLATFORM_IS_IOS)
    set(PLATFORM_IS_MOBILE TRUE)
endif()

add_library(platform_definitions INTERFACE)

if(PLATFORM_IS_DESKTOP)
    target_compile_definitions(platform_definitions INTERFACE PLATFORM_DESKTOP=1)
elseif(PLATFORM_IS_MOBILE)
    target_compile_definitions(platform_definitions INTERFACE PLATFORM_MOBILE=1)
endif()

if(PLATFORM_IS_WINDOWS)
    target_compile_definitions(platform_definitions INTERFACE
        PLATFORM_WINDOWS=1
    )
elseif(PLATFORM_IS_APPLE AND PLATFORM_IS_DESKTOP)
    target_compile_definitions(platform_definitions INTERFACE
        PLATFORM_APPLE=1
        PLATFORM_MACOS=1
    )
elseif(PLATFORM_IS_LINUX)
    target_compile_definitions(platform_definitions INTERFACE
        PLATFORM_LINUX=1
    )
elseif(PLATFORM_IS_EMSCRIPTEN)
    target_compile_definitions(platform_definitions INTERFACE
        PLATFORM_EMSCRIPTEN=1
    )
    target_include_directories(platform_definitions SYSTEM INTERFACE
        "${EMSDK}/upstream/emscripten/system/include"
    )
elseif(PLATFORM_IS_IOS)
    target_compile_definitions(platform_definitions INTERFACE
       PLATFORM_APPLE=1
       PLATFORM_IOS=1
    )
elseif(PLATFORM_IS_ANDROID)
    target_compile_definitions(platform_definitions INTERFACE
       PLATFORM_ANDROID=1
   )
endif()