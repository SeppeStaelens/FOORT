add_executable(test_kerr test_kerr.cpp)

target_include_directories(test_kerr PUBLIC ${CMAKE_SOURCE_DIR}/FOORT/src)

target_link_libraries(test_kerr PUBLIC config_lib metric_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_kerr)
