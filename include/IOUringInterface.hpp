#pragma once

#include <map>

#include "IPAddress.hpp"
#include "UringDefs.hpp"
#include <Logger.hpp>

namespace network
{
class IOUringInterface
{
public:
    IOUringInterface(Logger& logger)
        : m_logger(logger)
    {
    }

    virtual Error init();
    virtual Error submit_connect() = 0;
    virtual Error submit_accept() = 0;
    virtual Error submit_read() = 0;
    virtual Error submit_write() = 0;
    virtual Error poll_completion_queues() = 0;

    /** @returns aa:bb:cc:dd:ee:ff
     */
    std::string get_my_mac_address();

    const std::string get_interface_ip4() const
    {
        return m_interface_ip4;
    }

    const std::string get_interface_name() const
    {
        return m_interface_name;
    }

    Logger& get_logger()
    {
        return m_logger;
    }

private:
    bool m_tune = true;
    Logger& m_logger;
    std::string m_interface_ip4;
    std::string m_interface_ip6;
    std::string m_interface_name;
    std::optional<std::string> mac_opt;

protected:
    std::map<UringFeature, bool> m_features;

    void tune();

    void set_interface_ip4(const std::string& ip) {
        m_interface_ip4 = ip;
        LOG_INFO(get_logger(), "interface IP4 set to " + ip);
    }

    void set_interface_ip6(const std::string& ip) {
        m_interface_ip6 = ip;
        LOG_INFO(get_logger(), "interface IP6 set to " + ip);
    }

    virtual bool try_get_interface_ip();
    virtual bool retrieve_interface_ip();
};

Error errno_to_error(int err);

} // namespace network