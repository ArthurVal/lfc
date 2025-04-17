#pragma once

#include <type_traits>

namespace lfc::internal {

template <class = void, class... Args>
struct HasIsValidFreeFunctionImpl : std::false_type {};

template <class... Args>
struct HasIsValidFreeFunctionImpl<
    std::void_t<decltype(IsValid(std::declval<Args>()...))>, Args...>
    : std::is_convertible<decltype(IsValid(std::declval<Args>()...)), bool> {};

template <class... Args>
struct HasIsValidFreeFunction : HasIsValidFreeFunctionImpl<void, Args...> {};

template <class... Args>
constexpr bool HasIsValidFreeFunction_v =
    HasIsValidFreeFunction<Args...>::value;

}  // namespace lfc::internal
