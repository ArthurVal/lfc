#pragma once

#include <type_traits>  // false_type

namespace lfc::utils::details {

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

/// TODO
template <class Out, std::size_t N, class... T>
struct RepeatInto;

template <template <class...> class Out, std::size_t N, class... T,
          class... Others>
struct RepeatInto<Out<Others...>, N, T...>
    : RepeatInto<Out<Others..., T...>, N - 1, T...> {};

template <template <class...> class Out, class... T, class... Others>
struct RepeatInto<Out<Others...>, 0, T...> {
  using type = Out<Others...>;
};

template <class Out, std::size_t N, class... T>
using RepeatInto_t = typename RepeatInto<Out, N, T...>::type;

}  // namespace lfc::utils::details
