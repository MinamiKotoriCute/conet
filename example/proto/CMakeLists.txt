cmake_minimum_required(VERSION 3.6)

project(proto)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS hello.proto)
add_library(proto ${PROTO_SRCS} ${PROTO_HDRS})


target_link_libraries(proto
PUBLIC
    ${Protobuf_LIBRARIES}
)

target_include_directories(proto
PUBLIC
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)
