cmake_minimum_required(VERSION 3.5)

project(protobuf_tcp_client)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

add_executable(protobuf_tcp_client
main.cpp
)

target_link_libraries(protobuf_tcp_client
PRIVATE
    conet
)

target_include_directories(protobuf_tcp_client
PRIVATE
    conet
)
