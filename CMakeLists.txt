cmake_minimum_required(VERSION 3.16)

project("GameCheats" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	message("Enabling parallel build for MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

set(EXECUTABLE_OUTPUT_PATH "..")
set(LIBRARY_OUTPUT_PATH "..")

add_subdirectory("HackLib")
add_subdirectory("MayPayne")
add_subdirectory("HOMM3")
add_subdirectory("R6BO")
add_subdirectory("HiddenAndDangerous")
add_subdirectory("Quake1")

include("GoogleTest.cmake")
include(GoogleTest)
enable_testing()
add_subdirectory("HackLibTests")