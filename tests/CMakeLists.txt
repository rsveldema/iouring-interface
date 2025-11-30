
find_package(GTest REQUIRED)

add_executable(unittests test_mocks.cpp test_stringutils.cpp test_workpool.cpp test_sendpacket.cpp)
target_include_directories(unittests PUBLIC ${GTEST_INCLUDE_DIRS})
target_link_libraries(unittests -lgtest -lgmock iuring gtest_main)


add_test(NAME googletest
         COMMAND unittests)