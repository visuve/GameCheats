file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("HiddenAndDangerousTrainer" ${SourceCode})
target_precompile_headers("HiddenAndDangerousTrainer" PRIVATE "HiddenAndDangerous-PCH.hpp")
target_link_libraries("HiddenAndDangerousTrainer" LINK_PRIVATE HackLib)