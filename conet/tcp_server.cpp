#include "tcp_server.h"

#include <glog/logging.h>
#include "url_parser.h"

namespace conet {

TcpServer::TcpServer(boost::asio::io_context& io_context) :
    acceptor_(io_context)
{

}

TcpServer::TcpServer(boost::asio::strand<boost::asio::io_context::executor_type>& strand) :
    acceptor_(strand)
{

}

result<void> TcpServer::listen(const std::string &ip, short port)
{
    if (acceptor_.is_open())
    {
        return {};
    }

    boost::system::error_code ec;
    boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(ip, ec);
    if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        return error_code;
    }
    auto endpoint = boost::asio::ip::tcp::endpoint(ip_address, port);
    
    acceptor_.open(endpoint.protocol(), ec);
    if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        return error_code;
    }

    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));

    acceptor_.bind(endpoint, ec);
    if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        return error_code;
    }

    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        return error_code;
    }
    
    return RESULT_SUCCESS;
}

result<void> TcpServer::close()
{
    boost::system::error_code ec;
    acceptor_.close(ec);
    if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        return error_code;
    }
    
    return RESULT_SUCCESS;
}

boost::asio::awaitable<TcpServer::AcceptResultType> TcpServer::accept()
{
    boost::system::error_code ec;
    boost::asio::ip::tcp::socket socket = co_await acceptor_.async_accept(boost::asio::redirect_error(boost::asio::use_awaitable, ec));

    if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        co_return error_code;
    }

    co_return std::move(socket);
}

} // conet
