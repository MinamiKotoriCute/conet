#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <sstream>
#include <vector>

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
    const std::map<std::string, std::vector<std::string>>& pairs() const { return pairs_; }

    ErrorInfo& set_error_message(const std::string &error_message);
    ErrorInfo& set_error_message(std::string &&error_message);

    template<typename T>
    ErrorInfo& add_pair(const std::string &key, const T &value)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            pairs_[key].emplace_back(value);
        }
        else
        {
            std::stringstream ss;
            ss << value;
            pairs_[key].emplace_back(ss.str());
        }

        return *this;
    }

    template<typename T>
    ErrorInfo& add_pair(const std::string &key, T &&value)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            pairs_[key].emplace_back(std::forward<T>(value));
        }
        else
        {
            std::stringstream ss;
            ss << std::forward<T>(value);
            pairs_[key].emplace_back(ss.str());
        }

        return *this;
    }

    friend std::ostream& operator<<(std::ostream &os, const ErrorInfo &other);
    std::string beautiful_output() const;

private:
    std::uint64_t error_id_;
    boost::system::error_code error_code_;
    std::string error_message_;
    std::map<std::string, std::vector<std::string>> pairs_;
};

} // namespace conet
