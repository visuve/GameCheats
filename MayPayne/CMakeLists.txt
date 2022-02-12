file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("MaxPayneTrainer" ${SourceCode})
target_precompile_headers("MaxPayneTrainer" PRIVATE "MaxPayne-PCH.hpp")
target_link_libraries("MaxPayneTrainer" LINK_PRIVATE HackLib)