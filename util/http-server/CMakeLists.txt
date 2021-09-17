cmake_minimum_required(VERSION 3.15)
project(http_server C)

set(CMAKE_C_STANDARD 99)

add_executable(http_server main.c threadpool.h threadpool.c)