#pragma once

// #define USE_PLAIN_SOCKETS 1

#include <arpa/inet.h>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <liburing.h>
#include <sys/mman.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <stack>
#include <string>
#include <vector>

#include <iuring/ILogger.hpp>
#include <iuring/UringDefs.hpp>
#include "WorkItem.hpp"

namespace iuring
{
class IOUringInterface;

class WorkPool
{
public:
    explicit WorkPool(logging::ILogger& logger)
        : m_logger(logger)
    {
    }

    logging::ILogger& get_logger()
    {
        return m_logger;
    }

    std::shared_ptr<WorkItem> alloc_send_work_item(
        const std::shared_ptr<ISocket>& socket,
        const std::shared_ptr<IOUringInterface>& network,
        const char* descr);

    std::shared_ptr<WorkItem> alloc_recv_work_item(
        const std::shared_ptr<ISocket>& socket,
        const std::shared_ptr<IOUringInterface>& network,
        const recv_callback_func_t& callback, const char* descr);

    std::shared_ptr<WorkItem> alloc_accept_work_item(
        const std::shared_ptr<ISocket>& socket,
        const std::shared_ptr<IOUringInterface>& network,
        const accept_callback_func_t& callback, const char* descr);

    std::shared_ptr<WorkItem> alloc_connect_work_item(
        const IPAddress& target,
        const std::shared_ptr<ISocket>& socket,
        const std::shared_ptr<IOUringInterface>& network,
        const connect_callback_func_t& callback, const char* descr);

    std::shared_ptr<WorkItem> alloc_close_work_item(
        const std::shared_ptr<ISocket>& socket,
        const std::shared_ptr<iuring::IOUringInterface>& network,
        const close_callback_func_t& callback, const char* descr);


    std::shared_ptr<WorkItem> get_work_item(work_item_id_t id);
    void free_work_item(work_item_id_t id);

    // only for testing:
    const std::vector<std::shared_ptr<WorkItem>>& get_work_item_list() const
    {
        return m_work_items;
    }


private:
    logging::ILogger& m_logger;
    std::recursive_mutex m_mutex;
    /** because we pass indices into this array into io_uring, we are not
     * allowed to shrink this using erase(), we therefore append only and have
     * m_free_ids to track free indices.
     */
    std::vector<std::shared_ptr<WorkItem>> m_work_items;
    std::stack<work_item_id_t> m_free_ids;


    std::shared_ptr<WorkItem> internal_alloc_work_item(
        const std::shared_ptr<ISocket>& socket,
        const std::shared_ptr<IOUringInterface>& network, const char* descr);
};


} // namespace iuring