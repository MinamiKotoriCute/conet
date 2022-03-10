cmake_minimum_required(VERSION 3.5)

project(tcp_server)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

add_executable(tcp_server
    main.cpp
)

target_link_libraries(tcp_server
PRIVATE
    conet
)

target_include_directories(tcp_server
PRIVATE
    conet
)
