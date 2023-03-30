cmake_minimum_required(VERSION 3.20)

set(ignoreMe ${HOST_BUILD})
if (NOT DEFINED HOST_BUILD)
    include(pico-sdk/pico_sdk_init.cmake)
endif()

project(pico53
        VERSION 0.0.1
        DESCRIPTION "Interactive shell for a Si5351A synthesizer with a Pi Pico"
        HOMEPAGE_URL https://github.com/crazybolillo/pico53
        LANGUAGES C CXX
)

include_directories(include)
add_subdirectory(src)

if (NOT DEFINED HOST_BUILD)
    pico_sdk_init()
    add_subdirectory(pico)
endif()

if (DEFINED HOST_BUILD)
    add_subdirectory(googletest)
    enable_testing()
    add_subdirectory(tests)
endif()
