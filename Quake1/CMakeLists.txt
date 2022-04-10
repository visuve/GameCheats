file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("Quake1Trainer" ${SourceCode})
target_precompile_headers("Quake1Trainer" REUSE_FROM HackLib)
target_link_libraries("Quake1Trainer" LINK_PRIVATE HackLib)