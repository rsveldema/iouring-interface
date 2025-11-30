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

struct AcceptResult
{
    int m_new_fd;
    IPAddress m_address;
};

struct ConnectResult
{
    int status;
    IPAddress m_address;
};

struct CloseResult
{
    int status;
};


class ISocket
{
public:
    ISocket(SocketType type, SocketPortID port, Logger& logger)
        : m_type(type)
        , m_port(port)
        , m_logger(logger)
    {
    }

    virtual ~ISocket() = default;

    virtual int get_fd() const = 0;

    virtual int mcast_bind() = 0;
    virtual void join_multicast_group(
        const std::string& ip_address, const std::string& source_iface) = 0;

    virtual void close() = 0;

public:
    SocketPortID get_port() const
    {
        return m_port;
    }

    bool is_stream() const
    {
        switch (m_type)
        {
        case SocketType::IPV4_TCP:
        case SocketType::IPV6_TCP:
            return true;
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
};

} // namespace network