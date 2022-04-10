file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("HOMM3Trainer" ${SourceCode})
target_precompile_headers("HOMM3Trainer" REUSE_FROM HackLib)
target_link_libraries("HOMM3Trainer" LINK_PRIVATE HackLib)