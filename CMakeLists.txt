# What CMake to start at
cmake_minimum_required(VERSION 3.12)
set(PRJ_NAME "pico-synth")
set(PICO_CXX_ENABLE_EXCEPTIONS "1")

# Include the subsidiary .cmake file to get the SDK
include(pico_sdk_import.cmake)

# Initalise the SDK
pico_sdk_init()
 
# Set the name and version of the project
project(${PRJ_NAME} VERSION 0.1.0)

include_directories(${PRJ_NAME}
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stk/include
)
 
# Link the Project to a source file (step 4.6)
add_executable(${PRJ_NAME} 
    src/main.cpp
    src/midi.cpp
    src/audio.cpp
    external/stk/src/SineWave.cpp
    external/stk/src/Stk.cpp
)
 
# Link the Project to an extra library (pico_stdlib)
target_link_libraries(${PRJ_NAME} 
    pico_stdlib
    hardware_pwm
)
 
# target_compile_definitions(${PRJ_NAME} PRIVATE
#     PICO_DEFAULT_UART_TX_PIN=0
#     PICO_DEFAULT_UART_RX_PIN=1
#  )


# Create map/bin/hex file etc.)
pico_add_extra_outputs(${PRJ_NAME})