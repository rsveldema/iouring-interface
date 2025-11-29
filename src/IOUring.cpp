#include <ifaddrs.h>
#include <sys/mman.h>

#include <IOUring.hpp>
#include <thread>
#include <ProbeUringFeatures.hpp>

namespace network
{
IOUring::IOUring(Logger& logger, size_t queue_size)
    : IOUringInterface(logger)
    , m_queue_size(queue_size)
{
}

Error IOUring::init()
{
    IOUringInterface::init();

    if (true)
    {
        struct io_uring_params params;
        memset(&params, 0, sizeof(params));
        params.cq_entries = QD * 8;
        params.flags = IORING_SETUP_SUBMIT_ALL | IORING_SETUP_COOP_TASKRUN |
            IORING_SETUP_CQSIZE;

        if (const auto ret = io_uring_queue_init_params(QD, &m_ring, &params);
            ret < 0)
        {
            LOG_ERROR(get_logger(),
                "queue_init failed: %s\n"
                "NB: This requires a kernel version >= 6.0\n",
                strerror(-ret));
            abort();
        }
    }
    else
    {
        if (const auto status = io_uring_queue_init(m_queue_size, &m_ring, 0);
            status != 0)
        {
            return errno_to_error(-status);
        }
    }

    probe_features();

    return setup_buffer_pool();
}


Error IOUring::setup_buffer_pool()
{
    buf_ring_size = (sizeof(io_uring_buf) + buffer_size()) * BUFFERS;
    void* mapped = mmap(NULL, buf_ring_size, PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    if (mapped == MAP_FAILED)
    {
        LOG_ERROR(get_logger(), "buf_ring mmap: %s\n", strerror(errno));
        return Error::MMAP_FAILED;
    }
    buf_ring = (struct io_uring_buf_ring*) mapped;

    io_uring_buf_ring_init(buf_ring);

    io_uring_buf_reg reg;
    memset(&reg, 0, sizeof(reg));
    reg.ring_addr = (unsigned long) buf_ring;
    reg.ring_entries = BUFFERS;
    reg.bgid = 0;

    buffer_base =
        (unsigned char*) buf_ring + sizeof(struct io_uring_buf) * BUFFERS;

    const auto ret = io_uring_register_buf_ring(&m_ring, &reg, 0);
    if (ret)
    {
        LOG_ERROR(get_logger(),
            "buf_ring init failed: %s\n"
            "NB This requires a kernel version >= 6.0\n",
            strerror(-ret));
        return errno_to_error(-ret);
    }

    for (auto i = 0u; i < BUFFERS; i++)
    {
        io_uring_buf_ring_add(buf_ring, get_buffer(i), buffer_size(), i,
            io_uring_buf_ring_mask(BUFFERS), i);
    }
    io_uring_buf_ring_advance(buf_ring, BUFFERS / 2);

    for (int i = BUFFERS / 2; i < BUFFERS; i++)
    {
        m_free_send_ids.push(i);
    }

    return Error::OK;
}


void IOUring::probe_features()
{
    ProbeUringFeatures probe(&m_ring, m_features, get_logger());
}


IOUring::~IOUring()
{
    io_uring_queue_exit(&m_ring);
}

Error IOUring::submit_accept()
{
    return Error::OK;
}
Error IOUring::submit_connect()
{
    return Error::OK;
}
Error IOUring::submit_read()
{
    return Error::OK;
}
Error IOUring::submit_write()
{
    return Error::OK;
}
Error IOUring::poll_completion_queues()
{
    return Error::OK;
}
} // namespace network