#pragma once

#include <vector>
#include <string>
#include <cstring>

namespace conet {

class PacketParser
{
public:
	PacketParser(const std::vector<char>& buffer);

	bool get_uint16(uint16_t& i);
	bool get_int32(int32_t& i);
	bool get_string(std::string& s, size_t size);

	const void* current_point() const;
	size_t remaining_size() const;

private:
	template<typename T>
	bool get_direct(T& t) {
		if (offset_ + sizeof(T) > buffer_.size())
			return false;

		memcpy(&t, &buffer_[offset_], sizeof(T));
		offset_ += sizeof(T);
		return true;
	}

	const std::vector<char>& buffer_;
	size_t offset_;
};

} // namespace conet
