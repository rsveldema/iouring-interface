#pragma once

/**
 * @file IWorkItem.hpp
 * @brief Defines the IWorkItem interface for work items in asynchronous I/O
 * operations.
 *
 * This interface provides the basic functionalities for different types of
 * work items, such as sending, receiving, connecting, etc.
 */

#include <functional>

#include "CompletionCallbacks.hpp"
#include "NetworkProtocols.hpp"
#include "ReceivedMessage.hpp"
#include "SendPacket.hpp"

namespace iuring
{
struct DatagramSendParameters
{
    IPAddress destination_address;
    dscp_t dscp;
    timetolive_t ttl;
};


class IWorkItem
{
public:
    enum class Type
    {
        UNKNOWN,
        ACCEPT,
        SEND_STREAM_DATA,
        SEND_WORKPACKET,
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

    /** @brief Submits the work item for processing.
     */
    virtual void submit_stream_data(const send_callback_func_t& cb) = 0;


    /** @brief Submits the work item for processing.
     */
    virtual void submit_packet(const DatagramSendParameters& params,
        const send_callback_func_t& cb) = 0;


    /** @brief Get the socket associated with this work item.
     * 
     * Typically not needed. Can be used if you have multiple sockets and want to
     * know which one this work item is for.
     */
    virtual std::shared_ptr<ISocket> get_socket() const = 0;

protected:
    Type m_work_type = Type::UNKNOWN;
};
} // namespace iuring
