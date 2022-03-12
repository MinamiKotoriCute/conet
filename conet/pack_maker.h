#pragma once

#include <vector>

namespace conet {

class PackMacker
{
	struct Info
	{
		const void* p;
		size_t size;
	};

public:
	void add(const void* p, size_t size);
	std::vector<char> make() const;
	size_t size() const;

private:
	std::vector<Info> infos;
};

} // namespace conet
