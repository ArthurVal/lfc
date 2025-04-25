#pragma once

#include <type_traits>

namespace lfc::internal {

namespace details {

template <class, template <class...> class Trait, class... Args>
struct HasTraitImpl : std::false_type {};

template <template <class...> class Trait, class... Args>
struct HasTraitImpl<std::void_t<Trait<Args...>>, Trait, Args...>
    : std::true_type {};

} // namespace details

template <template <class...> class Trait, class... Args>
struct HasTrait : details::HasTraitImpl<void, Trait, Args...> {};

template <template <class...> class Trait, class... Args>
constexpr bool HasTrait_v = HasTrait<Trait, Args...>::value;

} // namespace lfc::internal
