file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("HackLibTests" ${SourceCode})

add_test(HackLibTests HackLibTests)

target_precompile_headers("HackLibTests" PRIVATE "HackLibTests-PCH.hpp")
target_link_libraries("HackLibTests" LINK_PRIVATE HackLib gtest)