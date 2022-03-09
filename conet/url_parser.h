#pragma once

#include <string>

#include "result.h"

namespace conet {

class UrlParser
{
public:
    result<void> parse(const std::string &url);

    std::string protocol() const noexcept { return protocol_; }
    std::string host() const noexcept { return host_; }
    std::string port() const noexcept { return port_; }
    std::string path() const noexcept;
    std::string service() const noexcept;

private:
    std::string protocol_;
    std::string host_;
    std::string port_;
    std::string path_;
};

} // namespace conet
