file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("FalloutNVTrainer" ${SourceCode})
target_precompile_headers("FalloutNVTrainer" REUSE_FROM HackLib)
target_link_libraries("FalloutNVTrainer" LINK_PRIVATE HackLib)