#include "pack_tcp_reader.h"

namespace conet {

using PackSizeType = int32_t;

PackTcpReader::PackTcpReader(TcpClient &tcp_client) :
    tcp_client_(tcp_client)
{
}

boost::asio::awaitable<result<std::vector<char>>> PackTcpReader::read()
{
	read_buffer_.resize(sizeof(PackSizeType));
    RESULT_CO_CHECK(co_await tcp_client_.read(read_buffer_));

    PackSizeType pack_size;
    memcpy(&pack_size, &read_buffer_[0], sizeof(PackSizeType));
    pack_size = ntohl(pack_size);
    read_buffer_.resize(pack_size);
    RESULT_CO_CHECK(co_await tcp_client_.read(read_buffer_));

    co_return std::move(read_buffer_);
}

} // namespace conet
