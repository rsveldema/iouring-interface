#include <array>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <expected>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <slogger/ILogger.hpp>
#include <slogger/Error.hpp>

#include <iuring/IPAddress.hpp>
#include <iuring/MacAddress.hpp>

namespace iuring
{
MacAddress::MacAddress(const std::string& mac)
{
    std::sscanf(mac.c_str(), "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
        &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5]);
}

std::expected<IPAddress, error::Error> IPAddress::parse(const std::string& ip_string)
{
    sockaddr_in sa4;
    if (inet_pton(AF_INET, ip_string.c_str(), &sa4.sin_addr) == 1)
    {
        sa4.sin_family = AF_INET;
        return IPAddress(sa4);
    }

    sockaddr_in6 sa6;
    if (inet_pton(AF_INET6, ip_string.c_str(), &sa6.sin6_addr) == 1)
    {
        sa6.sin6_family = AF_INET6;
        return IPAddress(sa6);
    }

    return std::unexpected(error::Error::HOSTNAME_RESOLVE_FAILED);
}

const std::string MacAddress::to_string(const char sep) const
{
    return std::format("{:02x}{}{:02x}{}{:02x}{}{:02x}{}{:02x}{}{:02x}",
        bytes[0], sep, // layout fix
        bytes[1], sep, // layout fix
        bytes[2], sep, // layout fix
        bytes[3], sep, // layout fix
        bytes[4], sep, // layout fix
        bytes[5]); //
}


std::string IPAddress::to_human_readable_ip_string() const
{
    std::array<char, 128> buffer;

    if (auto* v = get_ipv4())
    {
        const char* source_name =
            inet_ntop(AF_INET, &v->sin_addr, buffer.data(), buffer.size());
        if (!source_name)
        {
            source_name = "<INVALID>";
        }
        return std::string(source_name);
    }

    if (auto* v = get_ipv6())
    {
        const char* source_name =
            inet_ntop(AF_INET6, &v->sin6_addr, buffer.data(), buffer.size());
        if (!source_name)
        {
            source_name = "<INVALID>";
        }
        return std::string(source_name);
    }
    return "?.?.?.?";
}


std::string IPAddress::to_human_readable_string() const
{
    std::array<char, 128> buffer;

    if (const auto* v = get_ipv4())
    {
        const char* source_name =
            inet_ntop(AF_INET, &v->sin_addr, buffer.data(), buffer.size());
        if (!source_name)
        {
            source_name = "<INVALID>";
        }
        return std::format("v4: {}: port {}", source_name, get_port());
    }

    if (const auto* v = get_ipv6())
    {
        const char* source_name =
            inet_ntop(AF_INET6, &v->sin6_addr, buffer.data(), buffer.size());
        if (!source_name)
        {
            source_name = "<INVALID>";
        }
        return std::format("v6: {}: port {}", source_name, get_port());
    }
    return "?.?.?.?";
}

IPAddress create_sock_addr_in(
    const char* addr, const SocketPortID port, logging::ILogger& logger)
{
    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_addr =
        iuring::IPAddress::string_to_ipv4_address(addr, logger);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port =
        htons(static_cast<std::underlying_type_t<SocketPortID>>(port));
    return IPAddress(dest_addr);
}

in_addr IPAddress::string_to_ipv4_address(
    const std::string& _ip_address, logging::ILogger& logger)
{
    in_addr addr;

    std::string ip_address(_ip_address);
    if (StringUtils::ends_with(ip_address, "/32"))
    {
        ip_address = ip_address.substr(0, ip_address.length() - 3);
    }

    // fprintf(stderr, "using: {} instead of {}\n", ip_address.c_str(),
    // _ip_address.c_str());
    if (int ret = inet_pton(AF_INET, ip_address.c_str(), &(addr.s_addr));
        ret != 1)
    {
        if (ret < 0)
            perror("inet_pton - failed");
        else
            LOG_ERROR(logger, "invalid IP address: {}\n", ip_address.c_str());
        abort();
    }
    return addr;
}
} // namespace iuring