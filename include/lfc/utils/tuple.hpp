#pragma once

#include <tuple>

#include "lfc/utils/interger_sequence.hpp"  // MakeIndexSequenceStartingAt

namespace lfc::utils {

/**
 *  @return tuple<> Containing only forwarding references to the input tuple
 *                  elements
 */
template <class Tpl>
constexpr auto ForwardTuple(Tpl&& tpl) noexcept {
  return std::apply(
      [](auto&&... elems) {
        return std::forward_as_tuple(std::forward<decltype(elems)>(elems)...);
      },
      std::forward<Tpl>(tpl));
}

/**
 *  @return std::tuple<...> Sub tuple containing all the Ith elements from the
 *          given input tpl
 *
 *  @param[in] tpl Tuple like object we wish to sub tuple from
 */
template <std::size_t... I, class Tpl>
constexpr auto MakeSubTuple(Tpl&& tpl) noexcept
    -> std::tuple<std::tuple_element_t<I, std::decay_t<Tpl>>...> {
  return {std::get<I>(std::forward<Tpl>(tpl))...};
}

/**
 *  @brief Same as MakeSubTuple(tpl) using an index_sequence helper as
 * argument to automatically deduce indexes
 */
template <std::size_t... I, class Tpl>
constexpr auto MakeSubTuple(std::index_sequence<I...>, Tpl&& tpl) noexcept {
  return MakeSubTuple<I...>(std::forward<Tpl>(tpl));
}

/**
 *  @return std::tuple<...>  Corresponding to a slice [Begin, Begin + Size) of
 *          the given input tuple
 *
 *  @tparam Begin First index (included) of the slice
 *  @tparam Size Size of the slice
 *
 *  @important (Begin + Size) MUST be smaller or equal to the input tuple size
 *
 *  @param[in] tpl Tuple like object we wish to slice
 */
template <std::size_t Begin, std::size_t Size, class Tpl>
constexpr auto SliceTuple(Tpl&& tpl) noexcept {
  constexpr std::size_t TupleSize = std::tuple_size_v<std::decay_t<Tpl>>;
  static_assert(Begin + Size <= TupleSize);

  return MakeSubTuple(MakeIndexSequenceStartingAt<Begin, Size>{},
                      std::forward<Tpl>(tpl));
}

/**
 *  @brief Specialization of SliceTuple() taking only the beginning of the
 *         slice, up until the end of the input tuple
 */
template <std::size_t Begin, class Tpl>
constexpr auto SliceTuple(Tpl&& tpl) noexcept {
  constexpr std::size_t TupleSize = std::tuple_size_v<std::decay_t<Tpl>>;
  static_assert(Begin < TupleSize);

  return SliceTuple<Begin, (TupleSize - Begin)>(std::forward<Tpl>(tpl));
}

template <class T, class Tpl, class BinaryOp>
constexpr auto ReduceTuple(Tpl&& tpl, T init, BinaryOp&& f) -> T {
  return std::apply(
      [&](auto&&... v) {
        return ((init = f(init, std::forward<decltype(v)>(v))), ...);
      },
      std::forward<Tpl>(tpl));
}

namespace details {

template <std::size_t... I, class BinaryOp, class LhsTpl, class RhsTpl>
constexpr auto TransformTuplesImpl(std::index_sequence<I...>, LhsTpl&& lhs,
                                   RhsTpl&& rhs, BinaryOp&& f) {
  return std::make_tuple(f(std::get<I>(std::forward<LhsTpl>(lhs)),
                           std::get<I>(std::forward<RhsTpl>(rhs)))...);
}

}  // namespace details

template <class LhsTpl, class RhsTpl, class BinaryOp>
constexpr auto TransformTuples(LhsTpl&& lhs, RhsTpl&& rhs, BinaryOp&& f) {
  constexpr auto LhsSize = std::tuple_size_v<std::decay_t<LhsTpl>>;
  constexpr auto RhsSize = std::tuple_size_v<std::decay_t<RhsTpl>>;

  // Take the min size
  constexpr auto MinSize = LhsSize < RhsSize ? LhsSize : RhsSize;

  return details::TransformTuplesImpl(
      std::make_index_sequence<MinSize>{}, std::forward<LhsTpl>(lhs),
      std::forward<RhsTpl>(rhs), std::forward<BinaryOp>(f));
}

}  // namespace lfc::utils
