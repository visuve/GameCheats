file(GLOB SourceFiles "*.cpp")

set(Executable "Doom3BFGTrainer")
add_executable(${Executable} ${SourceFiles})
target_precompile_headers(${Executable} REUSE_FROM HackLib)
target_link_libraries(${Executable} LINK_PRIVATE HackLib EntryPoint)