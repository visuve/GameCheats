file(GLOB SourceFiles "*.cpp" "*.asm")

set(Executable "Quake1RTrainer")
add_executable(${Executable} ${SourceFiles})
target_precompile_headers(${Executable} REUSE_FROM HackLib)
target_link_libraries(${Executable} LINK_PRIVATE HackLib EntryPoint)