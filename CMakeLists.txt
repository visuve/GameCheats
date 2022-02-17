cmake_minimum_required(VERSION 3.16)

project("GameCheats" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH "..")
set(LIBRARY_OUTPUT_PATH "..")

add_subdirectory("HackLib")
add_subdirectory("MayPayne")
add_subdirectory("HOMM3")
add_subdirectory("R6BO")
add_subdirectory("HiddenAndDangerous")