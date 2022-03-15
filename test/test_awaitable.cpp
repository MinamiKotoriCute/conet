#include <gtest/gtest.h>
#include <glog/logging.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>

TEST(AwaitableTest, AwaitableCanMoveInNormalFunction)
{
    boost::asio::io_context io_context;

    std::atomic_int32_t check_point_1 = 0;

    auto f = [&] () -> boost::asio::awaitable<void>
        {
            ++check_point_1;
            co_return;
        };

    boost::asio::awaitable<void> a1;
    {
        auto&& a2 = f();
        a1 = std::move(a2);
    }

    boost::asio::co_spawn(
        io_context,
        std::move(a1),
        [] (std::exception_ptr e)
        {
            EXPECT_FALSE(e.operator bool());
        }
    );

    io_context.run();

    EXPECT_EQ(check_point_1.load(), 1);
}

TEST(AwaitableTest, AwaitableResumeWithOneParameter)
{
    boost::asio::io_context io_context;

    std::vector<int> check_point;
    std::function<void(int)> resume_func;

    auto f = [&] ()
        {
            check_point.push_back(5);
            return boost::asio::async_initiate<const boost::asio::use_awaitable_t<>, void(int)>
            (
                [&]<typename H> (H&& self) mutable
                {
                    check_point.push_back(6);
                    resume_func = [&, self = std::make_shared<H>(std::forward<H>(self))] (int result) mutable
                        {
                            check_point.push_back(8);
                            (*self)(result);
                            check_point.push_back(11);
                        };
                },
                boost::asio::use_awaitable
            );
        };

    check_point.push_back(1);
    boost::asio::co_spawn(
        io_context,
        [&] () -> boost::asio::awaitable<void>
        {
            check_point.push_back(4);
            int n = co_await f();
            check_point.push_back(9);
            EXPECT_EQ(n, -1);
            co_return;
        },
        [&] (std::exception_ptr e)
        {
            check_point.push_back(10);
            EXPECT_FALSE(e.operator bool());
        }
    );

    check_point.push_back(2);
    boost::asio::co_spawn(
        io_context,
        [&] () -> boost::asio::awaitable<void>
        {
            check_point.push_back(7);
            if (resume_func)
                resume_func(-1);
            check_point.push_back(12);

            co_return;
        },
        [&] (std::exception_ptr e)
        {
            check_point.push_back(13);
            EXPECT_FALSE(e.operator bool());
        }
    );

    check_point.push_back(3);
    io_context.run();
    check_point.push_back(14);
    
    std::vector<int> expect_check_point = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    EXPECT_EQ(check_point, expect_check_point);
}

TEST(AwaitableTest, AwaitableResumeWithTwoParameter)
{
    boost::asio::io_context io_context;

    std::atomic_int32_t check_point_1 = 0;
    std::atomic_int32_t check_point_2 = 0;

    std::function<void(int, int)> resume_func;

    auto f = [&] ()
        {
            return boost::asio::async_initiate<const boost::asio::use_awaitable_t<>, void(int, int)>
            (
                [&]<typename H> (H&& self) mutable
                {
                    resume_func = [self = std::make_shared<H>(std::forward<H>(self))] (int a1, int a2) mutable
                        {
                            (*self)(a1, a2);
                        };
                },
                boost::asio::use_awaitable
            );
        };

    boost::asio::co_spawn(
        io_context,
        [&] () -> boost::asio::awaitable<void>
        {
            ++check_point_1;

            auto [a1, a2] = co_await f();
            EXPECT_EQ(a1, 10);
            EXPECT_EQ(a2, 20);

            ++check_point_2;
            co_return;
        },
        [] (std::exception_ptr e)
        {
            EXPECT_FALSE(e.operator bool());
        }
    );

    boost::asio::co_spawn(
        io_context,
        [&] () -> boost::asio::awaitable<void>
        {
            if (resume_func)
                resume_func(10, 20);

            co_return;
        },
        [] (std::exception_ptr e)
        {
            EXPECT_FALSE(e.operator bool());
        }
    );

    io_context.run();

    EXPECT_EQ(check_point_1.load(), 1);
    EXPECT_EQ(check_point_2.load(), 1);
}

