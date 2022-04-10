file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("HiddenAndDangerousTrainer" ${SourceCode})
target_precompile_headers("HiddenAndDangerousTrainer" REUSE_FROM HackLib)
target_link_libraries("HiddenAndDangerousTrainer" LINK_PRIVATE HackLib)