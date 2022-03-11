#pragma once

#include <string>
#include <boost/asio.hpp>

#include "error.h"
#include "result.h"
#include "tcp_client.h"

namespace conet {

class TcpServer
{
public:
    using AcceptResultType = result<TcpClient>;

    TcpServer(boost::asio::io_context& io_context);
    TcpServer(boost::asio::any_io_executor executor);

    TcpServer(const TcpServer&) = delete;
    TcpServer(TcpServer &&) = default;
    TcpServer& operator=(const TcpServer &) = delete;
    TcpServer& operator=(TcpServer &&) = default;

    result<void> listen(const std::string &ip, short port);
    result<void> close();
    boost::asio::awaitable<AcceptResultType> accept();

private:
    boost::asio::ip::tcp::acceptor acceptor_;
};

} // namespace conet
