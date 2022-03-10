#pragma once

#include <vector>
#include <boost/asio.hpp>

#include "error.h"
#include "result.h"

namespace conet {

class TcpClient
{
public:
    TcpClient(boost::asio::io_context& io_context);
    TcpClient(boost::asio::any_io_executor executor);
    TcpClient(boost::asio::ip::tcp::socket socket);

    TcpClient(const TcpClient&) = delete;
    TcpClient(TcpClient &&) = default;
    TcpClient& operator=(const TcpClient &) = delete;
    TcpClient& operator=(TcpClient &&) = default;

    boost::asio::awaitable<result<void>> connect(const std::string &url);
    result<void> disconnect();
    boost::asio::awaitable<result<void>> read(std::vector<char> &read_buffer);
    boost::asio::awaitable<result<void>> write(std::vector<char> &&data);
    boost::asio::any_io_executor get_executor();

private:
	boost::asio::ip::tcp::socket socket_;
};

} // namespace conet
