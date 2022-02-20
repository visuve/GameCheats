file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("Quake1Trainer" ${SourceCode})
target_precompile_headers("Quake1Trainer" PRIVATE "Quake1-PCH.hpp")
target_link_libraries("Quake1Trainer" LINK_PRIVATE HackLib)