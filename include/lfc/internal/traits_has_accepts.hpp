#pragma once

#include "has_trait.hpp"

namespace lfc::internal {

namespace details {

template <class... Args>
using AcceptsFreeFunction = decltype(Accepts(std::declval<Args>()...));

}

template <class... Args>
struct HasAcceptsFreeFunction
    : HasTrait<details::AcceptsFreeFunction, Args...> {};

template <class... Args>
constexpr bool HasAcceptsFreeFunction_v =
    HasAcceptsFreeFunction<Args...>::value;

}  // namespace lfc::internal
