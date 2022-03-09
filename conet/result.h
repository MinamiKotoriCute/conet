#pragma once

#include "result_impl.h"

namespace conet {

template<typename T>
using result = impl::Result<T>;

} // namespace conet
