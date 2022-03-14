set(CMAKE_AUTOMOC ON) # Meta-Object Compiler
set(CMAKE_AUTORCC ON) # Resource Compiler
set(CMAKE_AUTOUIC ON) # User Interface Compiler


find_package(Boost 1.72 REQUIRED log)
find_package(fmt 8.0.1 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

file(GLOB core_src
    "./core/*.cc")
add_library(core
    ${core_src})
target_compile_options(core PRIVATE)
target_include_directories(core PUBLIC "./include")
target_link_libraries(core PUBLIC Boost::log fmt::fmt spdlog::spdlog)

file(GLOB ui_src
    "./ui/*.cc"
    "./ui/*.ui")
add_library(ui ${ui_src})
target_include_directories(ui PUBLIC "./include")
target_link_libraries(ui PUBLIC Qt5::Widgets)
add_executable(alpha-emu main.cpp)


target_include_directories(alpha-emu PUBLIC "./ui")
target_link_libraries(alpha-emu core ui glfw Vulkan::Vulkan glm::glm)
