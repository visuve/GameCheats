file(GLOB SourceFiles "*.cpp")

set(UnitTests "HackLibTests")
add_executable(${UnitTests} ${SourceFiles})

add_test(HackLibTests HackLibTests)

target_precompile_headers(${UnitTests} PRIVATE "<gtest/gtest.h>")
target_link_libraries(${UnitTests} LINK_PRIVATE HackLib gtest)
target_link_options(${UnitTests} PRIVATE /PROFILE)