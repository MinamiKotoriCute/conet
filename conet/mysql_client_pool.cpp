#include "mysql_client_pool.h"

#include <vector>

#include "error.h"

namespace conet {

MysqlClientPool::MysqlClientPool(boost::asio::io_context &io_context) :
    strand_(boost::asio::make_strand(io_context))
{

}

boost::asio::awaitable<result<void>> conet::MysqlClientPool::init(
        const std::string& host,
        unsigned int port,
        const std::string& user,
        const std::string& password,
        const std::string& database,
        int init_number,
        int limit_max_number)
{
    host_ = host;
    port_ = port;
    user_ = user;
    password_ = password;
    database_ = database;
    limit_max_number_ = limit_max_number;

    co_await boost::asio::post(strand_, boost::asio::use_awaitable);
    for (int i=0; i<init_number; ++i)
    {
        RESULT_CO_AUTO(mysql_client, co_await create());
        mysql_client_group_.push_back(std::move(mysql_client));
    }

    co_return RESULT_SUCCESS;
}

boost::asio::awaitable<result<std::shared_ptr<MysqlClient>>> MysqlClientPool::get()
{
    co_await boost::asio::post(strand_, boost::asio::use_awaitable);

    std::shared_ptr<MysqlClient> p(new MysqlClient, [this] (MysqlClient *raw_p)
    {
        boost::asio::dispatch(strand_, 
            [raw_p, this] ()
            {
                mysql_client_group_.push_back(std::move(*raw_p));
                delete raw_p;
            });
    });

    if (!mysql_client_group_.empty())
    {
        auto mysql_client = std::move(mysql_client_group_.front());
        mysql_client_group_.pop_front();

        *p = std::move(mysql_client);
        co_return p;
    }

    if (current_number_ < limit_max_number_)
    {
        RESULT_CO_TRY(*p, co_await create());
        co_return p;
    }

    boost::system::error_code error_code;
    static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
    error_code.assign(error::internal_error, error::conet_category(), &loc);
    co_return error_code;
}

boost::asio::awaitable<result<MysqlClient>> MysqlClientPool::create()
{
    MysqlClient mysql_client;
    RESULT_CO_CHECK(co_await mysql_client.connect(host_, port_, user_, password_, database_));
    ++current_number_;
    co_return mysql_client;
}

} // namespace conet
