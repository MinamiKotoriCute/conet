#include "pack_parser.h"

// for htonl
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

namespace conet {

PacketParser::PacketParser(const std::vector<char>& buffer) : buffer_(buffer), offset_(0)
{
}

bool PacketParser::get_uint16(uint16_t& i)
{
	if (!get_direct(i))
		return false;

	i = ntohs(i);
	return true;
}

bool PacketParser::get_int32(int32_t& i)
{
	if (!get_direct(i))
		return false;

	i = ntohl(i);
	return true;
}

bool PacketParser::get_string(std::string& s, size_t size)
{
	if (offset_ + size >= buffer_.size())
		return false;

	s = std::string(&buffer_[offset_], size);
	offset_ += size;
	return true;
}

const void* PacketParser::current_point() const
{
	return &buffer_[offset_];
}

size_t PacketParser::remaining_size() const
{
	return buffer_.size() - offset_;
}

} // namespace conet
