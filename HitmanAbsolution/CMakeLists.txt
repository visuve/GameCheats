file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("HMATrainer" ${SourceCode})
target_precompile_headers("HMATrainer" REUSE_FROM HackLib)
target_link_libraries("HMATrainer" LINK_PRIVATE HackLib)