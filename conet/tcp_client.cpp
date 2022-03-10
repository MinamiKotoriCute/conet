#include "tcp_client.h"

#include <regex>
#include <glog/logging.h>
#include "url_parser.h"

namespace conet {

TcpClient::TcpClient(boost::asio::io_context& io_context) :
    socket_(io_context)
{

}

TcpClient::TcpClient(boost::asio::any_io_executor executor) :
    socket_(executor)
{

}

TcpClient::TcpClient(boost::asio::ip::tcp::socket socket) :
    socket_(std::move(socket))
{

}

boost::asio::awaitable<result<void>> TcpClient::connect(const std::string &url)
{
    UrlParser url_parser;
    RESULT_CO_CHECK(url_parser.parse(url));

    boost::asio::ip::tcp::resolver::results_type ret_results;
	boost::asio::ip::tcp::resolver::query query(url_parser.host(), url_parser.service(), boost::asio::ip::resolver_query_base::numeric_service);

    boost::system::error_code ec;
	boost::asio::ip::tcp::resolver resolver(socket_.get_executor());
	boost::asio::ip::tcp::resolver::results_type results = co_await resolver.async_resolve(query, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        co_return error_code;
    }

    boost::asio::ip::tcp::endpoint endpoint = co_await boost::asio::async_connect(socket_, results, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        co_return error_code;
    }

    co_return RESULT_SUCCESS;
}

result<void> TcpClient::disconnect()
{
    if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec)
        {
            boost::system::error_code error_code;
            static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
            error_code.assign(ec.value(), ec.category(), &loc);
            return error_code;
		}

        socket_.close(ec);
        if (ec)
        {
            boost::system::error_code error_code;
            static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
            error_code.assign(ec.value(), ec.category(), &loc);
            return error_code;
		}
    }

    return {};
}

boost::asio::awaitable<result<void>> TcpClient::write(std::vector<char> &&data)
{
    boost::system::error_code ec;
    std::size_t bytes_transferred = co_await boost::asio::async_write(socket_, boost::asio::buffer(data), boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        co_return error_code;
    }

    if (bytes_transferred != data.size())
    {
        // Assuming not coming here
        LOG(ERROR) << "write not finished. bytes_transferred:" << bytes_transferred << " write_buffer.size():" << data.size();

        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::third_party_error, error::conet_category(), &loc);
        co_return error_code;
    }

    co_return RESULT_SUCCESS;
}

boost::asio::awaitable<result<void>> TcpClient::read(std::vector<char> &read_buffer)
{
    boost::system::error_code ec;
    std::size_t bytes_transferred = co_await boost::asio::async_read(socket_, boost::asio::buffer(read_buffer), boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec == boost::asio::error::eof)
    {
        // tcp closed
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::connection_closed, error::network_category(), &loc);
        co_return error_code;
    }
    else if (ec == boost::asio::error::bad_descriptor)
    {
        // tcp close by custom
        // call disconnect() will trigger this
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::connection_closed, error::network_category(), &loc);
        co_return error_code;
    }
    else if (ec == boost::asio::error::operation_aborted)
    {
        // tcp close by custom
        // call disconnect() will trigger this
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::connection_closed, error::network_category(), &loc);
        co_return error_code;
    }
    else if (ec == boost::asio::error::connection_reset)
    {
        // tcp close by remote
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::connection_closed, error::network_category(), &loc);
        co_return error_code;
    }
    else if (ec)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(ec.value(), ec.category(), &loc);
        co_return error_code;
    }

    co_return RESULT_SUCCESS;
}

boost::asio::any_io_executor TcpClient::get_executor()
{
    return socket_.get_executor();
}

} // namespace conet
