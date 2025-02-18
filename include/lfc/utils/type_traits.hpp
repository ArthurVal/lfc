#pragma once

#include <functional>   // std::reference_wrapper
#include <type_traits>  // false_type

namespace lfc::details {

/// Always evaluates to false
template <typename...>
struct AlwaysFalse : std::false_type {};

template <typename... T>
constexpr bool AlwaysFalse_v = AlwaysFalse<T...>::value;

/// Always evaluates to true
template <typename...>
struct AlwaysTrue : std::true_type {};

template <typename... T>
constexpr bool AlwaysTrue_v = AlwaysTrue<T...>::value;

/// Remove reference, const and volatile qualifiers.
template <class T>
struct RemoveCVRef {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <class T>
using RemoveCVRef_t = typename RemoveCVRef<T>::type;

/// Transforms std::reference_wrapper<T> to T&. Any other type is untouched.
template <class T>
struct UnwrapRefWrapper {
  using type = T;
};

template <class T>
struct UnwrapRefWrapper<std::reference_wrapper<T>> {
  using type = T&;
};

template <class T>
using UnwrapRefWrapper_t = typename UnwrapRefWrapper<T>::type;

}  // namespace lfc::details
