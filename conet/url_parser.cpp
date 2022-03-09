#include "url_parser.h"

#include <regex>
#include <iostream>

#include <glog/logging.h>

#include "error.h"

namespace conet {

result<void> UrlParser::parse(const std::string &url)
{
    std::regex regex(R"##((?:(\w+):\/\/)?([^:\/]+):?(\d+)?(\/.*)?)##");
    std::smatch smatch;
    if (!std::regex_match(url, smatch, regex))
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::parameter_error, error::conet_category(), &loc);
        return error_code;
    }

    if (smatch.size() != 5)
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(error::parameter_error, error::conet_category(), &loc);
        return error_code;
    }

    protocol_ = smatch[1].str();
    host_ = smatch[2].str();
    port_ = smatch[3].str();
    path_ = smatch[4].str();

    return {};
}

std::string UrlParser::path() const noexcept
{
    if (!path_.empty())
        return path_;
    return "/";
}

std::string UrlParser::service() const noexcept
{
    if (!port_.empty())
        return port_;
    if (!protocol_.empty())
        return protocol_;
    return "80";
}

} // namespace conet
