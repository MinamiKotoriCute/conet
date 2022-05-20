#pragma once

#include <boost/json.hpp>
#include "result.h"
#include "error.h"

namespace conet {

template<typename T>
result<void> json_object_get_value(const boost::json::object &object, const std::string &key, T &value)
{
    auto it = object.find(key);
    if (it == object.end())
    {
        boost::system::error_code error_code;
        static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
        error_code.assign(conet::error::internal_error, conet::error::conet_category(), &loc);

        conet::ErrorInfo error_info(error_code);
        error_info.set_error_message("json key not find");
        error_info.add_pair("key", key);

        return error_info;
    }

    value = boost::json::value_to<T>(it->value());
    return RESULT_SUCCESS;
}

template<typename T>
result<void> json_object_get_value_maybe_no_exist(const boost::json::object &object, const std::string &key, T &value)
{
    auto it = object.find(key);
    if (it == object.end())
    {
        return RESULT_SUCCESS;
    }

    value = boost::json::value_to<T>(it->value());
    return RESULT_SUCCESS;
}

template<typename T>
result<T> json_object_get_value(const boost::json::object &object, const std::string &key)
{
    T t;
    RESULT_CHECK(json_object_get_value(object, key, t));
    return t;
}

} // namespace conet