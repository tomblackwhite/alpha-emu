
add_library(core "core/clock.cc"
                    "core/cpu.cc")
target_compile_options(core PRIVATE -O2)
target_include_directories(core PUBLIC "./include")
add_executable(alpha-emu main.cpp)
target_link_libraries(alpha-emu core)
