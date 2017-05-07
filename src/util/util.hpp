#pragma once
#include <functional>
#include <memory>
#include <vector>

namespace sk
{
template <typename T>
using UniquePtrVector = std::vector<std::unique_ptr<T>>;
template <typename T>
using RefVector = std::vector<std::reference_wrapper<T>>;
}