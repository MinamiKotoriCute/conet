#include <iostream>

#include "conet/tcp_server.h"
#include "conet/tcp_client.h"

boost::asio::awaitable<conet::result<void>> f(boost::asio::io_context &io_context)
{
    conet::TcpServer tcp_server(io_context);
    RESULT_CO_CHECK(tcp_server.listen("0.0.0.0", 51000));

    RESULT_CO_AUTO(client, co_await tcp_server.accept());

    std::vector<char> buffer;
    buffer.resize(5);
    RESULT_CO_CHECK(co_await client.read(buffer));

    std::stringstream ss;
    for (const auto &c : buffer)
    {
        ss << c;
    }
    LOG(INFO) << "receive:" << ss.str();

    RESULT_CO_CHECK(co_await client.write(std::move(buffer)));
}

// nc 127.0.0.1 51000
int main(int argc, char *argv[])
{
    boost::asio::io_context io_context;

    boost::asio::co_spawn(io_context,
        f(io_context),
        [](std::exception_ptr e, conet::result<void> result)
        {
            if (result.has_error())
            {
                LOG(INFO) << "unique_error_id:" << result.unique_error_id() << " error_message:" << result.error_message() << "\n";
            }
        });

    io_context.run();

    return 0;
}
