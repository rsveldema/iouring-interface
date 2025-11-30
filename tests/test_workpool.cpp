#include <gtest/gtest.h>

#include "mocks.hpp"
#include <WorkPool.hpp>

using testing::_;


namespace Tests
{
class TestWorkPool : public testing::Test
{
public:
    Logger logger{ true, true, LogOutput::CONSOLE };
    network::WorkPool wp{ logger };

    std::shared_ptr<network::mocks::Socket> socket =
        std::make_shared<network::mocks::Socket>(network::SocketType::IPV4_TCP,
            network::SocketPortID::LOCAL_WEB_PORT, logger,
            network::SocketKind::CLIENT_SOCKET, 42);

    std::shared_ptr<network::mocks::IOUring> io =
        std::make_shared<network::mocks::IOUring>();
    bool seen_submit = false;
    bool seen_callback = false;
};

TEST_F(TestWorkPool, test_wp_accept)
{
    ASSERT_EQ(socket->get_fd(), 42);

    EXPECT_CALL(*io, submit(_)).WillOnce([this](network::IWorkItem& item) {
        ASSERT_EQ(item.get_type(), network::IWorkItem::Type::ACCEPT);
        seen_submit = true;

        auto* k = dynamic_cast<network::WorkItem *>(&item);
        ASSERT_NE(k, nullptr);

        network::AcceptResult ret;
        ret.m_new_fd = 12345;
        k->call_accept_callback(ret);
    });

    auto item = wp.alloc_accept_work_item(
        socket, io,
        [this](const network::AcceptResult& result) {
            ASSERT_EQ(result.m_new_fd, 12345);
            seen_callback = true;
        },
        "test-accept");

    ASSERT_NE(item, nullptr);
    ASSERT_TRUE(seen_submit);
    ASSERT_TRUE(seen_callback);
    ASSERT_EQ(item->get_type(), network::IWorkItem::Type::ACCEPT);
    ASSERT_EQ(socket, item->get_socket());
    ASSERT_EQ(item->get_type_str(), std::string("accept"));

    // the item should be registered:
    ASSERT_EQ(wp.get_work_item(item->get_id()), item);
    wp.free_work_item(item->get_id());

    // after freeing it, we get nullptr back:
    ASSERT_EQ(wp.get_work_item(item->get_id()), nullptr);

    // double free causes an assert (in debug build...):
    ASSERT_EXIT(wp.free_work_item(item->get_id()),
        ::testing::KilledBySignal(SIGABRT), "work_item != nullptr");
}

TEST_F(TestWorkPool, test_wp_close)
{
    EXPECT_CALL(*io, submit(_)).WillOnce([this](network::IWorkItem& item) {
        ASSERT_EQ(item.get_type(), network::IWorkItem::Type::CLOSE);
        seen_submit = true;

        auto* k = dynamic_cast<network::WorkItem *>(&item);
        ASSERT_NE(k, nullptr);
        k->call_close_callback(12321);
    });

    auto item = wp.alloc_close_work_item(
        socket, io,
        [](const network::CloseResult& result) {
            ASSERT_EQ(result.status, 12321);
        },
        "test-close");

    ASSERT_NE(item, nullptr);
    ASSERT_TRUE(seen_submit);
    ASSERT_EQ(socket, item->get_socket());
    ASSERT_EQ(item->get_type_str(), std::string("close"));
}

TEST_F(TestWorkPool, test_wp_connect)
{
    EXPECT_CALL(*io, submit(_)).WillOnce([this](network::IWorkItem& item) {
        ASSERT_EQ(item.get_type(), network::IWorkItem::Type::CONNECT);
        seen_submit = true;

        auto* k = dynamic_cast<network::WorkItem *>(&item);
        ASSERT_NE(k, nullptr);
        network::ConnectResult ret;
        ret.status = 12321;
        ret.m_address = network::IPAddress(
            network::IPAddress::string_to_ipv4_address("9.8.7.6", logger),
            network::SocketPortID::ENCRYPTED_WEB_PORT
        );
        k->call_connect_callback(ret);
    });

    auto item = wp.alloc_connect_work_item(
        network::IPAddress(  network::IPAddress::string_to_ipv4_address("1.2.3.4", logger),
                network::SocketPortID::ENCRYPTED_WEB_PORT ),
        socket, io,
        [](const network::ConnectResult& result) {
            ASSERT_EQ(result.status, 12321);
            ASSERT_EQ(result.m_address.to_human_readable_ip_string(), "9.8.7.6");
        },
        "test-conn");

    ASSERT_NE(item, nullptr);
    ASSERT_TRUE(seen_submit);
    ASSERT_EQ(socket, item->get_socket());
    ASSERT_EQ(item->get_type_str(), std::string("connect"));
}

} // namespace Tests