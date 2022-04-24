cmake_minimum_required(VERSION 3.16)

project("GameCheats" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH "..")
set(LIBRARY_OUTPUT_PATH "..")

if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4 /WX")
endif()

add_subdirectory("HackLib")
include_directories("HackLib")

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
	add_subdirectory("Hitman2006")
	add_subdirectory("Stalker2007")
	add_subdirectory("Template")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	add_subdirectory("Quake1")
	add_subdirectory("Hitman2016")
	add_subdirectory("Template")
endif()

include("GoogleTest.cmake")

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

include(GoogleTest)
add_subdirectory("HackLibTests")
enable_testing()

