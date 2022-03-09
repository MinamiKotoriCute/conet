#include <memory>

#include <gtest/gtest.h>

#include "conet/error.h"
#include "conet/result.h"

struct NoDefaultConstructor
{
    NoDefaultConstructor() = delete;
};

TEST(ResultTest, ConstructorDefault)
{
    // void
    conet::result<void> r1;
    
    // build-in type
    conet::result<int> r2;
    
    // move-only
    conet::result<std::unique_ptr<int>> r3;
    
    // no default constructor type
    // conet::result<NoDefaultConstructor> r4; // not support
}

TEST(ResultTest, ConstructorAcceptsErrorCode)
{
    boost::system::error_code ec(1, conet::error::conet_category());

    // void
    conet::result<void> r1(ec);
    EXPECT_EQ(r1.error_code(), boost::system::error_condition(1, conet::error::conet_category()));

    // build-in type
    conet::result<int> r2(ec);
    EXPECT_EQ(r2.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
    
    // move-only
    conet::result<std::unique_ptr<int>> r3(ec);
    EXPECT_EQ(r3.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
}

TEST(ResultTest, ConstructorCopyCheckalue)
{
    conet::result<void> a1;
    conet::result<int> b1(1);
    conet::result<std::unique_ptr<int>> c1(std::make_unique<int>(1));
    
    // copy value, same type
    conet::result<void> a2(a1);
    conet::result<int> b2(b1);
    EXPECT_EQ(b1.value(), b2.value());

    // conet::result<std::unique_ptr<int>> c2(c1); // std::unique_ptr<int> is move-only type. so c1 is move-only type too.
    // EXPECT_EQ(c1.value(), c2.value());


    // difference type
    conet::result<void> a3(b1);
    conet::result<void> a4(c1);
    conet::result<int> b3(a1); // b3.value() not init
    conet::result<int> b4(c1);
    conet::result<std::unique_ptr<int>> c3(a1); // it's ok
    EXPECT_EQ(c3.value().get(), nullptr);
    conet::result<std::unique_ptr<int>> c4(a2);
    EXPECT_EQ(c4.value().get(), nullptr);
}

TEST(ResultTest, ConstructorCopyCheckErrorCode)
{
    boost::system::error_code ec(1, conet::error::conet_category());

    conet::result<void> a1(ec);
    conet::result<int> b1(ec);
    conet::result<std::unique_ptr<int>> c1(ec);


    // copy error, same type
    conet::result<void> a2(a1);
    EXPECT_EQ(a2.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
    conet::result<int> b2(b1);
    EXPECT_EQ(b2.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
    // conet::result<std::unique_ptr<int>> c2(c1); // std::unique_ptr<int> is move-only type. so c1 is move-only type too.
    // EXPECT_EQ(c2.error_code(), boost::system::error_condition(1, conet::conet_category()));


    // difference type
    conet::result<void> a3(b1);
    EXPECT_EQ(a3.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
    conet::result<void> a4(c1);
    EXPECT_EQ(a4.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
    conet::result<int> b3(a1);
    EXPECT_EQ(b3.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
    conet::result<int> b4(c1);
    EXPECT_EQ(b4.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
    conet::result<std::unique_ptr<int>> c3(a1);
    EXPECT_EQ(c3.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
    conet::result<std::unique_ptr<int>> c4(b1);
    EXPECT_EQ(c4.error_code(), boost::system::error_condition(1, conet::error::conet_category()));
}
