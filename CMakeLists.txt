cmake_minimum_required(VERSION 3.5)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER /home/nanhe/RV1109/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /home/nanhe/RV1109/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++)

#uvc camera
project(uvc_camera)

file(GLOB SOURCES  "src/*.c")

#executable
add_executable(uvccamera  ${SOURCES})

target_include_directories(uvccamera
    PRIVATE
    $(PROJECT_SOURCE_NAME)/include
)
