#pragma once

#include <stack>

#include "Error.hpp"
#include "IOUringInterface.hpp"

#include <liburing.h>

static constexpr size_t DEFAULT_QUEUE_SIZE = 64;

namespace network
{
class IOUring : public IOUringInterface
{
public:
    IOUring(Logger& logger, size_t queue_size = DEFAULT_QUEUE_SIZE);
    ~IOUring();

    IOUring(const IOUring&) = delete;
    IOUring& operator=(const IOUring&) = delete;
    IOUring(IOUring&&) = delete;
    IOUring& operator=(IOUring&&) = delete;

    Error init() override;
    Error submit_connect() override;
    Error submit_accept() override;
    Error submit_read() override;
    Error submit_write() override;
    Error poll_completion_queues() override;


private:
    static constexpr auto QD = 64;
    static constexpr auto BUF_SHIFT = 12; /* 4k */
    static constexpr auto CQES = (QD * 16);
    static constexpr auto BUFFERS = CQES;

    size_t m_queue_size;

    io_uring m_ring{};
    io_uring_buf_ring* buf_ring = nullptr;
    size_t buf_ring_size = 0;
    size_t buf_shift = BUF_SHIFT;
    unsigned char* buffer_base = nullptr;
    std::stack<int> m_free_send_ids;


    Error setup_buffer_pool();
    void probe_features();

    size_t buffer_size() const
    {
        assert(buf_shift > 0);
        return 1U << buf_shift;
    }

    uint8_t* get_buffer(int idx)
    {
        assert(buf_shift > 0);
        return buffer_base + (idx << buf_shift);
    }

};
} // namespace network