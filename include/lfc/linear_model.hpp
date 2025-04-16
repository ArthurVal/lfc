#pragma once

#include <type_traits>
#include <utility>

#include "lfc/utils/reference_wrapper.hpp"

namespace lfc {

template <class _Gains, class _Offset = void>
struct LinearModel {
  using gains_t = _Gains;
  gains_t gains;

  using offset_t = _Offset;
  offset_t offset;
};

template <class _Gains>
struct LinearModel<_Gains, void> {
  using gains_t = _Gains;
  gains_t gains;

  using offset_t = void;
};

namespace details {

template <class T>
struct IsLinearModel : std::false_type {};

template <class G, class O>
struct IsLinearModel<LinearModel<G, O>> : std::true_type {};

template <class T>
constexpr bool IsLinearModel_v = IsLinearModel<T>::value;

template <class T>
using EnableIfLinearModel = std::enable_if_t<IsLinearModel_v<T>, bool>;

}  // namespace details

template <class GainsType, class OffsetType>
constexpr auto MakeLinearModel(GainsType&& gains, OffsetType&& offset)
    -> LinearModel<utils::UnwrapRefWrapper_t<std::decay_t<GainsType>>,
                   utils::UnwrapRefWrapper_t<std::decay_t<OffsetType>>> {
  return {std::forward<GainsType>(gains), std::forward<OffsetType>(offset)};
}

template <class GainsType>
constexpr auto MakeLinearModel(GainsType&& gains)
    -> LinearModel<utils::UnwrapRefWrapper_t<std::decay_t<GainsType>>> {
  return {std::forward<GainsType>(gains)};
}

template <class GainsType, class OffsetType>
constexpr auto TieAsLinearModel(GainsType& gains, OffsetType& offset)
    -> LinearModel<GainsType&, OffsetType&> {
  return {gains, offset};
}

template <class GainsType>
constexpr auto TieAsLinearModel(GainsType& gains) -> LinearModel<GainsType&> {
  return {gains};
}

template <class GainsType, class OffsetType>
constexpr auto ForwardAsLinearModel(GainsType&& gains, OffsetType&& offset)
    -> LinearModel<GainsType&&, OffsetType&&> {
  return {std::forward<GainsType>(gains), std::forward<OffsetType>(offset)};
}

template <class GainsType>
constexpr auto ForwardAsLinearModel(GainsType&& gains)
    -> LinearModel<GainsType&&> {
  return {std::forward<GainsType>(gains)};
}

template <class Model, details::EnableIfLinearModel<Model> = true>
constexpr auto HasOffset() -> bool {
  return !std::is_void_v<typename Model::offset_t>;
}

template <class Model, details::EnableIfLinearModel<std::decay_t<Model>> = true>
constexpr auto HasOffset(Model&&) -> bool {
  return HasOffset<std::decay_t<Model>>();
}

template <class Model, class X,
          details::EnableIfLinearModel<std::decay_t<Model>> = true>
constexpr auto Solve(Model&& model, X&& x) -> decltype(auto) {
  if constexpr (HasOffset<std::decay_t<Model>>()) {
    return model.offset + (model.gains * std::forward<X>(x));
  } else {
    return std::forward<Model>(model).gains * std::forward<X>(x);
  }
}

}  // namespace lfc
