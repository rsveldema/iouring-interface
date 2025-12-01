
#include <stdio.h>

#include "mocks.hpp"

TEST(check_mocks, declare_tests) {
    printf("hello world\n");

    iuring::mocks::WorkItem work_item;
    iuring::mocks::IOUring mock_ring;
}