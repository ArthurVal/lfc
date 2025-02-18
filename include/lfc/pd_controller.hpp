#pragma once

#include <utility>  // std::forward

#include "lfc/utils/type_traits.hpp"  // UnwrapRefWrapper_t

namespace lfc {

// Forward declaration for the details section below
template <class KP, class KD>
struct PDController;

namespace details {

// Meta function to determine if T is an instanciation of PDController<>
template <class T>
struct IsPDController : std::false_type {};

template <class... T>
struct IsPDController<PDController<T...>> : std::true_type {};

template <class T>
constexpr bool IsPDController_v = IsPDController<T>::value;

}  // namespace details

// Generic PD Problem representation, containing a KP/KD terms
template <class KP, class KD>
struct PDController {
  using kp_t = KP;
  using kd_t = KD;

  kp_t kp;
  kd_t kd;

  /**
   *  @return The result of ((kp * x) + (kd * dx))
   *  @param[in] x, dx State terms use to compute the PD output relative to the
   *                   PD parameters
   */
  template <class X, class DX>
  constexpr auto Solve(X&& x, DX&& dx) const -> decltype(auto) {
    return (kp * std::forward<X>(x)) + (kd * std::forward<DX>(dx));
  }
};

/**
 *  @brief Create a PD<KP,KD> from the given args
 *
 *  @note This act exactly like std::make_tuple() in a sense that the returned
 *        PD<> will own the KP/KD terms, effecively copying inputs provided
 *  @note std::ref()/std::cref() (std::reference_wrapper) can be used in order
 *        to store references instead of values
 *
 *  @param[in] kp, kd The value that will be used to construct the PD<>
 *  @return PD<KP, KD> Constructed by forwarding the arguments to it
 */
template <class KP, class KD>
constexpr auto MakePDController(KP&& kp, KD&& kd) noexcept
    -> PDController<details::UnwrapRefWrapper_t<std::decay_t<KP>>,
                    details::UnwrapRefWrapper_t<std::decay_t<KD>>> {
  return {std::forward<KP>(kp), std::forward<KD>(kd)};
}

/**
 *  @brief Create a PD<KP&,KD&> of references from the given args
 *
 *  @note This act like std::tie(), the returned PD<> doesn't own the
 *        KP/KD terms. Special care must be taken into account concerning the
 *        kp/kd lifetime relative to the PD created.
 *  @note If you want to have only one reference out of two, and not both terms
 *        as references, use MakePD() with std::cref()/std::ref() instead
 *
 *  @param[in] kp, kd Lvalues that will be used to construct the PD
 *  @return PD<KP&, KD&> Referencing the provided values
 */
template <class KP, class KD>
constexpr auto TieAsPDController(KP& kp,
                                 KD& kd) noexcept -> PDController<KP&, KD&> {
  return {kp, kd};
}

/**
 *  @brief Create a PD<KP&&,KD&&> of forwarding references from the given args
 *
 *  @note This act like std::forward_as_tuple()
 *
 *  @param[in] kp, kd rvalues/lvalues that will be used to construct the PD
 *  @return PD<KP&&, KD&&> Containing the provided lvalues or rvalues
 */
template <class KP, class KD>
constexpr auto ForwardAsPDController(KP&& kp, KD&& kd) noexcept
    -> PDController<KP&&, KD&&> {
  return {std::forward<KP>(kp), std::forward<KD>(kd)};
}

}  // namespace lfc
