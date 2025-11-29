#pragma once


#include <net/if.h>
#include <netinet/in.h>
#include <optional>
#include <poll.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <Logger.hpp>
#include <StringUtils.hpp>

#include <IPAddress.hpp>
#include <cassert>
#include <cstring>

#include <ISocket.hpp>

namespace network
{

class SocketImpl : public ISocket
{
public:
    SocketImpl(SocketType type, SocketPortID port, Logger& logger,
        const network::AcceptResult& new_conn_opt)
        : ISocket(type, port, logger)
    {
        m_fd = new_conn_opt.m_new_fd;
        memset(&m_mreq, 0, sizeof(m_mreq));
        assert(m_fd > 0);
    }

    SocketImpl(
        SocketType type, SocketPortID port, Logger& logger, SocketKind kind);


    int get_fd() const override
    {
        return m_fd;
    }


    void dump_info();

    void close() override
    {
        assert(m_fd >= 0);
        ::close(m_fd);
        m_fd = -1;
    }

    int mcast_bind() override;

    void join_multicast_group(
        const std::string& ip_address, const std::string& source_iface) override;
    void leave_multicast_group();

    int get() const
    {
        assert(m_fd > 0);
        return m_fd;
    }

private:
    int m_fd = -1;
    ip_mreq m_mreq{};

    void local_bind(SocketPortID port_id);
};

}