cmake_minimum_required (VERSION 2.6)
project (cgafaq)

# For make-based builds, defines make target named test.
# For Visual Studio builds, defines Visual Studio project named RUN_TESTS.
enable_testing()

include_directories($ENV{GTEST_ROOT}/include)
link_directories($ENV{GTEST_ROOT})

add_executable(runUnitTests
    cgafaqtest.cpp
)

target_link_libraries(runUnitTests pthread)
target_link_libraries(runUnitTests gtest)
target_link_libraries(runUnitTests gtest_main)

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)
