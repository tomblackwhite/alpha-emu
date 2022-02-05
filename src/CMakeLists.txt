find_package(Boost 1.72 REQUIRED log)
find_package(fmt 8.0.1 REQUIRED)
find_package(spdlog REQUIRED)
add_library(core
    "core/clock.cc"
    "core/cpu.cc"
    "core/file.cc")
target_compile_options(core PRIVATE)
target_include_directories(core PUBLIC "./include")
target_link_libraries(core PUBLIC Boost::log fmt::fmt spdlog::spdlog)

add_executable(alpha-emu main.cpp)
target_link_libraries(alpha-emu core)
