include(CTest)

add_executable(test_parse_command test_si5351.cpp)
target_link_libraries(test_parse_command gtest_main)
add_test(parse_command, test_parse_command)
