file(GLOB SourceFiles "*.cpp")

add_library("HackLib" STATIC ${SourceFiles})

target_precompile_headers("HackLib" PUBLIC "../Mega.pch" PRIVATE "<bcrypt.h>")

target_link_libraries("HackLib" LINK_PRIVATE "bcrypt")