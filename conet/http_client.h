#pragma once

#include <functional>
#include <optional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/leaf.hpp>

#include "url_parser.h"

namespace conet {

template<typename Executor = boost::asio::any_io_executor>
class HttpClient
{
public:
    HttpClient(Executor &ex) :
        executor_(ex),
        resolver_(ex),
        stream_(ex)
    {
    }

    boost::asio::awaitable<result<std::string>> co_get(const std::string &url)
    {
        RESULT_CO_CHECK(parser_.parse(url));

        auto results = co_await resolver_.async_resolve(parser_.host(), parser_.service(), boost::asio::use_awaitable);
        co_await stream_.async_connect(results, boost::asio::use_awaitable);

        req_.method(boost::beast::http::verb::get);
        req_.target(std::string(parser_.path()));
        req_.version(11);
        req_.set(boost::beast::http::field::host, std::string(parser_.host()));
        req_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        co_await boost::beast::http::async_write(stream_, req_, boost::asio::use_awaitable);
        co_await boost::beast::http::async_read(stream_, rsp_buffer_, rsp_, boost::asio::use_awaitable);

        boost::system::error_code ec;
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec && ec != boost::beast::errc::not_connected)
        {
            boost::system::error_code error_code;
            static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
            error_code.assign(ec.value(), ec.category(), &loc);
            co_return error_code;
        }

        co_return rsp_.body();
    }

private:
    Executor& executor_;
    UrlParser parser_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::tcp_stream stream_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    boost::beast::flat_buffer rsp_buffer_;
    boost::beast::http::response<boost::beast::http::string_body> rsp_;
};

} // namespace conet
