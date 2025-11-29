#pragma once

#include "IPAddress.hpp"

namespace network
{
    class IOUringInterface
    {
    public:
        virtual void submit_connect() = 0;
        virtual void submit_accept() = 0;
        virtual void submit_read() = 0;
        virtual void submit_write() = 0;
        virtual void poll_completion_queues() = 0;
    };
}