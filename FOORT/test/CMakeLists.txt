add_executable(test_kerr test_kerr.cpp ${CMAKE_SOURCE_DIR}/FOORT/src/Geometry.h)

# target_include_directories(test_kerr PUBLIC ${CMAKE_SOURCE_DIR}/FOORT/src)

target_link_libraries(test_kerr PUBLIC diagnostics_lib geodesic_lib input_output_lib integrators_lib metric_lib terminations_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_kerr)
