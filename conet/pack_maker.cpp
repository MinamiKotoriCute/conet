#include "pack_maker.h"

#include <cstring> // memcpy

// for htonl
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

namespace conet {

void PackMacker::add(const void* p, size_t size)
{
	infos.emplace_back(Info{ p, size });
}

std::vector<char> PackMacker::make() const
{
	auto data_size = size();
	std::vector<char> buffer(4 + data_size);

	data_size = htonl(data_size);
	*(int32_t*)&buffer[0] = data_size;

	void* p = &buffer[4];
	for (const auto& info : infos) {
		std::memcpy(p, info.p, info.size);
		(char*&)p += info.size;
	}
	return buffer;
}

size_t PackMacker::size() const
{
	size_t total_size = 0;
	for (const auto& info : infos) {
		total_size += info.size;
	}
	return total_size;
}

} // namespace conet
