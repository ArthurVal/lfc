#pragma once

#include <tuple>
#include <utility>  // std::forward

#include "lfc/utils/reference_wrapper.hpp"  // UnwrapRefWrapper_t

namespace lfc {

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
struct Ignored {};

template <class T>
constexpr auto operator*(Ignored, T&&) noexcept -> Ignored {
  return Ignored{};
}

template <class T>
constexpr auto operator*(T&&, Ignored) noexcept -> Ignored {
  return Ignored{};
}

template <class T>
constexpr auto operator+(Ignored, T&& v) noexcept -> T&& {
  return std::forward<T>(v);
}

template <class T>
constexpr auto operator+(T&& v, Ignored) noexcept -> T&& {
  return std::forward<T>(v);
}

/// Forward declaration of LinearEquation for the meta function below
template <class K0, class... Kn>
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

// Generic LinearEquation, solving K0 + (Kn[0] * X0) + (Kn[1] * X1) + ...
template <class K0, class... Kn>
struct LinearEquation {
  constexpr LinearEquation() = default;
  constexpr LinearEquation(const LinearEquation&) = default;
  constexpr LinearEquation(LinearEquation&&) = default;
  constexpr auto operator=(const LinearEquation&) -> LinearEquation& = default;
  constexpr auto operator=(LinearEquation&&) -> LinearEquation& = default;
  ~LinearEquation() = default;

  /// Construct by forwarding coeffs
  template <class _K0, class... _Kn>
  constexpr LinearEquation(_K0&& _k0, _Kn&&... _kn)
      : m_k0(std::forward<_K0>(_k0)), m_kn{std::forward<_Kn>(_kn)...} {}

  /// Return a tuple of every coefficients of the given Equation
  template <
      class AnyEq,
      std::enable_if_t<std::is_same_v<std::decay_t<AnyEq>, LinearEquation>,
                       bool> = true>
  friend constexpr auto AsTuple(AnyEq&& eq) noexcept {
    return std::apply(
        [&eq](auto&&... kn) {
          return std::forward_as_tuple(std::forward<AnyEq>(eq).m_k0,
                                       std::forward<decltype(kn)>(kn)...);
        },
        std::forward<AnyEq>(eq).m_kn);
  }

  /// Access the Nth coefficient (lvalue)
  template <std::size_t N>
  constexpr auto k() & noexcept -> auto& {
    return std::get<N>(AsTuple(*this));
  }

  /// Access the Nth coefficient (const lvalue)
  template <std::size_t N>
  constexpr auto k() const& noexcept -> const auto& {
    return std::get<N>(AsTuple(*this));
  }

  /// Access the Nth coefficient (rvalue)
  template <std::size_t N>
  constexpr auto k() && noexcept -> auto&& {
    return std::get<N>(AsTuple(*this));
  }

  /// Access the Nth coefficient (const rvalue)
  template <std::size_t N>
  constexpr auto k() const&& noexcept -> const auto&& {
    return std::get<N>(AsTuple(*this));
  }

  /**
   *  @brief Iterate over all coefficients K of any LinearEquation eq
   *
   *  @param[in] eq LinearEquation we wish to iterates over
   *  @param[in] op Functor called for each coefficients 'k' of type K, if
   *                'op(k)' is defined. Additionally, if 'op(k, std::size_t)' is
   *                defined, called it with the index of k as second argument.
   */
  template <
      class AnyEq, class Op,
      std::enable_if_t<std::is_same_v<std::decay_t<AnyEq>, LinearEquation>,
                       bool> = true>
  friend constexpr auto ForEachCoeffsOf(AnyEq&& eq, Op&& op) -> void {
    constexpr auto DoCall = [](auto&& f, auto&& k, std::size_t i) {
      if constexpr (std::is_invocable_v<decltype(f), decltype(k),
                                        std::size_t>) {
        f(std::forward<decltype(k)>(k), i);
      } else if constexpr (std::is_invocable_v<decltype(f), decltype(k)>) {
        f(std::forward<decltype(k)>(k));
      } else {
        // Type of k not handled
      }
    };

    std::apply(
        [&](auto&&... k) {
          std::size_t i = 0;
          (DoCall(op, std::forward<decltype(k)>(k), i++), ...);
        },
        AsTuple(std::forward<AnyEq>(eq)));
  }

  /// The size (number of K) of the LinearEquation
  static constexpr auto Size() -> std::size_t { return 1 + sizeof...(Kn); }

  /**
   *  @return k0 + ((kn[0] * x[0]) + (kn[1] * x[1]) + ... + (kn[n] * x[n])) with
   *          n being the number of x provided.
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
        (sizeof...(Kn)) >= (sizeof...(X)),
        "Not enought coefficients to solve this as a linear equation");

    if constexpr (MultiplyRight) {
      return m_k0 + SolveImpl(std::make_index_sequence<sizeof...(X)>{}, m_kn,
                              std::forward_as_tuple(std::forward<X>(x)...));
    } else {
      return m_k0 + SolveImpl(std::make_index_sequence<sizeof...(X)>{},
                              std::forward_as_tuple(std::forward<X>(x)...),
                              m_kn);
    }
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

  K0 m_k0;                /*!< K0 constant coeff */
  std::tuple<Kn...> m_kn; /*!< Kn coeffs multiplied to the inputs x */
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

}  // namespace lfc
