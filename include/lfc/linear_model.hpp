#pragma once

#include <cassert>
#include <optional>
#include <type_traits>
#include <utility>

#include "internal/reference_wrapper.hpp"
#include "internal/traits_has_accepts.hpp"
#include "internal/traits_has_is_valid.hpp"

namespace lfc {

/**
 *  \brief Represents a LinearModel solving `Y = Offset + (Coeffs * X)`
 *
 *  \tparam _Coefficients Type of the underlying coefficients, directly
 *                        multiplied to X
 *  \tparam _Offset Optional offset type. If set to void (by default), doesn't
 *                  old any offset values
 *
 *  Users can provided additionnal safety checks when calling Solve() by
 *  providing either `IsValid(_Coefficients, _Offset) -> bool` and/or
 *  `Accepts(_Coefficients, T) -> bool` free functions.
 *  If those functions exists, they may be used through assertions/checks and
 *  will be called when calling the equivalent
 * `IsValid(LinearModel) -> bool` / `Accepts(LinearModel, X) -> bool`.
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

/**
 *  \brief Traits for LinearModel.
 *
 *  Generic Traits struct, containing meta informations on what the linear model
 *  is capable of.
 *
 *  If T is not an implementation of LinearModel<>, LinearModelTraits::value is
 *  set to false.
 *  Otherwise, LinearModelTraits::value is set to true, and the underlying
 *  traits define static compile-time functions use to help determine if some
 *  functions are defnined or not.
 */
template <class T>
struct LinearModelTraits : std::false_type {};

/// Implementation when T is a LinearModel<>
template <class _Coeffs, class _Offset>
struct LinearModelTraits<LinearModel<_Coeffs, _Offset>> : std::true_type {
  using coeffs_t = _Coeffs;
  using offset_t = _Offset;

  /// Returns True when the LinearModel has an offset member.
  static constexpr bool HasOffset() { return !std::is_void_v<offset_t>; }

  /// Returns True when IsValid(coeffs, offset) (IsValid(coeffs) when HasOffset
  /// is false) function is defined an returns something convertible to bool
  static constexpr bool HasIsValid() {
    if constexpr (HasOffset()) {
      return internal::HasIsValidFreeFunction_v<coeffs_t, offset_t>;
    } else {
      return internal::HasIsValidFreeFunction_v<coeffs_t>;
    }
  }

  /// Returns True when Accepts(coeffs, X) function is defined an returns
  /// something convertible to bool
  template <class X>
  static constexpr bool HasAccepts() {
    return internal::HasAcceptsFreeFunction_v<coeffs_t, X>;
  }
};

/// Meta variable returning TRUE when T is a LinearModel<>
template <class T>
constexpr bool IsLinearModel_v = LinearModelTraits<T>::value;

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
 *  \return True when the given model is considered valid. False otherwise.
 *
 *  This function ALWAYS returns true by default, unless you define a free
 *  function `IsValid(Coeffs, Offset)` (or `IsValid(Coeffs)` when the Model
 *  doesn't have any offsets). In this case, it returns the value of the
 *  provided function.
 *
 *  IsValid() is intended to let user provide pre-checks on the validity of the
 *  LinearModel internally, e.g. if the coeffs/offsets sizes match in case of
 *  matrix/vectors etc...
 */
template <class Model, class...,
          class ModelTraits = LinearModelTraits<std::decay_t<Model>>,
          std::enable_if_t<ModelTraits::value, bool> = true>
constexpr auto IsValid(Model&& m) -> bool {
  if constexpr (ModelTraits::HasIsValid()) {
    if constexpr (ModelTraits::HasOffset()) {
      return IsValid(std::forward<Model>(m).coeffs,
                     std::forward<Model>(m).offset);
    } else {
      return IsValid(std::forward<Model>(m).coeffs);
    }
  } else {
    return true;
  }
}

/**
 *  \return True when the given model accepts X. False otherwise.
 *
 *  This function ALWAYS returns true by default, unless you define a free
 *  function `Accepts(Coeffs, X)`. In this case, it returns the value of the
 *  provided function.
 *
 *  Accepts() is intended to let user provide pre-checks on the validity of the
 *  LinearModel relative to the inputs provided, e.g. if the coeffs size matches
 *  with the input size etc...
 */
template <class Model, class X, class...,
          class ModelTraits = LinearModelTraits<std::decay_t<Model>>,
          std::enable_if_t<ModelTraits::value, bool> = true>
constexpr auto Accepts(Model&& m, X&& x) -> bool {
  if constexpr (ModelTraits::template HasAccepts<X>()) {
    return Accepts(std::forward<Model>(m).coeffs, std::forward<X>(x));
  } else {
    return true;
  }
}

/**
 *  \return The result of (offset + (coeffs * x)) or (coeffs * x) if the model
 *          doesn't have any offsets
 *
 *  \param[in] m Any valid LinearModel<>
 *  \param[in] x Any value X that can be multiplied by the model's coeffs
 *
 *  \pre IsValid(m) returns true
 *  \pre Accepts(m, x) returns true
 */
template <class Model, class X, class...,
          class ModelTraits = LinearModelTraits<std::decay_t<Model>>,
          std::enable_if_t<ModelTraits::value, bool> = true>
constexpr auto Solve(Model&& m, X&& x) {
  assert(IsValid(m) &&
         "Model is not valid. Some parameters may be wrongly set internally.");

  assert(Accepts(m, x) && "Model doesn't accept the given state X.");

  if constexpr (ModelTraits::HasOffset()) {
    return std::forward<Model>(m).offset +
           (std::forward<Model>(m).coeffs * std::forward<X>(x));
  } else {
    return std::forward<Model>(m).coeffs * std::forward<X>(x);
  }
}

/**
 *  \return The result of Solve() when IsValid() and Accepts() returns true,
 *          std::nullopt otherwise.
 *
 *  \param[in] m Any valid LinearModel<>
 *  \param[in] x Any value X that can be multiplied by the model's coeffs
 */
template <class Model, class X, class...,
          class ModelTraits = LinearModelTraits<std::decay_t<Model>>,
          std::enable_if_t<ModelTraits::value, bool> = true>
constexpr auto TryToSolve(Model&& model, X&& x)
    -> std::optional<decltype(Solve(std::forward<Model>(model),
                                    std::forward<X>(x)))> {
  if (IsValid(model) && Accepts(model, x)) {
    return Solve(std::forward<Model>(model), std::forward<X>(x));
  } else {
    return std::nullopt;
  }
}

}  // namespace lfc
