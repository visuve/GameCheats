file(GLOB SourceFiles "*.cpp" "*.asm")

set(Executable "Hitman2021")
add_executable(${Executable} ${SourceFiles})
target_precompile_headers(${Executable} REUSE_FROM HackLib)
target_link_libraries(${Executable} LINK_PRIVATE HackLib EntryPoint)