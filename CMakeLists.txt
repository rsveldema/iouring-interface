cmake_minimum_required(VERSION 3.18)

project(io_uring_interface_suite CXX)

include(CTest)

enable_testing()

# Include directories
set(PUBLIC_INCLUDE_DIRECTORIES include/)
set(PRIVATE_INCLUDE_DIRECTORIES src/)

# Source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.hpp")
file(GLOB_RECURSE INCLUDE_FILES "include/*.hpp")

add_library(iuring SHARED ${SOURCE_FILES} ${INCLUDE_FILES})

target_link_libraries(iuring -luring ${UUID_LIBRARIES})
target_include_directories(iuring PUBLIC ${PUBLIC_INCLUDE_DIRECTORIES})
target_include_directories(iuring PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES})

set_property(TARGET iuring PROPERTY CXX_STANDARD 20)

add_executable(ping tests/ping.cpp)
set_property(TARGET ping PROPERTY CXX_STANDARD 20)
target_link_libraries(ping iuring)

add_executable(echo_server tests/echo_server.cpp)
set_property(TARGET echo_server PROPERTY CXX_STANDARD 20)
target_link_libraries(echo_server iuring)

find_package(GTest)

if(GTest_FOUND)
add_subdirectory(tests)
endif()