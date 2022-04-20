cmake_minimum_required(VERSION 3.16)

project("GameCheats" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

include("GoogleTest.cmake")
include(GoogleTest)
enable_testing()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
endif()

set(EXECUTABLE_OUTPUT_PATH "..")
set(LIBRARY_OUTPUT_PATH "..")

add_subdirectory("HackLib")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	add_subdirectory("MayPayne")
	add_subdirectory("HOMM3")
	add_subdirectory("R6BO")
	add_subdirectory("HiddenAndDangerous")
	add_subdirectory("MayPayne2")
	add_subdirectory("Hitman2012")
	add_subdirectory("FalloutNV")
	add_subdirectory("Hitman2000")
	add_subdirectory("Hitman2002")
	add_subdirectory("Hitman2004")
	add_subdirectory("Template")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	add_subdirectory("Quake1")
	add_subdirectory("Hitman2016")
	add_subdirectory("Template")
endif()

add_subdirectory("HackLibTests")