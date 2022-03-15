#pragma once

#include <functional>
#include "result.h"

namespace conet {

template<typename Key, typename CallRet, typename... CallParameters>
class CallbackRegister
{
public:
    void regist(const Key &key, std::function<CallRet(CallParameters...)> &&f)
    {
        callbacks_[key] = std::move(f);
    }

    void regist(const Key &key, const std::function<CallRet(CallParameters...)> &f)
    {
        callbacks_[key] = f;
    }

    bool exist(const Key &key) const
    {
        return callbacks_.count(key);
    }

    template<typename... Args>
    CallRet call(const Key &key, Args...&& args) const
    {
        auto it = callbacks_.find(key);
        if (it == callbacks_.end())
            return {};
        
        return it->second(std::forward<Args>(args)...);
    }

protected:
    std::map<Key, std::funcion<CallRet(CallParameters...)>> callbacks_;
};

} // namespace conet
