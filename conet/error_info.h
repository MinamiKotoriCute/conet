#pragma once

#include <cstdint>
#include <map>
#include <string>

#include <boost/system.hpp>

namespace conet {

class ErrorInfo
{
public:
    ErrorInfo();
    ErrorInfo(const boost::system::error_code &error_code);
    ErrorInfo(boost::system::error_code &&error_code);

    bool has_error() const;
    std::uint64_t error_id() const { return error_id_; }
    const boost::system::error_code& error_code() const { return error_code_; }
    const std::string& error_message() const { return error_message_; }
    const std::map<std::string, std::string>& pairs() const { return pairs_; }

    ErrorInfo& set_error_message(const std::string &error_message);
    ErrorInfo& set_error_message(std::string &&error_message);

    ErrorInfo& add_pair(const std::string &key, const std::string &value);
    friend std::ostream& operator<<(std::ostream &os, const ErrorInfo &other);
    std::string beautiful_output() const;

private:
    std::uint64_t error_id_;
    boost::system::error_code error_code_;
    std::string error_message_;
    std::map<std::string, std::string> pairs_;
};

} // namespace conet
