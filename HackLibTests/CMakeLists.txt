file(GLOB SourceCode "*.cpp" "*.hpp")

set(VS_UNIT_TEST "$ENV{VCINSTALLDIR}Auxiliary\\VS\\UnitTest\\")

include_directories("${VS_UNIT_TEST}include")
link_directories("${VS_UNIT_TEST}lib")

add_library("HackLibTests" SHARED ${SourceCode})

target_precompile_headers("HackLibTests" PRIVATE "HackLibTests-PCH.hpp")
target_link_libraries("HackLibTests" LINK_PRIVATE HackLib)