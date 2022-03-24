#pragma once

#include <memory>
#include <mutex>
#include <list>

#include <boost/asio.hpp>
#include "mysql_client.h"

namespace conet {

class MysqlClientPool
{
public:
    MysqlClientPool(boost::asio::io_context &io_context);

    boost::asio::awaitable<result<void>> init(
        const std::string& host,
        unsigned int port,
        const std::string& user,
        const std::string& password,
        const std::string& database,
        int init_number,
        int limit_max_number);

    boost::asio::awaitable<result<std::shared_ptr<MysqlClient>>> get();

private:
    boost::asio::awaitable<result<MysqlClient>> create();

    boost::asio::strand<boost::asio::any_io_executor> strand_;
    int current_number_;
    int limit_max_number_;
    std::list<MysqlClient> mysql_client_group_;
    std::string host_;
    unsigned int port_;
    std::string user_;
    std::string password_;
    std::string database_;
};

} // namespace conet
