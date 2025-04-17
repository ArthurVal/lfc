#pragma once

#include <type_traits>
#include <utility>

#include "internal/reference_wrapper.hpp"

namespace lfc {

/**
 *  \brief Represents a LinearModel solving `Y = Offset + (Coeffs * X)`
 *
 *  \tparam _Coefficients Type of the underlying coefficients, directly
 *                        multiplied to X
 *  \tparam _Offset Optional offset type. If set to void (by default), doesn't
 *                  old any offset values
 */
template <class _Coefficients, class _Offset = void>
struct LinearModel {
  using coeffs_t = _Coefficients;
  coeffs_t coeffs;

  using offset_t = _Offset;
  offset_t offset;
};

/// Specialisation disabling offset when _Offset is set to void
template <class _Coefficients>
struct LinearModel<_Coefficients, void> {
  using coeffs_t = _Coefficients;
  coeffs_t coeffs;

  using offset_t = void;
};

namespace details {

/// Meta function checking that T is an implementation of LinearModel<>
template <class T>
struct IsLinearModel : std::false_type {};

template <class G, class O>
struct IsLinearModel<LinearModel<G, O>> : std::true_type {};

template <class T>
constexpr bool IsLinearModel_v = IsLinearModel<T>::value;

/// Alias for std::enable_if_t<IsLinearModel_v<T>, bool>
template <class T>
using EnableIfLinearModel = std::enable_if_t<IsLinearModel_v<T>, bool>;

}  // namespace details

/**
 *  \return A LinearModel that owns its underlying coeffs/offsets.
 *
 *  \note Any std::reference_wrapper<T> (using std::ref() or std::cref()) will
 *        be unwrapped and transforms into its underlying references T&. This
 *        can be used to have partial references, instead of having all member
 *        as references when using TieAs.
 *
 *  \param[in] coeffs The coefficients of the linear model
 *  \param[in] offset The offset of the linear model
 */
template <class CoeffsType, class OffsetType>
constexpr auto MakeLinearModel(CoeffsType&& coeffs, OffsetType&& offset)
    -> LinearModel<internal::UnwrapRefWrapper_t<std::decay_t<CoeffsType>>,
                   internal::UnwrapRefWrapper_t<std::decay_t<OffsetType>>> {
  return {std::forward<CoeffsType>(coeffs), std::forward<OffsetType>(offset)};
}

/// Specialisation disabling offset
template <class CoeffsType>
constexpr auto MakeLinearModel(CoeffsType&& coeffs)
    -> LinearModel<internal::UnwrapRefWrapper_t<std::decay_t<CoeffsType>>> {
  return {std::forward<CoeffsType>(coeffs)};
}

/**
 *  \return A LinearModel with only references as coeffs/offset
 *
 *  \note If you only want one element as ref, use MakeLinearModel with
 *        std::ref/std::cref instead
 *
 *  \param[in] coeffs Reference to the coefficients of the linear model
 *  \param[in] offset Reference to the offset of the linear model
 */
template <class CoeffsType, class OffsetType>
constexpr auto TieAsLinearModel(CoeffsType& coeffs, OffsetType& offset)
    -> LinearModel<CoeffsType&, OffsetType&> {
  return {coeffs, offset};
}

/// Specialisation disabling offset
template <class CoeffsType>
constexpr auto TieAsLinearModel(CoeffsType& coeffs)
    -> LinearModel<CoeffsType&> {
  return {coeffs};
}

/**
 *  \return A LinearModel forwarding its coeffs/offset
 *
 *  \param[in] coeffs Forward reference of the coefficients
 *  \param[in] offset Forward reference of the offset
 */
template <class CoeffsType, class OffsetType>
constexpr auto ForwardAsLinearModel(CoeffsType&& coeffs, OffsetType&& offset)
    -> LinearModel<CoeffsType&&, OffsetType&&> {
  return {std::forward<CoeffsType>(coeffs), std::forward<OffsetType>(offset)};
}

/// Specialisation disabling offset
template <class CoeffsType>
constexpr auto ForwardAsLinearModel(CoeffsType&& coeffs)
    -> LinearModel<CoeffsType&&> {
  return {std::forward<CoeffsType>(coeffs)};
}

/**
 *  \return True when the underlying LinearModel<> has its offset member
 */
template <class Model, details::EnableIfLinearModel<Model> = true>
constexpr auto HasOffset() -> bool {
  return !std::is_void_v<typename Model::offset_t>;
}

/**
 *  \return True when the underlying LinearModel<> has its offset member
 */
template <class Model, details::EnableIfLinearModel<std::decay_t<Model>> = true>
constexpr auto HasOffset(Model&&) -> bool {
  return HasOffset<std::decay_t<Model>>();
}

/**
 *  \return The result of (offset + (coeffs * x)) or (coeffs * x) if the model
 *          doesn't have any offsets
 *
 *  \param[in] model Any valid LinearModel<>
 *  \param[in] x Any value X that can be multiplied by the model's coeffs
 */
template <class Model, class X,
          details::EnableIfLinearModel<std::decay_t<Model>> = true>
constexpr auto Solve(Model&& model, X&& x) {
  if constexpr (HasOffset<std::decay_t<Model>>()) {
    return std::forward<Model>(model).offset +
           (std::forward<Model>(model).coeffs * std::forward<X>(x));
  } else {
    return std::forward<Model>(model).coeffs * std::forward<X>(x);
  }
}

}  // namespace lfc
