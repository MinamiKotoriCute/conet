cmake_minimum_required(VERSION 3.14)

# conet requires at least c++20
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(conet_test
    test_result.cpp
    test_url_parser.cpp
)

target_link_libraries(conet_test
PRIVATE
    conet
    gtest_main
)

target_include_directories(conet_test
PRIVATE
    conet
)

include(GoogleTest)
gtest_discover_tests(conet_test)
