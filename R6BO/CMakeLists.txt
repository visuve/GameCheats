file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("R6BOTrainer" ${SourceCode})
target_precompile_headers("R6BOTrainer" REUSE_FROM HackLib)
target_link_libraries("R6BOTrainer" LINK_PRIVATE HackLib)