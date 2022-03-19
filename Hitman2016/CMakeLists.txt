file(GLOB SourceCode "*.cpp" "*.hpp")

add_executable("Hitman2016Trainer" ${SourceCode})
target_precompile_headers("Hitman2016Trainer" PRIVATE "HM2016-PCH.hpp")
target_link_libraries("Hitman2016Trainer" LINK_PRIVATE HackLib)