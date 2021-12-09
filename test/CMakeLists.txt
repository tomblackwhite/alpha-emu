
#引用Boost TEST框架
find_package(Boost 1.72 REQUIRED unit_test_framework)
add_executable(alpha-emu-test main.cpp)
target_link_libraries(alpha-emu-test core Boost::unit_test_framework)
target_compile_options(alpha-emu-test PRIVATE -O2)
add_test(NAME alpha-emu-test COMMAND alpha-emu-test)
