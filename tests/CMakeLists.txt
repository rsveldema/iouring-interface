

find_package(GTest REQUIRED)

add_executable(unittests tests.cpp)
target_include_directories(unittests PUBLIC ${GTEST_INCLUDE_DIRS})
target_link_libraries(unittests -lgtest -lgmock io_uring_interface_lib gtest_main)