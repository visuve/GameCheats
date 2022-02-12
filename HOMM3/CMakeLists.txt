file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("HOMM3Trainer" ${SourceCode})
target_precompile_headers("HOMM3Trainer" PRIVATE "HOMM3-PCH.hpp")
target_link_libraries("HOMM3Trainer" LINK_PRIVATE HackLib)