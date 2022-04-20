cmake_minimum_required (VERSION 3.8)

project (joc_tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(GTEST_RUN_FLAGS --gtest_color=yes --gtest_repeat=5 --gtest_shuffle)

# Download and unpack googletest at configure time
configure_file(test_libs/CMakeLists.txt googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
${CMAKE_BINARY_DIR}/googletest-build)

# The gtest/gmock targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
include_directories("${gtest_SOURCE_DIR}/include"
    "${gmock_SOURCE_DIR}/include")

enable_testing()

# Utility functions
function(configure_test testExecutable)
    # Link against gtest library and specific test libraries
    set(testLibraries ${${testExecutable}_libs})
    target_link_libraries(${testExecutable} PRIVATE gtest PRIVATE gmock_main PUBLIC ${testLibraries})
    # Create test name as the capitalized form of the test executable
    string(TOUPPER ${testExecutable} testName)
    # Add executable to test suite
    add_test(${testName} ${testExecutable} ${GTEST_RUN_FLAGS})
    # Add tests to coverage reports
    #setup_target_for_coverage(NAME ${testName}_coverage EXECUTABLE ${testExecutable} DEPENDENCIES ${testExecutable})
endfunction(configure_test)

# Common headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_libs/gmock-gtest
)

set(UNIT_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/ut)

# JsonObjectConverter test
add_executable(jsonobjectconverter_test
    ${UNIT_TESTS}/JsonObjectConverter_test.cpp
)
set(jsonobjectconverter_test_libs joclib)
configure_test(jsonobjectconverter_test)

set(JOCLIB_OUTPUT_DIR test_libs/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${JOCLIB_OUTPUT_DIR})
