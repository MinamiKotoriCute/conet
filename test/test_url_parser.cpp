#include <memory>

#include <gtest/gtest.h>

#include "conet/url_parser.h"


TEST(UrlParserTest, Ip)
{
    conet::UrlParser url_parser;
    auto result = url_parser.parse("127.0.0.1");

    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(url_parser.protocol(), "");
    EXPECT_EQ(url_parser.host(), "127.0.0.1");
    EXPECT_EQ(url_parser.port(), "");
    EXPECT_EQ(url_parser.path(), "/");
    EXPECT_EQ(url_parser.service(), "80");
}

TEST(UrlParserTest, IpPort)
{
    conet::UrlParser url_parser;
    auto result = url_parser.parse("127.0.0.1:45678");

    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(url_parser.protocol(), "");
    EXPECT_EQ(url_parser.host(), "127.0.0.1");
    EXPECT_EQ(url_parser.port(), "45678");
    EXPECT_EQ(url_parser.path(), "/");
    EXPECT_EQ(url_parser.service(), "45678");
}

TEST(UrlParserTest, IpPortPath)
{
    conet::UrlParser url_parser;
    auto result = url_parser.parse("127.0.0.1:45678/query?");

    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(url_parser.protocol(), "");
    EXPECT_EQ(url_parser.host(), "127.0.0.1");
    EXPECT_EQ(url_parser.port(), "45678");
    EXPECT_EQ(url_parser.path(), "/query?");
    EXPECT_EQ(url_parser.service(), "45678");
}

TEST(UrlParserTest, HttpIp)
{
    conet::UrlParser url_parser;
    auto result = url_parser.parse("http://127.0.0.1");

    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(url_parser.protocol(), "http");
    EXPECT_EQ(url_parser.host(), "127.0.0.1");
    EXPECT_EQ(url_parser.port(), "");
    EXPECT_EQ(url_parser.path(), "/");
    EXPECT_EQ(url_parser.service(), "http");
}

TEST(UrlParserTest, HttpIpPort)
{
    conet::UrlParser url_parser;
    auto result = url_parser.parse("http://127.0.0.1:45678");

    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(url_parser.protocol(), "http");
    EXPECT_EQ(url_parser.host(), "127.0.0.1");
    EXPECT_EQ(url_parser.port(), "45678");
    EXPECT_EQ(url_parser.path(), "/");
    EXPECT_EQ(url_parser.service(), "45678");
}
