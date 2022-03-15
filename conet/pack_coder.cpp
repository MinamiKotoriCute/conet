#include "pack_coder.h"

#include <string>

// for htonl
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include "error.h"
#include "error_info.h"
#include "pack_maker.h"
#include "pack_parser.h"

namespace conet {

std::vector<char> PackCoder::encode(const google::protobuf::Message& message) const
{
	std::string protobuf_name = message.GetDescriptor()->full_name();
	uint16_t protobuf_name_length = protobuf_name.size();
	std::string data = message.SerializeAsString();

	protobuf_name_length = htons(protobuf_name_length);
	int32_t packet_version = htonl(packet_version_);

	PackMacker pack_maker;
	pack_maker.add(&protobuf_name_length, sizeof(protobuf_name_length));
	pack_maker.add(protobuf_name.c_str(), protobuf_name.size());
	pack_maker.add(&packet_version, sizeof(packet_version));
	pack_maker.add(data.c_str(), data.size());

	return pack_maker.make();
}

result<std::shared_ptr<google::protobuf::Message>> PackCoder::decode(std::vector<char> &&binary)
{
	PacketParser parser(binary);

	uint16_t protobuf_name_length;
	if (!parser.get_uint16(protobuf_name_length))
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::parameter_error, error::conet_category(), &loc);
        return error_code;
    }

	std::string protobuf_name;
	if (!parser.get_string(protobuf_name, protobuf_name_length))
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::parameter_error, error::conet_category(), &loc);
        return error_code;
    }

	int32_t packet_version;
	if (!parser.get_int32(packet_version))
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::parameter_error, error::conet_category(), &loc);
        return error_code;
    }
	packet_version_ = packet_version;

	const void* data = nullptr;
	size_t size = parser.remaining_size();
	if (size > 0)
		data = parser.current_point();

	//uint16_t protobuf_name_length = *(uint16_t*)&buffer[0];
	//std::string protobuf_name(&buffer[sizeof(protobuf_name_length)], (size_t)protobuf_name_length);
	//int32_t roomid = *(int32_t*)&buffer[sizeof(protobuf_name_length) + protobuf_name_length];
	//const void* data = &buffer[sizeof(protobuf_name_length) + protobuf_name_length + sizeof(roomid)];
	//size_t size = buffer.size() - (sizeof(protobuf_name_length) + protobuf_name_length + sizeof(roomid));
		

	const google::protobuf::Descriptor* descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(protobuf_name);
	if (nullptr == descriptor)
	{
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::internal_error, error::conet_category(), &loc);

        ErrorInfo error_info(error_code);
        error_info.add_pair("protobuf_name", protobuf_name);
		
        return error_info;
	}

	const google::protobuf::Message* prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
	if (nullptr == prototype)
	{
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::third_party_error, error::conet_category(), &loc);
        return error_code;
	}

	auto message = std::shared_ptr<google::protobuf::Message>(prototype->New());
	if (!message->ParseFromArray(data, size))
	{
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::third_party_error, error::conet_category(), &loc);
        return error_code;
	}

	return {std::move(message)};
}

} // namespace conet
