cmake_minimum_required(VERSION 3.21)
project(RingBuffer C)

set(CMAKE_C_STANDARD 23)

add_executable(RingBuffer example.c RingBuffer.c RingBuffer.h libs.h)
