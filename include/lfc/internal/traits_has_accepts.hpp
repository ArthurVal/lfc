#pragma once

#include <type_traits>

namespace lfc::internal {

template <class = void, class... Args>
struct HasAcceptsFreeFunctionImpl : std::false_type {};

template <class... Args>
struct HasAcceptsFreeFunctionImpl<
    std::void_t<decltype(Accepts(std::declval<Args>()...))>, Args...>
    : std::is_convertible<decltype(Accepts(std::declval<Args>()...)), bool> {};

template <class... Args>
struct HasAcceptsFreeFunction : HasAcceptsFreeFunctionImpl<void, Args...> {};

template <class... Args>
constexpr bool HasAcceptsFreeFunction_v =
    HasAcceptsFreeFunction<Args...>::value;

}  // namespace lfc::internal
