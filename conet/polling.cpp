#include "polling.h"

#include <chrono>

#include <glog/logging.h>

Polling& Polling::instance()
{
    static Polling object;
    return object;
}

Polling::~Polling()
{
    {
        std::lock_guard<std::mutex> lock(functions_mutex_);
        functions_.clear();
    }

    {
        std::lock_guard<std::mutex> lock(thread_mutex_);
        if (thread_.joinable())
            thread_.join();
    }
}

void Polling::add(void *c, std::function<bool()> &&f)
{
    {
        std::lock_guard<std::mutex> lock(functions_mutex_);
        functions_[c].push_back(std::move(f));
    }

    {
        std::lock_guard<std::mutex> lock(thread_mutex_);
        if (!thread_.joinable())
            thread_ = std::thread(std::bind(&Polling::run, this));
    }
}

void Polling::remove(void *c)
{
    std::lock_guard<std::mutex> lock(functions_mutex_);
    functions_.erase(c);
}

void Polling::run()
{
    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(functions_mutex_);
            std::erase_if(functions_, [] (auto &p)
            {
                std::erase_if(p.second, [] (auto &f)
                {
                    return f();
                });

                return p.second.empty();
            });

            if (functions_.empty())
                break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    

    std::lock_guard<std::mutex> lock(thread_mutex_);
    thread_.detach();
    thread_ = {};
}

