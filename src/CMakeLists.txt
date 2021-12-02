
add_library(core "core/clock.cpp")
target_include_directories(core PUBLIC "./include")
add_executable(alpha-emu main.cpp)
target_link_libraries(alpha-emu core)
