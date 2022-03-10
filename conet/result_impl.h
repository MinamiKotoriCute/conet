#pragma once

#include <sstream>
#include <cstdio>
#include <memory>

#include <glog/logging.h>
#include <boost/system.hpp>

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

template<typename T>
T&& result_log(T &&result)
{
    LOG(WARNING) << result;
    return std::forward<T>(result);
}

int64_t result_get_unique_error_id();
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
    Result() :
        unique_error_id_(0)
    {
    }

    Result(const boost::system::error_code &error_code) :
        unique_error_id_(result_get_unique_error_id()),
        error_code_(error_code)
    {
    }

    Result(boost::system::error_code &&error_code) :
        unique_error_id_(result_get_unique_error_id()),
        error_code_(std::move(error_code))
    {
    }

    template<typename U>
    Result(const Result<U> &other) :
        unique_error_id_(other.unique_error_id_),
        error_code_(other.error_code_)
    {
    }

    template<typename U>
    Result(Result<U> &&other) :
        unique_error_id_(std::move(other).unique_error_id_),
        error_code_(std::move(other).error_code_)
    {
    }

    operator bool() const
    {
        return !has_error();
    }

    bool has_error() const
    {
        return unique_error_id_ != 0;
    }

    std::string error_message() const
    {
        return error_message_;
    }

    uint64_t unique_error_id() const
    {
        return unique_error_id_;
    }

    const boost::system::error_code& error_code() const
    {
        return error_code_;
    }

    friend std::ostream& operator<<(std::ostream &os, const this_type &other)
    {
        os << "unique_error_id:" << other.unique_error_id_
            << " error_message:" << other.error_message_
            << " what:" << other.error_code_.what();
        return os;
    }

protected:
    uint64_t unique_error_id_;
    std::string error_message_;
    boost::system::error_code error_code_;
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
    Result() :
        base_type()
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

    Result(const Result &other) :
        base_type(other),
        value_(other.value_)
    {
    }

    Result(Result &&other) :
        base_type(std::move(other)),
        value_(std::move(other).value_)
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
    Result() :
        base_type()
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

    Result(const Result &other) :
        base_type(other),
        value_(std::make_unique<ValueType>(*other.value_))
    {
    }

    Result(Result &&other) :
        base_type(std::move(other)),
        value_(std::make_unique<ValueType>(std::move(*other.value_)))
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
