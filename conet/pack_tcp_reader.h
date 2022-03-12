#pragma once

#include <vector>
#include <functional>

#include "tcp_client.h"

namespace conet {

class PackTcpReader
{
public:
    PackTcpReader(TcpClient &tcp_client);

    boost::asio::awaitable<result<std::vector<char>>> read();

private:
	std::vector<char> read_buffer_;
	TcpClient &tcp_client_;
};

} // namespace conet
