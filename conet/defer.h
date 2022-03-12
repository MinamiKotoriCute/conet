#pragma once

#include <type_traits>

namespace conet {

template <typename F>
struct privDefer
{
    F f;
    privDefer(const F &f) : f(f) {}
    privDefer(F &&f) : f(std::move(f)) {}
    ~privDefer() { f(); }
};

template <typename F>
privDefer<std::decay_t<F>> defer_func(F&& f)
{
    return privDefer<std::decay_t<F>>(std::forward<F>(f));
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x)    DEFER_2(x, __COUNTER__)
#define DEFER(code)   auto DEFER_3(_defer_) = defer_func([&](){code;})

} // namespace conet
