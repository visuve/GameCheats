add_library("EntryPoint" STATIC "Main.cpp")
target_precompile_headers("EntryPoint" REUSE_FROM HackLib)
target_link_libraries("EntryPoint" LINK_PRIVATE HackLib)