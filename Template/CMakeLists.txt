file(GLOB SourceFiles "*.cpp" "*.asm")

set(Executable "Template")
add_executable(${Executable} ${SourceFiles})
target_precompile_headers(${Executable} REUSE_FROM HackLib)
target_link_options(${Executable} PUBLIC "/SAFESEH:NO") # Required when using assembly
target_link_libraries(${Executable} LINK_PRIVATE HackLib EntryPoint)