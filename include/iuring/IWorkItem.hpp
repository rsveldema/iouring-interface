#pragma once

/**
 * @file IWorkItem.hpp
 * @brief Defines the IWorkItem interface for work items in asynchronous I/O operations.
 * 
 * This interface provides the basic functionalities for different types of
 * work items, such as sending, receiving, connecting, etc.
 */

#include <functional>

#include "ReceivedMessage.hpp"
#include "SendPacket.hpp"
#include "CompletionCallbacks.hpp"

namespace iuring
{
class IWorkItem
{
public:
    enum class Type
    {
        UNKNOWN,
        ACCEPT,
        SEND,
        RECV,
        CONNECT,
        CLOSE
    };

    virtual ~IWorkItem() {}

    Type get_type() const
    {
        return m_work_type;
    }

    virtual SendPacket& get_send_packet() = 0;
    virtual void submit(const send_callback_func_t& cb) = 0;

protected:
    Type m_work_type = Type::UNKNOWN;
};
} // namespace iuring
