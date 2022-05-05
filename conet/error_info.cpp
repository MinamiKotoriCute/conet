#include "error_info.h"

#include <sstream>

namespace conet {

static int64_t result_get_unique_error_id()
{
    static std::atomic_uint64_t id;
    return ++id;
}

ErrorInfo::ErrorInfo() :
    error_id_(0)
{
}

ErrorInfo::ErrorInfo(const boost::system::error_code &error_code) :
    error_id_(result_get_unique_error_id()),
    error_code_(error_code)
{
}

ErrorInfo::ErrorInfo(boost::system::error_code &&error_code) :
    error_id_(result_get_unique_error_id()),
    error_code_(std::move(error_code))
{
}

bool ErrorInfo::has_error() const
{
    return error_id_ != 0;
}

ErrorInfo& ErrorInfo::set_error_message(const std::string &error_message)
{
    error_message_ = error_message;
    return *this;
}

ErrorInfo& ErrorInfo::set_error_message(std::string &&error_message)
{
    error_message_ = std::move(error_message);
    return *this;
}

std::string ErrorInfo::beautiful_output() const
{
    std::stringstream ss;

    // error_id
    ss << "error_id: " << error_id_;

    // error_code
    ss << "\nerror_num: " << error_code_.value()
        << "\nerror_name: " << error_code_.message()
        << "\nerror_category: " << error_code_.category().name();
    if (error_code_.has_location())
    {
        ss << "\nlocation: " << error_code_.location().file_name() << ":" << error_code_.location().line()
            << "\nfunction: " << error_code_.location().function_name();
    }
    
    // error_message
    if (!error_message_.empty())
    {
        ss << "\nerror_message: " << error_message_ << "\n";
    }

    // pairs
    for (const auto &p : pairs_)
    {
        ss << "\n" << p.first << ": " << p.second;
    }

    return ss.str();
}

std::ostream& operator<<(std::ostream &os, const ErrorInfo &other)
{
    // error_id
    os << "[error_id=" << other.error_id_;

    // error_code
    os << ",error_num=" << other.error_code_.value()
        << ",error_name=" << other.error_code_.message()
        << ",error_category=" << other.error_code_.category().name();
    if (other.error_code_.has_location())
    {
        os << ",line=" << other.error_code_.location().line()
            << ",file=" << other.error_code_.location().file_name()
            << ",function=" << other.error_code_.location().function_name();
    }

    // error_message
    if (!other.error_message_.empty())
    {
        os << ",error_message=" << other.error_message_;
    }

    // pairs
    for (const auto &p : other.pairs_)
    {
        os << "," << p.first << "=" << p.second;
    }
    os << "]";

    return os;
}

} // namespace conet
