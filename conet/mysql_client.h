#pragma once

#include <string>
#include <sstream>
#include <memory>

#include <boost/asio.hpp>

#include "error.h"
#include "result.h"

struct MYSQL;
struct MYSQL_RES;
struct MYSQL_FIELD;

// mysql api
extern "C"
{
unsigned int mysql_errno(MYSQL *mysql);
unsigned long *mysql_fetch_lengths(MYSQL_RES *result);
}

namespace conet {

std::string get_mysql_error(MYSQL *mysql);

class initiate_mysql_connect;
class initiate_mysql_query;
class initiate_mysql_store_result;
class initiate_mysql_fetch_row;
class initiate_mysql_free_result;

class MysqlQueryResultImpl
{
public:
    template<typename T>
    T get(const std::string &key) const
    {
        auto it = datas_.find(key);
        if (it == datas_.end())
            return {};

        std::stringstream ss(it->second);
        T tmp;
        ss >> tmp;
        return tmp;
    }

    template<typename T>
        requires std::same_as<T, std::string>
    T get(const std::string &key) const
    {
        auto it = datas_.find(key);
        if (it == datas_.end())
            return {};
        
        return it->second;
    }

    void add_result(const std::string &key, const std::string &value);
    void add_result(const std::string &key, std::string &&value);

private:
    std::unordered_map<std::string, std::string> datas_;
};

class MysqlClient
{
    friend initiate_mysql_connect;
    friend initiate_mysql_query;
    friend initiate_mysql_store_result;
    friend initiate_mysql_fetch_row;
    friend initiate_mysql_free_result;

public:
    MysqlClient() = default;
    ~MysqlClient();

    MysqlClient(const MysqlClient &) = delete;
    MysqlClient(MysqlClient &&other);
    MysqlClient& operator=(const MysqlClient &) = delete;
    MysqlClient& operator=(MysqlClient &&other);

    boost::asio::awaitable<result<void>> connect(const std::string& host, unsigned int port, const std::string& user, const std::string& password, const std::string& database);

    void close();

    template<typename T = MysqlQueryResultImpl>
    boost::asio::awaitable<result<std::vector<T>>> query(const std::string& sql)
    {
        RESULT_CO_AUTO(results, co_await query_real(sql), r.error_info().add_pair("sql", sql));

        co_return std::move(results);
    }

    std::string encode_string(const std::string &raw);

private:
    template<typename T = MysqlQueryResultImpl>
    boost::asio::awaitable<result<std::vector<T>>> query_real(const std::string& sql)
    {
        RESULT_CO_CHECK(co_await mysql_query(sql));
        RESULT_CO_AUTO(res, co_await mysql_store_result());
        if (res == nullptr)
            co_return RESULT_SUCCESS;

        RESULT_CO_AUTO(row, co_await mysql_fetch_row(res.get()));
        RESULT_CO_AUTO(field_names, get_fields(res.get()));
        
        std::vector<T> query_result_group;
        while (row)
        {
            unsigned long* element_size = mysql_fetch_lengths(res.get());
            if (element_size == nullptr)
            {
                boost::system::error_code error_code;
                static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
                error_code.assign(mysql_errno(mysql_), error::mysql_category(), &loc);

                ErrorInfo error_info(error_code);
                error_info.set_error_message(get_mysql_error(mysql_));
                co_return error_info;
            }

            T query_result;
            for (unsigned long i=0; i<field_names.size(); ++i)
            {
                std::string value(row[i], static_cast<std::size_t>(element_size[i]));
                query_result.add_result(field_names[i], std::move(value));
            }
            query_result_group.push_back(std::move(query_result));

            RESULT_CO_TRY(row, co_await mysql_fetch_row(res.get()));
        }

        co_return query_result_group;
    }

    boost::asio::awaitable<result<void>> mysql_query(const std::string& sql);
    boost::asio::awaitable<result<std::shared_ptr<MYSQL_RES>>> mysql_store_result();
    boost::asio::awaitable<result<char **>> mysql_fetch_row(MYSQL_RES *r);
    boost::asio::awaitable<result<void>> mysql_free_result(MYSQL_RES *r);

    result<std::vector<std::string>> get_fields(MYSQL_RES *res);

    MYSQL *mysql_ = nullptr;
    std::string host_;
    unsigned int port_ = 0;
    std::string user_;
    std::string password_;
    std::string database_;
};

}
