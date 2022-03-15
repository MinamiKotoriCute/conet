#pragma once

#include <vector>
#include <memory>

#include <google/protobuf/message.h>
#include "result.h"

namespace conet {

class PackCoder
{
public:
	std::vector<char> encode(const google::protobuf::Message& message) const;
	result<std::shared_ptr<google::protobuf::Message>> decode(std::vector<char> &&binary);

	int32_t packet_version_;
};

} // namespace conet
