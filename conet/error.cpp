#include "error.h"

namespace conet {
namespace error {

const char* conet_category_impl::name() const noexcept
{
    return "conet";
}

std::string conet_category_impl::message(int ev) const
{
    switch (ev)
    {
    case parameter_error:
        return "parameter_error";
    case third_party_error:
        return "third_party_error";
    case internal_error:
        return "internal_error";
    }

    return "conet error";
}

const boost::system::error_category& conet_category()
{
    static const conet_category_impl instance;
    return instance;
}

const char* network_category_impl::name() const noexcept
{
    return "conet.network";
}

std::string network_category_impl::message(int ev) const
{
    switch (ev)
    {
    case connection_closed:
        return "connection_closed";
    }

    return "conet.network error";
}

const boost::system::error_category& network_category()
{
    static const network_category_impl instance;
    return instance;
}


const char* mysql_category_impl::name() const noexcept
{
    return "conet.mysql";
}

std::string mysql_category_impl::message(int ev) const
{
    return "conet.mysql error";
}

const boost::system::error_category& mysql_category()
{
    static const mysql_category_impl instance;
    return instance;
}

} // namespace error
} // namespace conet
