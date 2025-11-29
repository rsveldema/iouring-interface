#include <IOUringInterface.hpp>

namespace network
{
Logger& WorkItem::get_logger()
{
    return m_network->get_logger();
}

    void WorkItem::submit(const recv_callback_func_t& cb)
    {
        m_callback = cb;
        m_work_type = Type::RECV;
        m_network->re_submit(*this);
    }

    void WorkItem::submit(const send_callback_func_t& cb)
    {
        m_callback = cb;
        m_work_type = Type::SEND;
        m_network->re_submit(*this);
    }

    void WorkItem::submit(const accept_callback_func_t& cb)
    {
        m_callback = cb;
        m_work_type = Type::ACCEPT;
        m_network->re_submit(*this);
    }

    void WorkItem::submit(const IPAddress& target, const connect_callback_func_t& cb)
    {
        LOG_INFO(get_logger(), "connecting to %s\n",
            target.to_human_readable_ip_string().c_str())

        m_connect_sock_len = target.size_sockaddr();
        assert(sizeof(m_buffer_for_uring) >= m_connect_sock_len);
        memcpy(&m_buffer_for_uring, target.data_sockaddr(), m_connect_sock_len);

        m_callback  = cb;
        m_work_type = Type::CONNECT;
        m_network->re_submit(*this);
    }

} // namespace network