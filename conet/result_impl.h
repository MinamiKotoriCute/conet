#pragma once

#include <sstream>
#include <cstdio>
#include <memory>

#include <glog/logging.h>

#include "error_info.h"

#define RESULT_ERROR(...) conet::impl::Result<void>(ResultFailureType{}, ##__VA_ARGS__)
#define RESULT_SUCCESS conet::impl::Result<void>()

#define RESULT_CONCAT_INNER(a, b) a ## b
#define RESULT_CONCAT(a, b) RESULT_CONCAT_INNER(a, b)
#define RESULT_VARIABLE_TMP RESULT_CONCAT(result_tmp_, __LINE__)

#define RESULT_LOG(level, e) \
[]<typename RESULT_LOG_T> (RESULT_LOG_T &&result) -> RESULT_LOG_T&& \
{ \
    LOG(level) << result; \
    return std::forward<RESULT_LOG_T>(result); \
}(std::move(e));

#define RESULT_LOG_WITH_ERROR(level, result)


#define RESULT_CHECK(e) \
{ \
    auto &&r = e; \
    if (r.has_error()) \
    { \
        RESULT_LOG_WITH_ERROR(INFO, r); \
        return r; \
    } \
}

#define RESULT_TRY(v, e) \
auto && RESULT_VARIABLE_TMP = e; \
if ( !RESULT_VARIABLE_TMP ) \
{ \
    RESULT_LOG_WITH_ERROR(INFO, RESULT_VARIABLE_TMP); \
    return RESULT_VARIABLE_TMP; \
} \
v = std::forward<decltype(RESULT_VARIABLE_TMP)>(RESULT_VARIABLE_TMP).value();

#define RESULT_AUTO(r, e) RESULT_TRY(auto &&r, e)

#define RESULT_CO_CHECK(e) \
{ \
    auto &&r = e; \
    if ( !r ) \
    { \
        RESULT_LOG_WITH_ERROR(INFO, r); \
        co_return r; \
    } \
}

#define RESULT_CO_TRY(v, e) \
auto && RESULT_VARIABLE_TMP = e; \
if ( !RESULT_VARIABLE_TMP ) \
{ \
    RESULT_LOG_WITH_ERROR(INFO, RESULT_VARIABLE_TMP); \
    co_return RESULT_VARIABLE_TMP; \
} \
v = std::forward<decltype(RESULT_VARIABLE_TMP)>(RESULT_VARIABLE_TMP).value();

#define RESULT_CO_AUTO(r, e) RESULT_CO_TRY(auto &&r, e)



namespace conet {
namespace impl {

std::string string_format(const std::string format, ...);
std::string string_format();

struct ResultFailureType
{
};

template<typename T>
class Result {};

template<>
class Result<void>
{
    using this_type = Result;

    template<typename T>
    friend class Result;

public:
    Result() = default;
    Result(const Result &) = default;
    Result(Result &&) = default;

    Result(const ErrorInfo &error_info) :
        error_info_(error_info)
    {
    }

    Result(ErrorInfo &&error_info) :
        error_info_(std::move(error_info))
    {
    }

    Result(const boost::system::error_code &error_code) :
        error_info_(error_code)
    {
    }

    Result(boost::system::error_code &&error_code) :
        error_info_(std::move(error_code))
    {
    }

    template<typename U>
    Result(const Result<U> &other) :
        error_info_(other.error_info_)
    {
    }

    template<typename U>
    Result(Result<U> &&other) :
        error_info_(std::move(other).error_info_)
    {
    }

    // is_success
    operator bool() const
    {
        return !has_error();
    }

    bool has_error() const
    {
        return error_info_.has_error();
    }

    const ErrorInfo& error_info() const
    {
        return error_info_;
    }

    ErrorInfo& error_info()
    {
        return error_info_;
    }

protected:
    ErrorInfo error_info_;
};

template<typename ValueType>
    requires (std::default_initializable<ValueType> && !std::same_as<ValueType, void>)
class Result<ValueType> : public Result<void>
{
    using this_type = Result;
    using base_type = Result<void>;

    template<typename T>
    friend class Result;

public:
    Result() = default;
    Result(const Result &) = default;
    Result(Result &&) = default;

    Result(const ErrorInfo &error_info) :
        base_type(error_info)
    {
    }

    Result(ErrorInfo &&error_info) :
        base_type(std::move(error_info))
    {
    }

    Result(const boost::system::error_code &error_code) :
        base_type(error_code)
    {
    }

    Result(boost::system::error_code &&error_code) :
        base_type(std::move(error_code))
    {
    }

    Result(const ValueType &x) :
        base_type(),
        value_(x)
    {
    }

    Result(ValueType &&x) :
        base_type(),
        value_(std::move(x))
    {
    }

    template<typename U>
    Result(const Result<U> &other) :
        base_type(other)
    {
    }

    template<typename U>
    Result(Result<U> &&other) :
        base_type(std::move(other))
    {
    }

    ValueType&& value() &&
    {
        return std::move(value_);
    }

    ValueType& value() &
    {
        return value_;
    }

    const ValueType& value() const &
    {
        return value_;
    }

    const ValueType& value() const &&
    {
        return value_;
    }

protected:
    ValueType value_;
};


template<typename ValueType>
    requires (!std::default_initializable<ValueType> && !std::same_as<ValueType, void>)
class Result<ValueType> : public Result<void>
{
    using this_type = Result;
    using base_type = Result<void>;

    template<typename T>
    friend class Result;

public:
    Result() = default;
    Result(const Result &) = default;
    Result(Result &&) = default;

    Result(const ErrorInfo &error_info) :
        base_type(error_info)
    {
    }

    Result(ErrorInfo &&error_info) :
        base_type(std::move(error_info))
    {
    }

    Result(const boost::system::error_code &error_code) :
        base_type(error_code)
    {
    }

    Result(boost::system::error_code &&error_code) :
        base_type(std::move(error_code))
    {
    }

    Result(const ValueType &x) :
        base_type(),
        value_(std::make_unique<ValueType>(x))
    {
    }

    Result(ValueType &&x) :
        base_type(),
        value_(std::make_unique<ValueType>(std::move(x)))
    {
    }

    template<typename U>
    Result(const Result<U> &other) :
        base_type(other)
    {
    }

    template<typename U>
    Result(Result<U> &&other) :
        base_type(std::move(other))
    {
    }

    ValueType& value() &
    {
        return *value_;
    }

    ValueType&& value() &&
    {
        return std::move(*value_);
    }

    const ValueType& value() const &
    {
        return *value_;
    }

    const ValueType& value() const &&
    {
        return *value_;
    }

protected:
    std::unique_ptr<ValueType> value_;
};

} // namespace impl
} // namespace conet
