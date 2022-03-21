cmake_minimum_required(VERSION 3.5)

project(mysql_client)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

add_executable(mysql_client
main.cpp
)

target_link_libraries(mysql_client
PRIVATE
    conet
)

target_include_directories(mysql_client
PRIVATE
    conet
)
