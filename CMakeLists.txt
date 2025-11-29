cmake_minimum_required(VERSION 3.18)

project(io_uring_interface_suite CXX)

add_library(io_uring_interface_lib
    src/IOUring.cpp
    src/StringUtils.cpp
)

target_link_libraries(io_uring_interface_lib -luring ${UUID_LIBRARIES})
target_include_directories(io_uring_interface_lib PUBLIC include PUBLIC ${PROJECT_BINARY_DIR})
set_property(TARGET io_uring_interface_lib PROPERTY CXX_STANDARD 20)

add_executable(io_uring_interface src/main.cpp)
set_property(TARGET io_uring_interface PROPERTY CXX_STANDARD 20)
target_link_libraries(io_uring_interface io_uring_interface_lib)

find_package(GTest)

if(GTest_FOUND)
add_subdirectory(tests)
endif()