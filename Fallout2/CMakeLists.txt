file(GLOB SourceFiles "*.cpp")

set(Executable "Fallout2Trainer")
add_executable(${Executable} ${SourceFiles})
target_precompile_headers(${Executable} REUSE_FROM HackLib)
target_link_libraries(${Executable} LINK_PRIVATE HackLib EntryPoint)