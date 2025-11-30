#pragma once

#include <map>

#include "IPAddress.hpp"
#include "Logger.hpp"
#include "UringDefs.hpp"
#include "WorkPool.hpp"

namespace network
{
class IOUringInterface
{
public:
    IOUringInterface(
        Logger& logger, const std::string& interface_name, bool tune)
        : m_logger(logger)
        , m_pool(logger)
        , m_interface_name(interface_name)
        , m_tune(tune)
    {
    }

    WorkPool& get_pool()
    {
        return m_pool;
    }

    virtual Error init();

    virtual Error poll_completion_queues() = 0;

    virtual void submit(WorkItem& item) = 0;

    virtual void submit_connect(const std::shared_ptr<ISocket>& socket,
        const IPAddress& target, connect_callback_func_t handler) = 0;

    /** This accepts new connections from other machines.
     * Note that this requires that the socket is opened with
     *  SocketKind::SERVER_STREAM_SOCKET
     * As only server sockets can accept new connections.
     * We check this by asserting the correct behavior here to safeguard this.
     */
    virtual void submit_accept(const std::shared_ptr<ISocket>& socket,
        accept_callback_func_t handler) = 0;

    virtual void submit_recv(const std::shared_ptr<ISocket>& socket,
        recv_callback_func_t handler) = 0;

    /** The steps for sending a packet:
     *      - This returns a work-item where you can retrieve the SendPacket
     * object from
     *      - Then with that send packet you append your dara
     *      - Then you call submit on the work-item.
     *      - The WorkItem::submit() method then has the callback arg.
     */
    virtual std::shared_ptr<WorkItem> submit_send(
        const std::shared_ptr<ISocket>& socket) = 0;

    virtual void submit_close(const std::shared_ptr<ISocket>& socket,
        close_callback_func_t handler) = 0;

    //virtual void submit_all_requests() = 0;


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
    Logger& m_logger;
    WorkPool m_pool;
    std::string m_interface_ip4;
    std::string m_interface_ip6;
    std::string m_interface_name;
    bool m_tune = true;
    std::optional<std::string> mac_opt;

protected:
    bool m_initialized = false;

    void tune();

    void set_interface_ip4(const std::string& ip)
    {
        m_interface_ip4 = ip;
        LOG_INFO(get_logger(), "interface IP4 set to " + ip);
    }

    void set_interface_ip6(const std::string& ip)
    {
        m_interface_ip6 = ip;
        LOG_INFO(get_logger(), "interface IP6 set to " + ip);
    }

    virtual bool try_get_interface_ip();
    virtual bool retrieve_interface_ip();
};

Error errno_to_error(int err);

} // namespace network