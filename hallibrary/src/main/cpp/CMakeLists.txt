cmake_minimum_required(VERSION 3.18.1)
project("hallibrary")
add_library(
        hallibrary
        SHARED
        native-lib.cpp)
find_library(
        log-lib
        log)
target_link_libraries(
        hallibrary
        ${log-lib})