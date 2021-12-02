
add_executable(alpha-emu-test main.cpp)
target_link_libraries(alpha-emu-test core)
add_test(NAME alpha-emu-test COMMAND alpha-emu-test)
