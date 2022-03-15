cmake_minimum_required(VERSION 3.5)


FIND_PACKAGE( Boost 1.78 )

project(conet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

set(conet_src
    defer.h
    error_info.cpp
    error_info.h
    error.cpp
    error.h
    pack_coder.cpp
    pack_coder.h
    pack_maker.cpp
    pack_maker.h
    pack_parser.cpp
    pack_parser.h
    pack_tcp_reader.cpp
    pack_tcp_reader.h
    protobuf_tcp_client.h
    result_impl.cpp
    result_impl.h
    result.h
    tcp_client.cpp
    tcp_client.h
    tcp_server.cpp
    tcp_server.h
    url_parser.cpp
    url_parser.h
)

add_library(conet
    ${conet_src}
)

target_link_libraries(conet
PUBLIC
    -pthread
    glog
    protobuf
)

target_include_directories(conet
INTERFACE
    ..
PUBLIC
    ${Boost_INCLUDE_DIR}
)

set_target_properties(conet PROPERTIES LINKER_LANGUAGE CXX)
