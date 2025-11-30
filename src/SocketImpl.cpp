#include <SocketImpl.hpp>
#include <cassert>
#include <cerrno>
#include <cstring>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <Logger.hpp>

namespace network
{
#include <fcntl.h>

SocketImpl::SocketImpl(
    SocketType type, SocketPortID port, Logger& logger, SocketKind kind)
    : ISocket(type, port, logger)
{
    memset(&m_mreq, 0, sizeof(m_mreq));
    int non_blocking_option = 0;
    if (false)
    {
        non_blocking_option |= SOCK_NONBLOCK;
    }

    switch (type)
    {
    case SocketType::IPV4_UDP:
        m_fd = socket(AF_INET, SOCK_DGRAM | non_blocking_option, 0);
        LOG_DEBUG(logger, "socket-v4 %d with dgram type!", m_fd);
        break;
    case SocketType::IPV4_TCP:
        m_fd = socket(AF_INET, SOCK_STREAM | non_blocking_option, 0);
        LOG_DEBUG(logger, "socket-v4 %d with stream type!", m_fd);
        break;
    case SocketType::IPV6_UDP:
        m_fd = socket(AF_INET6, SOCK_DGRAM | non_blocking_option, 0);
        LOG_DEBUG(logger, "socket-v6 %d with dgram type!", m_fd);
        abort();
        break;
    case SocketType::IPV6_TCP:
        m_fd = socket(AF_INET6, SOCK_STREAM | non_blocking_option, 0);
        LOG_DEBUG(logger, "socket-v6 %d with stream type!", m_fd);
        break;
    }

    assert(m_fd > 0);

    int set_option_on = 1;
    // it is important to do "reuse address" before bind, not after
    int res = setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &set_option_on,
        sizeof(set_option_on));
    assert(res == 0);

    switch (kind)
    {
    case SocketKind::CLIENT_SOCKET: {
        local_bind(static_cast<SocketPortID>(9090));

        int val = 1;
        int ret = setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
        if (ret == -1)
        {
            perror("setsockopt()");
            abort();
        }
        break;
    }

    case SocketKind::MULTICAST_PACKET_SOCKET: {
        local_bind(port);

        int ttl = 1;
        if (setsockopt(m_fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)))
        {
            perror("setsockopt IP_MULTICAST_TTL failed");
            abort();
        }
        break;
    }

    case SocketKind::SERVER_STREAM_SOCKET: {
        const auto tmp_port =
            static_cast<std::underlying_type_t<SocketPortID>>(port);

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(tmp_port);

        int err =
            bind(m_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
        if (err < 0)
        {
            fprintf(stderr, "bind error: %s (port %d)\n", strerror(errno),
                tmp_port);
            abort();
        }

        err = ::listen(m_fd, 1024);
        if (err < 0)
        {
            fprintf(stderr, "listen error: %s, port %d\n", strerror(errno),
                tmp_port);
            abort();
        }
        break;
    }
    }
}


void SocketImpl::dump_info()
{
    assert(m_fd >= 0);
    sockaddr_storage s;
    socklen_t sz = sizeof(s);

    if (getsockname(m_fd, (struct sockaddr*) &s, &sz))
    {
        LOG_ERROR(get_logger(), "getsockname failed\n");
        return;
    }

    const auto* sa = (struct sockaddr_in*) &s;

    const auto port = ntohs(sa->sin_port);
    const auto addr = sa->sin_addr;
    LOG_DEBUG(get_logger(), "DOUBLE CHECK -----> port bound to %d: %s", port,
        inet_ntoa(addr));
}


int SocketImpl::mcast_bind()
{
    assert(m_fd >= 0);

    ip_mreq req = m_mreq;

    int err = setsockopt(m_fd, IPPROTO_IP, IP_MULTICAST_IF, &req, sizeof(req));
    if (err)
    {
        perror("setsockopt IP_MULTICAST_IF failed: %m");
        return -1;
    }
    return 0;
}


void SocketImpl::join_multicast_group(
    const std::string& ip_address, const std::string& source_iface)
{
    assert(m_fd >= 0);
    assert(!ip_address.empty());
    assert(!source_iface.empty());

    LOG_DEBUG(get_logger(), "PTP: join_multicast_group: '%s', interface '%s'\n",
        ip_address.c_str(), source_iface.c_str());

    m_mreq.imr_multiaddr =
        IPAddress::string_to_ipv4_address(ip_address, get_logger());
    m_mreq.imr_interface =
        IPAddress::string_to_ipv4_address(source_iface, get_logger());

    if (int ret = setsockopt(
            m_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &m_mreq, sizeof(m_mreq));
        ret < 0)
    {
        perror("setsockopt");
        abort();
    }

    int off = 0;
    if (int err =
            setsockopt(m_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &off, sizeof(off));
        err)
    {
        perror("setsockopt IP_MULTICAST_LOOP failed");
        abort();
    }
}

void SocketImpl::leave_multicast_group()
{
    assert(m_fd >= 0);
    LOG_DEBUG(get_logger(), "PTP: leave_multicast_group\n");
    if (int ret = setsockopt(
            m_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &m_mreq, sizeof(m_mreq));
        ret < 0)
    {
        perror("setsockopt");
        exit(1);
    }
}

void SocketImpl::local_bind(SocketPortID port_id)
{
    assert(m_fd >= 0);

    const auto port_value =
        static_cast<std::underlying_type_t<network::SocketPortID>>(port_id);

    LOG_DEBUG(get_logger(), "PTP: binding interface to port %d\n", port_value);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_value);

    if (int ret = ::bind(m_fd, (sockaddr*) &addr, sizeof(addr)); ret < 0)
    {
        perror("bind");
        LOG_ERROR(
            get_logger(), "failed to bind to port %d, exiting", port_value);
        exit(1);
    }

    dump_info();
}
} // namespace network