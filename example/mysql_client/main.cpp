#include <iostream>

#include "conet/result.h"
#include "conet/mysql_client.h"

boost::asio::awaitable<conet::result<void>> f(boost::asio::io_context &io_context)
{
    conet::MysqlClient mysql_client;
    // RESULT_CO_CHECK(co_await mysql_client.connect( $info... ));

    RESULT_CO_CHECK(co_await mysql_client.query("create table if not exists `test` (`id` bigint(20) AUTO_INCREMENT, `name` varchar(20), PRIMARY KEY (`id`))"));
    RESULT_CO_CHECK(co_await mysql_client.query("insert into `test`(`name`) VALUES ('hello')"));
    RESULT_CO_CHECK(co_await mysql_client.query("insert into `test`(`name`) VALUES ('world')"));
    RESULT_CO_AUTO(r, co_await mysql_client.query("select * from `test`"));

    for (const auto &row : r)
    {
        LOG(INFO) << "id:" << row.get<int64_t>("id") << " name:" << row.get<std::string>("name");
    }

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
