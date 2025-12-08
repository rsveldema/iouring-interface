#pragma once

#include <memory>

#include <iuring/ISocketFactory.hpp>

namespace iuring
{

    /** creates real impls */
class SocketFactoryImpl : public ISocketFactory
{
public:
    std::shared_ptr<ISocket> create_impl(SocketType type, SocketPortID port,
        logging::ILogger& logger, SocketKind kind) override
    {
        return ISocket::create_impl(type, port, logger, kind);
    }

    std::shared_ptr<ISocket> create_impl(
        logging::ILogger& logger, const AcceptResult& res) override
    {
        return ISocket::create_impl(logger, res);
    }
};
} // namespace iuring