#pragma once

#include <memory>

#include <iuring/ISocket.hpp>

namespace iuring
{

/** A factory class for creating ISocket instances.
 *
 * - In the unit tests this is overriden with an instance that returns mocks.
 * - normally, this is overriden by iuring::SocketFactory.
 */
class ISocketFactory
{
public:
    virtual std::shared_ptr<ISocket> create_impl(SocketType type,
        SocketPortID port, logging::ILogger& logger, SocketKind kind) = 0;

    virtual std::shared_ptr<ISocket> create_impl(
        logging::ILogger& logger, const AcceptResult& res) = 0;
};
} // namespace iuring