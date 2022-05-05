#pragma once

#include <string>
#include <sstream>

#include <boost/asio.hpp>
#include <mysql/mysql.h>

#include "error.h"
#include "result.h"

struct MYSQL;

namespace conet {

std::string get_mysql_error(MYSQL *mysql);

class initiate_mysql_connect;
class initiate_mysql_query;
class initiate_mysql_store_result;
class initiate_mysql_fetch_row;

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

private:
    std::unordered_map<std::string, std::string> datas_;
};

class MysqlClient
{
    friend initiate_mysql_connect;
    friend initiate_mysql_query;
    friend initiate_mysql_store_result;
    friend initiate_mysql_fetch_row;

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
        RESULT_CO_CHECK(co_await mysql_query(sql));
        RESULT_CO_AUTO(res, co_await mysql_store_result());
        if (res == nullptr)
            co_return RESULT_SUCCESS;

        RESULT_CO_AUTO(row, co_await mysql_fetch_row(res));

        unsigned long field_num = mysql_num_fields(res);

        MYSQL_FIELD* field = nullptr;
        std::vector<std::string> field_name;
        for (unsigned long i=0; i<field_num; ++i)
        {
            field = mysql_fetch_field(res);
            if (field == nullptr)
            {
                boost::system::error_code error_code;
                static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
                error_code.assign(mysql_errno(mysql_), error::mysql_category(), &loc);

                ErrorInfo error_info(error_code);
                error_info.set_error_message(get_mysql_error(mysql_));
                co_return error_info;
            }

            field_name.push_back(field->name);
        }
        
        std::vector<T> query_result_group;
        while (row)
        {
            unsigned long* element_size = mysql_fetch_lengths(res);
            if (element_size == nullptr)
            {
                boost::system::error_code error_code;
                static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
                error_code.assign(mysql_errno(mysql_), error::mysql_category(), &loc);

                ErrorInfo error_info(error_code);
                error_info.set_error_message(get_mysql_error(mysql_));
                co_return error_info;
            }
            

            if (field_name.size() != field_num)
            {
                boost::system::error_code error_code;
                static constexpr boost::source_location loc = BOOST_CURRENT_LOCATION;
                error_code.assign(error::third_party_error, error::conet_category(), &loc);
                co_return error_code;
            }

            T query_result;
            for (unsigned long i=0; i<field_num; ++i)
            {
                std::string key = field->name;
                std::string value(row[i], static_cast<std::size_t>(element_size[i]));
                query_result.add_result(field_name[i], value);
            }
            query_result_group.push_back(std::move(query_result));

            RESULT_CO_TRY(row, co_await mysql_fetch_row(res));
        }

        co_return query_result_group;
    }

    std::string encode_string(const std::string &raw)
    {
        std::string ret;
        ret.resize(raw.size()*2+1);
        auto size = mysql_real_escape_string(mysql_, &ret[0], &raw[0], raw.size());
        ret.resize(size);
        return ret;
    }

private:
    boost::asio::awaitable<result<void>> mysql_query(const std::string& sql);
    boost::asio::awaitable<result<MYSQL_RES *>> mysql_store_result();
    boost::asio::awaitable<result<char **>> mysql_fetch_row(MYSQL_RES *r);

    MYSQL *mysql_ = nullptr;
    std::string host_;
    unsigned int port_ = 0;
    std::string user_;
    std::string password_;
    std::string database_;
};

}
