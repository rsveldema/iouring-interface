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

namespace network
{
class ISocket
{
public:
    ISocket(SocketType type, SocketPortID port, Logger& logger, SocketKind kind,
        int fd)
        : m_type(type)
        , m_port(port)
        , m_logger(logger)
        , m_kind(kind)
        , m_fd(fd)
    {
    }

    virtual ~ISocket() = default;

    int get_fd() const { return m_fd; }

    virtual int mcast_bind() = 0;
    virtual void join_multicast_group(
        const std::string& ip_address, const std::string& source_iface) = 0;

    SocketPortID get_port() const
    {
        return m_port;
    }

    SocketKind get_kind() const { return m_kind; }

    bool is_stream() const
    {
        switch (m_type)
        {
        case SocketType::IPV4_TCP:
        case SocketType::IPV6_TCP:
            return true;
        case SocketType::UNKNOWN:
        case SocketType::IPV4_UDP:
        case SocketType::IPV6_UDP:
            return false;
        }
        return false;
    }

    SocketType get_type() const
    {
        return m_type;
    }

    in_port_t get_port_as_int() const
    {
        const auto port_value =
            static_cast<std::underlying_type_t<network::SocketPortID>>(
                get_port());
        return port_value;
    }

    Logger& get_logger()
    {
        return m_logger;
    }

private:
    SocketType m_type;
    SocketPortID m_port;
    Logger& m_logger;
    SocketKind m_kind;
    int m_fd;
};

} // namespace network