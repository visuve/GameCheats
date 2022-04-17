file(GLOB SourceCode "*.cpp" "*.hpp")

add_library("HackLib" STATIC ${SourceCode})
target_precompile_headers("HackLib" PRIVATE "../Mega.pch")
target_link_libraries("HackLib" LINK_PRIVATE "bcrypt")