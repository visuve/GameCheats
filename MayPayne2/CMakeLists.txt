file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("MaxPayne2Trainer" ${SourceCode})
target_precompile_headers("MaxPayne2Trainer" PRIVATE "MaxPayne2-PCH.hpp")
target_link_libraries("MaxPayne2Trainer" LINK_PRIVATE HackLib)