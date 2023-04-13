include(CTest)

add_executable(test_si5351 test_si5351.cpp)
target_link_libraries(test_si5351 si5351 gtest_main)
add_test(si5351 test_si5351)
