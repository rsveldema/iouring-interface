#pragma once

#include "IOUringInterface.hpp"

#include <liburing.h>

namespace network
{
    class IOUring : public IOUringInterface
    {
    public:
         void submit_connect()  { }
         void submit_accept()  { }
         void submit_read()  { }
         void submit_write()  { }
         void poll_completion_queues()  { }
    };
}