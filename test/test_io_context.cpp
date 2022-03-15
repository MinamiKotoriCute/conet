#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>

TEST(IoContextTest, CoSpanIsNotStrand)
{
    boost::asio::io_context io_context;

    std::atomic_int32_t n = 0;
    int max = 0;
    std::mutex mutex;

    auto f = [&] () -> boost::asio::awaitable<void>
        {
            ++n;
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (n > max)
                    max = n.load();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // if test fail. add sleep time.
            --n;
            co_return;
        };

    boost::asio::co_spawn(
        io_context,
        f(),
        [] (std::exception_ptr e)
        {
            EXPECT_FALSE(e.operator bool());
        }
    );
    boost::asio::co_spawn(
        io_context,
        f(),
        [] (std::exception_ptr e)
        {
            EXPECT_FALSE(e.operator bool());
        }
    );

    std::thread t1([&] { io_context.run(); });
    std::thread t2([&] { io_context.run(); });

    t1.join();
    t2.join();

    EXPECT_EQ(max, 2);
}

TEST(IoContextTest, IoObjectExcutorIsNotStrand)
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);

    std::atomic_int32_t n = 0;
    int max = 0;
    std::mutex mutex;

    auto f = [&] () -> boost::asio::awaitable<void>
        {
            ++n;
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (n > max)
                    max = n.load();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // if test fail. add sleep time.
            --n;
            co_return;
        };

    boost::asio::co_spawn(
        socket.get_executor(),
        f(),
        [] (std::exception_ptr e)
        {
            EXPECT_FALSE(e.operator bool());
        }
    );
    boost::asio::co_spawn(
        socket.get_executor(),
        f(),
        [] (std::exception_ptr e)
        {
            EXPECT_FALSE(e.operator bool());
        }
    );

    std::thread t1([&] { io_context.run(); });
    std::thread t2([&] { io_context.run(); });

    t1.join();
    t2.join();

    EXPECT_EQ(max, 2);
}

TEST(IoContextTest, CoroutineExcutorIsStrand)
{
    boost::asio::io_context io_context;

    std::atomic_int32_t n = 0;
    int max = 0;
    std::mutex mutex;

    auto f = [&] () -> boost::asio::awaitable<void>
        {
            ++n;
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (n > max)
                    max = n.load();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            --n;
            co_return;
        };

    boost::asio::co_spawn(
        io_context,
        [&] () -> boost::asio::awaitable<void>
        {
            boost::asio::co_spawn(
                co_await boost::asio::this_coro::executor,
                f(),
                [] (std::exception_ptr e)
                {
                    EXPECT_FALSE(e.operator bool());
                }
            );
            
            boost::asio::co_spawn(
                co_await boost::asio::this_coro::executor,
                f(),
                [] (std::exception_ptr e)
                {
                    EXPECT_FALSE(e.operator bool());
                }
            );
            co_return;
        }(),
        [] (std::exception_ptr e)
        {
            EXPECT_FALSE(e.operator bool());
        }
    );

    std::thread t1([&] { io_context.run(); });
    std::thread t2([&] { io_context.run(); });

    t1.join();
    t2.join();

    EXPECT_EQ(max, 1);
}
