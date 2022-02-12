cmake_minimum_required(VERSION 3.16)

project("hax" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("HackLib")
add_subdirectory("MayPayneTrainer")
