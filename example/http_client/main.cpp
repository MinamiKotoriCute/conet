#include <iostream>

#include "conet/http_client.h"

boost::asio::awaitable<conet::result<void>> f(boost::asio::io_context &io_context)
{
    conet::HttpClient http_client(io_context);
    RESULT_CO_AUTO(rsp, co_await http_client.co_get("192.168.1.216:8004"));
    
    LOG(INFO) << rsp;

    co_return RESULT_SUCCESS;
}

// ncat -l 2000 -k -c 'xargs -n1 echo'
int main(int argc, char *argv[])
{
    boost::asio::io_context io_context;

    boost::asio::co_spawn(io_context,
        f(io_context),
        [](std::exception_ptr e, conet::result<void> result)
        {
            if (result.has_error())
            {
                LOG(INFO) << result;
            }
        });

    io_context.run();

    return 0;
}