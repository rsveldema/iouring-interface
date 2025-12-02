
find_package(GTest REQUIRED)

add_executable(iuring_unittests test_mocks.cpp test_stringutils.cpp test_workpool.cpp test_sendpacket.cpp)
target_include_directories(iuring_unittests PUBLIC ${GTEST_INCLUDE_DIRS})
target_link_libraries(iuring_unittests -lgtest -lgmock iuring gtest_main)


add_test(NAME googletest
         COMMAND iuring_unittests)