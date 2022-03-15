#include <iostream>

#include <boost/asio/experimental/awaitable_operators.hpp>

#include "conet/result.h"
#include "conet/protobuf_tcp_client.h"
#include "conet/pack_coder.h"
#include "conet/pack_tcp_reader.h"

#include "hello.pb.h"

using ProtobufTcpClient = conet::basic_ProtobufTcpClient<conet::PackCoder, conet::PackTcpReader>;

boost::asio::awaitable<conet::result<void>> g(ProtobufTcpClient &protobuf_tcp_client)
{
    {
        // proto req declare
        proto::HelloREQ req;
        req.set_value("hello");

        RESULT_CO_AUTO(rsp, co_await protobuf_tcp_client.send<proto::HelloRSP>(req));
        LOG(INFO) << rsp->ShortDebugString();
    }

    co_return RESULT_SUCCESS;
}

boost::asio::awaitable<conet::result<void>> f(boost::asio::io_context &io_context)
{
    ProtobufTcpClient protobuf_tcp_client(io_context);
    RESULT_CO_CHECK(co_await protobuf_tcp_client.connect("127.0.0.1:2000"));

    using namespace boost::asio::experimental::awaitable_operators;
    auto [r1, r2] = co_await ( protobuf_tcp_client.run() && g(protobuf_tcp_client) );

    RESULT_CO_CHECK(r1);
    RESULT_CO_CHECK(r2);

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
