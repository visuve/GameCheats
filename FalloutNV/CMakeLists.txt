file(GLOB SourceCode "*.cpp" "*.hpp")

set(Executable "FalloutNVTrainer")
add_executable(${Executable} ${SourceCode})
target_precompile_headers(${Executable} REUSE_FROM HackLib)
target_link_libraries(${Executable} LINK_PRIVATE HackLib)