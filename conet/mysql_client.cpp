#include "mysql_client.h"

#include "polling.h"
#include "error.h"

#define RESULT_MYSQL_CHECK_ERROR

namespace conet {

std::string get_mysql_error(MYSQL *mysql)
{
    const char *error_message = mysql_error(mysql);
    if (error_message == nullptr)
        return "";

    return error_message;
}

class initiate_mysql_connect
{
public:
    initiate_mysql_connect(MysqlClient &mysql_client) :
        mysql_client_(mysql_client)
    {
    }

    void operator()(boost::asio::detail::awaitable_handler<boost::asio::any_io_executor, result<void>> &&handler)
    {
        auto handler_ptr = std::make_shared<boost::asio::detail::awaitable_handler<boost::asio::any_io_executor, result<void>>>(std::move(handler));
        Polling::instance().add(&mysql_client_, [handler_ptr, this] () mutable -> bool // return is_finish
        {
            auto net_async_status = mysql_real_connect_nonblocking(
                mysql_client_.mysql_,
                mysql_client_.host_.c_str(),
                mysql_client_.user_.c_str(),
                mysql_client_.password_.c_str(),
                mysql_client_.database_.c_str(),
                mysql_client_.port_,
                0,
                CLIENT_MULTI_RESULTS | CLIENT_MULTI_STATEMENTS
                );

            switch (net_async_status)
            {
            case NET_ASYNC_COMPLETE:
                {
                    auto executor = boost::asio::get_associated_executor(*handler_ptr);
                    boost::asio::dispatch(executor, [handler_ptr] () mutable
                    {
                        auto&& handler = std::move(*handler_ptr.get());
                        handler(RESULT_SUCCESS);
                    });
                }
                return true;
            case NET_ASYNC_NOT_READY:
                return false;
            default:
                {
                    boost::system::error_code error_code;
                    static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
                    error_code.assign(mysql_errno(mysql_client_.mysql_), error::mysql_category(), &loc);

                    ErrorInfo error_info(error_code);
                    error_info.set_error_message(get_mysql_error(mysql_client_.mysql_));
                    error_info.add_pair("result_type", std::to_string(net_async_status));

                    auto executor = boost::asio::get_associated_executor(*handler_ptr);
                    boost::asio::dispatch(executor, [handler_ptr, error_info = std::move(error_info)] () mutable
                    {
                        auto&& handler = std::move(*handler_ptr.get());
                        handler(std::move(error_info));
                    });
                }
                return true;
            }
        });
    }

private:
    MysqlClient &mysql_client_;
};

class initiate_mysql_query
{
public:
    initiate_mysql_query(MysqlClient &mysql_client, const std::string &sql) :
        mysql_client_(mysql_client),
        sql_(sql)
    {
    }

    void operator()(boost::asio::detail::awaitable_handler<boost::asio::any_io_executor, result<void>> &&handler)
    {
        auto handler_ptr = std::make_shared<boost::asio::detail::awaitable_handler<boost::asio::any_io_executor, result<void>>>(std::move(handler));
        Polling::instance().add(&mysql_client_, [handler_ptr, this] () mutable -> bool // return is_finish
        {
            auto net_async_status = mysql_real_query_nonblocking(
                mysql_client_.mysql_,
                sql_.c_str(),
                static_cast<unsigned long>(sql_.size())
                );

            switch (net_async_status)
            {
            case NET_ASYNC_COMPLETE:
                {
                    auto executor = boost::asio::get_associated_executor(*handler_ptr);
                    boost::asio::dispatch(executor, [handler_ptr] () mutable
                    {
                        auto&& handler = std::move(*handler_ptr.get());
                        handler(RESULT_SUCCESS);
                    });
                }
                return true;
            case NET_ASYNC_NOT_READY:
                return false;
            default:
                {
                    boost::system::error_code error_code;
                    static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
                    error_code.assign(mysql_errno(mysql_client_.mysql_), error::mysql_category(), &loc);

                    ErrorInfo error_info(error_code);
                    error_info.set_error_message(get_mysql_error(mysql_client_.mysql_));
                    error_info.add_pair("result_type", std::to_string(net_async_status));

                    auto executor = boost::asio::get_associated_executor(*handler_ptr);
                    boost::asio::dispatch(executor, [handler_ptr, error_info = std::move(error_info)] () mutable
                    {
                        auto&& handler = std::move(*handler_ptr.get());
                        handler(std::move(error_info));
                    });
                }
                return true;
            }
        });
    }

private:
    MysqlClient &mysql_client_;
    const std::string &sql_;
};

class initiate_mysql_store_result
{
public:
    initiate_mysql_store_result(MysqlClient &mysql_client) :
        mysql_client_(mysql_client)
    {
    }

    void operator()(boost::asio::detail::awaitable_handler<boost::asio::any_io_executor, result<MYSQL_RES *>> &&handler)
    {
        auto handler_ptr = std::make_shared<boost::asio::detail::awaitable_handler<boost::asio::any_io_executor, result<MYSQL_RES *>>>(std::move(handler));
        Polling::instance().add(&mysql_client_, [handler_ptr, this] () mutable -> bool // return is_finish
        {
            MYSQL_RES *mysql_res;
            auto net_async_status = mysql_store_result_nonblocking(
                mysql_client_.mysql_,
                &mysql_res
                );

            switch (net_async_status)
            {
            case NET_ASYNC_COMPLETE:
                {
                    auto executor = boost::asio::get_associated_executor(*handler_ptr);
                    boost::asio::dispatch(executor, [handler_ptr, mysql_res] () mutable
                    {
                        auto&& handler = std::move(*handler_ptr.get());
                        handler(mysql_res);
                    });
                }
                return true;
            case NET_ASYNC_NOT_READY:
                return false;
            default:
                {
                    boost::system::error_code error_code;
                    static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
                    error_code.assign(mysql_errno(mysql_client_.mysql_), error::mysql_category(), &loc);

                    ErrorInfo error_info(error_code);
                    error_info.set_error_message(get_mysql_error(mysql_client_.mysql_));
                    error_info.add_pair("result_type", std::to_string(net_async_status));

                    auto executor = boost::asio::get_associated_executor(*handler_ptr);
                    boost::asio::dispatch(executor, [handler_ptr, error_info = std::move(error_info)] () mutable
                    {
                        auto&& handler = std::move(*handler_ptr.get());
                        handler(std::move(error_info));
                    });
                }
                return true;
            }
        });
    }

private:
    MysqlClient &mysql_client_;
};


class initiate_mysql_fetch_row
{
public:
    initiate_mysql_fetch_row(MysqlClient &mysql_client, MYSQL_RES *result) :
        mysql_client_(mysql_client),
        result_(result)
    {
    }

    void operator()(boost::asio::detail::awaitable_handler<boost::asio::any_io_executor, result<char **>> &&handler)
    {
        auto handler_ptr = std::make_shared<boost::asio::detail::awaitable_handler<boost::asio::any_io_executor, result<char **>>>(std::move(handler));
        Polling::instance().add(&mysql_client_, [handler_ptr, this] () mutable -> bool // return is_finish
        {
            char** row;

            auto net_async_status = mysql_fetch_row_nonblocking(
                result_,
                &row
                );

            switch (net_async_status)
            {
            case NET_ASYNC_COMPLETE:
                {
                    auto executor = boost::asio::get_associated_executor(*handler_ptr);
                    boost::asio::dispatch(executor, [handler_ptr, row] () mutable
                    {
                        auto&& handler = std::move(*handler_ptr.get());
                        handler(row);
                    });
                }
                return true;
            case NET_ASYNC_NOT_READY:
                return false;
            default:
                {
                    boost::system::error_code error_code;
                    static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
                    error_code.assign(mysql_errno(mysql_client_.mysql_), error::mysql_category(), &loc);

                    ErrorInfo error_info(error_code);
                    error_info.set_error_message(get_mysql_error(mysql_client_.mysql_));
                    error_info.add_pair("result_type", std::to_string(net_async_status));

                    auto executor = boost::asio::get_associated_executor(*handler_ptr);
                    boost::asio::dispatch(executor, [handler_ptr, error_info = std::move(error_info)] () mutable
                    {
                        auto&& handler = std::move(*handler_ptr.get());
                        handler(std::move(error_info));
                    });
                }
                return true;
            }
        });
    }

private:
    MysqlClient &mysql_client_;
    MYSQL_RES *result_;
};

void MysqlQueryResultImpl::add_result(const std::string &key, const std::string &value)
{
    datas_[key] = value;
}

boost::asio::awaitable<result<void>> MysqlClient::connect(const std::string& host, unsigned int port, const std::string& user, const std::string& password, const std::string& database)
{
    if (mysql_ == nullptr)
    {
        mysql_ = mysql_init(nullptr);
    }

    host_ = host;
    port_ = port;
    user_ = user;
    password_ = password;
    database_ = database;

    return boost::asio::async_initiate<const boost::asio::use_awaitable_t<>, void(result<void>)>(
        initiate_mysql_connect(*this),
        boost::asio::use_awaitable);
}

boost::asio::awaitable<result<void>> MysqlClient::mysql_query(const std::string& sql)
{
    return boost::asio::async_initiate<const boost::asio::use_awaitable_t<>, void(result<void>)>(
        initiate_mysql_query(*this, sql),
        boost::asio::use_awaitable);
}

boost::asio::awaitable<result<MYSQL_RES *>> MysqlClient::mysql_store_result()
{
    return boost::asio::async_initiate<const boost::asio::use_awaitable_t<>, void(result<MYSQL_RES *>)>(
        initiate_mysql_store_result(*this),
        boost::asio::use_awaitable);
}

boost::asio::awaitable<result<char **>> MysqlClient::mysql_fetch_row(MYSQL_RES *r)
{    
    return boost::asio::async_initiate<const boost::asio::use_awaitable_t<>, void(result<char **>)>(
        initiate_mysql_fetch_row(*this, r),
        boost::asio::use_awaitable);
}

} // conet
