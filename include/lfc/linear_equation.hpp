#pragma once

#include <type_traits>  // std::true/false_type
#include <utility>      // std::forward

#include "lfc/utils/reference_wrapper.hpp"  // UnwrapRefWrapper_t
#include "lfc/utils/type_traits.hpp"        // RepeatInto_t

namespace lfc {

/// Forward declaration of LinearEquation for the meta function below
template <class... Kn>
struct LinearEquation;

namespace details {

/// Returns true when T is an instanciation of a LinearEquation
template <class T>
struct IsLinearEquation : std::false_type {};

template <class... K>
struct IsLinearEquation<LinearEquation<K...>> : std::true_type {};

template <class T>
constexpr bool IsLinearEquation_v = IsLinearEquation<T>::value;

}  // namespace details

// Generic LinearEquation, solving (Kn[0] * X0) + (Kn[1] * X1) + ...
template <class... Kn>
struct LinearEquation {
  /// The size (number of K) of the LinearEquation
  static constexpr auto Size() -> std::size_t { return sizeof...(Kn); }

  using kn_t = std::tuple<Kn...>;

  kn_t kn; /*!< Kn coeffs multiplied to the inputs x */

  constexpr LinearEquation() = default;
  constexpr LinearEquation(const LinearEquation&) = default;
  constexpr LinearEquation(LinearEquation&&) = default;
  constexpr auto operator=(const LinearEquation&) -> LinearEquation& = default;
  constexpr auto operator=(LinearEquation&&) -> LinearEquation& = default;
  ~LinearEquation() = default;

  /// Construct by forwarding coeffs
  template <class... T>
  constexpr LinearEquation(T&&... values) : kn{std::forward<T>(values)...} {
    static_assert(sizeof...(T) == Size());
  }

  // TODO
  template <std::size_t N>
  using RepeatedTimes =
      utils::details::RepeatInto_t<LinearEquation<>, N, Kn...>;

  /**
   *  @return k<0> + ((k<1> * x[0]) + (k<2> * x[1]) + ... + (k<n + 1> * x[n]))
   *          with n being the number of x provided.
   *
   *  @note Set MultiplyRight to false in order to do
   *        ((x[0] * kn[0]) + (x[1] * kn[1]) + ... + (x[n] * kn[n])) instead, in
   *        case of non-commutative multiplication
   *
   *  @param[in] x... Unknown values used to solve the linear equation
   */
  template <bool MultiplyRight = true, class... X>
  constexpr auto Solve(X&&... x) const {
    static_assert(
        Size() >= (sizeof...(X)),
        "Not enought coefficients to solve this as a linear equation");

    if constexpr (MultiplyRight) {
      return SolveImpl(std::make_index_sequence<sizeof...(X)>{}, kn,
                       std::forward_as_tuple(std::forward<X>(x)...));
    } else {
      return SolveImpl(std::make_index_sequence<sizeof...(X)>{},
                       std::forward_as_tuple(std::forward<X>(x)...), kn);
    }
  }

  /// Access the Nth coefficient (lvalue)
  template <std::size_t N>
  constexpr auto k() & noexcept -> auto& {
    return std::get<N>(kn);
  }

  /// Access the Nth coefficient (const lvalue)
  template <std::size_t N>
  constexpr auto k() const& noexcept -> const auto& {
    return std::get<N>(kn);
  }

  /// Access the Nth coefficient (rvalue)
  template <std::size_t N>
  constexpr auto k() && noexcept -> auto&& {
    return std::get<N>(kn);
  }

  /// Access the Nth coefficient (const rvalue)
  template <std::size_t N>
  constexpr auto k() const&& noexcept -> const auto&& {
    return std::get<N>(kn);
  }

 private:
  /**
   *  @brief Implementation of Solve, effectively doing a transform/reduce over
   *         2 tuples, using I as indexes
   */
  template <std::size_t... I, class LhsTpl, class RhsTpl>
  static constexpr auto SolveImpl(std::index_sequence<I...>, LhsTpl&& lhs,
                                  RhsTpl&& rhs) {
    return (... + (std::get<I>(std::forward<LhsTpl>(lhs)) *
                   std::get<I>(std::forward<RhsTpl>(rhs))));
  }
};

/**
 *  @return LinearEquation<K, ...> owning the given coeffs
 *
 *  @param[in] k The coeffs defining the LinearEquation
 *
 *  @note This act exactly like std::make_tuple() in a sense that the returned
 *        LinearEquation<> will own its coeffiscients, effecively copying the
 *        args provided
 *  @note std::ref()/std::cref() (std::reference_wrapper) can be used in order
 *        to store references instead of values
 */
template <class... K>
constexpr auto MakeLinearEquation(K&&... k)
    -> LinearEquation<utils::details::UnwrapRefWrapper_t<std::decay_t<K>>...> {
  return {std::forward<K>(k)...};
}

/**
 *  @return LinearEquation<K&,...> of references to the given coeffs
 *
 *  @param[in] k... The coeffs lvalue used by the LinearEquation
 *
 *  @note This act like std::tie(), the returned LinearEquation<> doesn't own
 *        the values. Special care must be taken into account concerning
 *        their lifetime relative to the LinearEquation lifetime.
 *  @note If you only want a subset of reference, and not all values
 *        as references, use MakeLinearEquation() with std::cref()/std::ref()
 *        instead
 */
template <class... K>
constexpr auto TieAsLinearEquation(K&... k) -> LinearEquation<K&...> {
  return {k...};
}

/**
 *  @return LinearEquation<K&&, ...> of forwarding references
 *
 *  @param[in] k... The coeffs lvalue that will be forwarded inside the
 *                  LinearEquation
 *
 *  @note This act like std::forward_as_tuple()
 */
template <class... K>
constexpr auto ForwardAsLinearEquation(K&&... k) -> LinearEquation<K&&...> {
  return {std::forward<K>(k)...};
}

// Usefull Coeffs adapters //////////////////////////////////////////////////
/**
 * @brief Pseudo arithmetic tag object use to disable operator*() and forward
 *        anything through operator+().
 *
 * Can be used either when:
 * - Creating the equation direclty:
 *   MakeLinearEquation(NotUsed{}, K1, K2) -> Removes K0 completely;
 * - Solving and skiping some Kn:
 *   Solve(x1, NotUsed{}, x3) -> K0 + (K1 * x1) + (K3 * x3),
 *   (K2 * x2) is ignored
 */
struct Ignored_t {};
constexpr auto Ignored = Ignored_t{};

template <class T>
constexpr auto operator*(Ignored_t, T&&) noexcept -> Ignored_t {
  return Ignored;
}

template <class T>
constexpr auto operator*(T&&, Ignored_t) noexcept -> Ignored_t {
  return Ignored;
}

template <class T>
constexpr auto operator+(Ignored_t, T&& v) noexcept -> T&& {
  return std::forward<T>(v);
}

template <class T>
constexpr auto operator+(T&& v, Ignored_t) noexcept -> T&& {
  return std::forward<T>(v);
}

}  // namespace lfc
