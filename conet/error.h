#pragma once

#include <boost/system.hpp>

namespace conet {
namespace error {

enum basic_errors
{
    parameter_error = -1,
    third_party_error = -2,
    internal_error = -3,
};

class conet_category_impl : public boost::system::error_category
{
public:
    const char* name() const noexcept override;
    std::string message(int ev) const override;
};

const boost::system::error_category& conet_category();

enum network_errors
{
    connection_closed = 1,
};

class network_category_impl : public boost::system::error_category
{
public:
    const char* name() const noexcept override;
    std::string message(int ev) const override;
};

const boost::system::error_category& network_category();

} // namespace error
} // namespace conet
