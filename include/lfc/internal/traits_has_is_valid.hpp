#pragma once

#include "has_trait.hpp"

namespace lfc::internal {

namespace details {

template <class... Args>
using IsValidFreeFunction = decltype(IsValid(std::declval<Args>()...));

}

template <class... Args>
struct HasIsValidFreeFunction
    : HasTrait<details::IsValidFreeFunction, Args...> {};

template <class... Args>
constexpr bool HasIsValidFreeFunction_v =
    HasIsValidFreeFunction<Args...>::value;

} // namespace lfc::internal
