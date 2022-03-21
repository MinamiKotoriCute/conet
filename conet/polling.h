#pragma once

#include <thread>
#include <thread>
#include <functional>
#include <unordered_map>
#include <list>
#include <mutex>

class Polling
{
public:
    static Polling& instance();

    ~Polling();

    void add(void *c, std::function<bool()> &&f);
    void remove(void *c);

private:
    Polling() = default;
    void run();

    bool is_close_ = false;
    std::mutex thread_mutex_;
    std::thread thread_;
    std::mutex functions_mutex_;
    std::unordered_map<void *, std::list<std::function<bool()>>> functions_;
};
