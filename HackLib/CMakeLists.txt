file(GLOB SourceFiles "*.cpp" "*.asm")

add_library("HackLib" STATIC ${SourceFiles})

target_precompile_headers("HackLib" PUBLIC "../Mega.pch" PRIVATE "<bcrypt.h>")

target_link_options("HackLib" PUBLIC "/SAFESEH:NO") # Required when using assembly
target_link_libraries("HackLib" LINK_PRIVATE "bcrypt")