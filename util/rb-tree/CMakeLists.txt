cmake_minimum_required(VERSION 3.15)
set(PROJECT rbtree)

project(${PROJECT})

set(CMAKE_C_STANDARD 99)

add_definitions (-w -Wall -o2 -g)

link_directories(./)

add_executable(${PROJECT} main.c rb_tree.c rb_tree.h)
