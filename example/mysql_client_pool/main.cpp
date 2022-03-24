#include <iostream>

#include <boost/asio/experimental/awaitable_operators.hpp>

#include "conet/result.h"
#include "conet/mysql_client.h"
#include "conet/mysql_client_pool.h"

boost::asio::awaitable<conet::result<int>> g(boost::asio::io_context &io_context, conet::MysqlClientPool &mysql_client_pool)
{
    RESULT_CO_AUTO(a, co_await mysql_client_pool.get());
    conet::MysqlClient& mysql_client = *a;

    RESULT_CO_CHECK(co_await mysql_client.query("create table if not exists `test` (`id` bigint(20) AUTO_INCREMENT, `name` varchar(20), PRIMARY KEY (`id`))"));
    RESULT_CO_CHECK(co_await mysql_client.query("insert into `test`(`name`) VALUES ('hello')"));
    RESULT_CO_AUTO(r, co_await mysql_client.query("select * from `test`"));

    co_return r.size();
}

boost::asio::awaitable<conet::result<void>> f(boost::asio::io_context &io_context)
{
    conet::MysqlClientPool mysql_client_pool(io_context);
    // RESULT_CO_CHECK(co_await mysql_client_pool.init( ... ));

    using namespace boost::asio::experimental::awaitable_operators;
    auto&& [r1, r2] = co_await (g(io_context, mysql_client_pool) && g(io_context, mysql_client_pool));

    RESULT_CO_AUTO(num1, r1);
    RESULT_CO_AUTO(num2, r2);

    if (num1 + num2 != 3)
    {
        LOG(WARNING) << "error " << num1 << " " << num2;
    }
    
    RESULT_CO_AUTO(a, co_await mysql_client_pool.get());
    conet::MysqlClient& mysql_client = *a;
    RESULT_CO_CHECK(co_await mysql_client.query("drop table if exists `test`"));

    co_return RESULT_SUCCESS;
}

int main(int argc, char *argv[])
{
    boost::asio::io_context io_context;

    boost::asio::co_spawn(io_context,
        f(io_context),
        [](std::exception_ptr e, conet::result<void> result)
        {
            if (result.has_error())
            {
                LOG(INFO) << result.error_info();
            }
        });

    io_context.run();

    return 0;
}
