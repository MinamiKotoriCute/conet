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
    TcpServer(boost::asio::strand<boost::asio::io_context::executor_type>& strand);

    result<void> listen(const std::string &ip, short port);
    result<void> close();
    boost::asio::awaitable<AcceptResultType> accept();

private:
    boost::asio::ip::tcp::acceptor acceptor_;
};

} // namespace conet
