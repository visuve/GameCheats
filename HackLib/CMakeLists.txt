file(GLOB SourceCode "*.cpp" "*.hpp")

add_library("HackLib" STATIC ${SourceCode})
target_precompile_headers("HackLib" PRIVATE "../Mega.pch")