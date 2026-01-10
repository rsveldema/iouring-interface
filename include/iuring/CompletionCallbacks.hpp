#pragma once

#include <functional>
#include <string>
#include <expected>

/**
 * @file CompletionCallbacks.hpp
 * @brief Defines callback function types for network operations.
 */

namespace iuring
{
enum class [[nodiscard]] ReceivePostAction{ NONE, RE_SUBMIT };

struct AcceptResult
{
    int m_new_fd;
    IPAddress m_address;
};

class SendResult
{
public:
    explicit SendResult(int s) : m_status(s) {}

    std::expected<int, error::Error> to_expected() const
    {
        if (m_status >= 0)
        {
            return std::expected<int, error::Error>(m_status);
        }
        else
        {
            return std::unexpected<error::Error>(
                error::errno_to_error(-m_status));
        }
    }

private:
    int m_status;
};

class ConnectResult
{
public:
    ConnectResult(int status, const IPAddress& addr)
        : m_status(status)
        , m_address(addr)
    {
    }

    std::expected<IPAddress, error::Error> to_expected() const
    {
        if (m_status >= 0)
        {
            return m_address;
        }
        else
        {
            return std::unexpected<error::Error>(
                error::errno_to_error(-m_status));
        }
    }
private:
    int m_status;
    IPAddress m_address;
};

class CloseResult
{
public:
    explicit CloseResult(int s) : m_status(s) {}

    std::expected<int, error::Error> to_expected() const
    {
        if (m_status >= 0)
        {
            return std::expected<int, error::Error>(m_status);
        }
        else
        {
            return std::unexpected<error::Error>(
                error::errno_to_error(-m_status));
        }
    }

private:
    int m_status;
};

class ReceivedMessage;

using recv_callback_func_t =
    std::function<ReceivePostAction(const ReceivedMessage& msg)>;

using send_callback_func_t = std::function<void(const SendResult&)>;

using accept_callback_func_t =
    std::function<void(const AcceptResult& new_conn)>;

using connect_callback_func_t =
    std::function<void(const ConnectResult& result)>;

using close_callback_func_t = std::function<void(const CloseResult& result)>;

} // namespace iuring