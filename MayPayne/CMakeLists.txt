file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("MaxPayneTrainer" ${SourceCode})
target_precompile_headers("MaxPayneTrainer" REUSE_FROM  HackLib)
target_link_libraries("MaxPayneTrainer" LINK_PRIVATE HackLib)