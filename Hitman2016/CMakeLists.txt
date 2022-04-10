file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("Hitman2016Trainer" ${SourceCode})
target_precompile_headers("Hitman2016Trainer" REUSE_FROM HackLib)
target_link_libraries("Hitman2016Trainer" LINK_PRIVATE HackLib)