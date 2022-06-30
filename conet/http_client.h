#pragma once

#include <functional>
#include <optional>
#include <memory>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/leaf.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>

#include "url_parser.h"
#include "error.h"

namespace conet {

template<typename Executor = boost::asio::any_io_executor>
class HttpClient
{
public:
    HttpClient(Executor &ex) :
        executor_(ex),
        stream_(ex)
    {
    }

    boost::asio::awaitable<result<std::string>> co_get(const std::string &url)
    {
        UrlParser parser;
        RESULT_CO_CHECK(parser.parse(url), r.error_info().add_pair("url", url));

        boost::asio::ip::tcp::resolver resolver(stream_.get_executor());
        auto results = co_await resolver.async_resolve(parser.host(), parser.service(), boost::asio::use_awaitable);
        co_await stream_.async_connect(results, boost::asio::use_awaitable);

        boost::beast::http::request<boost::beast::http::string_body> req;
        boost::beast::flat_buffer rsp_buffer;
        boost::beast::http::response<boost::beast::http::string_body> rsp;
        req.method(boost::beast::http::verb::get);
        req.target(std::string(parser.path()));
        req.version(11);
        req.set(boost::beast::http::field::host, std::string(parser.host()));
        req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        co_await boost::beast::http::async_write(stream_, req, boost::asio::use_awaitable);
        co_await boost::beast::http::async_read(stream_, rsp_buffer, rsp, boost::asio::use_awaitable);

        boost::system::error_code ec;
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec && ec != boost::beast::errc::not_connected)
        {
            boost::system::error_code error_code;
            static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
            error_code.assign(ec.value(), ec.category(), &loc);
            co_return error_code;
        }

        co_return rsp.body();
    }

    boost::asio::awaitable<result<std::string>> co_get(const std::string &url, std::chrono::nanoseconds timeout)
    {
        using namespace boost::asio::experimental::awaitable_operators;

        boost::asio::steady_timer timer(stream_.get_executor());
        timer.expires_after(timeout);
        boost::system::error_code ec;
        std::variant<std::monostate, result<std::string>> result = co_await (timer.async_wait(boost::asio::redirect_error(boost::asio::use_awaitable, ec)) || co_get(url));

        if (result.index() == 0)
        {
            if (ec)
            {
                boost::system::error_code error_code;
                static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
                error_code.assign(ec.value(), ec.category(), &loc);
                co_return error_code;
            }
            
            boost::system::error_code error_code;
            static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
            error_code.assign(error::timeout, error::network_category(), &loc);
            co_return error_code;
        }

        co_return std::get<1>(std::move(result));
    }

private:
    Executor& executor_;
    boost::beast::tcp_stream stream_;
};

} // namespace conet
