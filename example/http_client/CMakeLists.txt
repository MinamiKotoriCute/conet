cmake_minimum_required(VERSION 3.5)

project(example_http_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

set(example_http_client_src
    main.cpp
)

add_executable(example_http_client
    ${example_http_client_src}
)

target_link_libraries(example_http_client
PRIVATE
    conet
)

target_include_directories(example_http_client
PRIVATE
    conet
)
