#pragma once

#include <functional>
#include <tuple>

#include "lfc/utils/interger_sequence.hpp"  // MakeIndexSequenceStartingAt

namespace lfc::utils {

/**
 *  @return tuple<> Containing only forwarding references to the input tuple
 *                  elements
 */
template <class TplLike>
constexpr auto ForwardTuple(TplLike&& tpl) noexcept {
  return std::apply(
      [](auto&&... elems) {
        return std::forward_as_tuple(std::forward<decltype(elems)>(elems)...);
      },
      std::forward<TplLike>(tpl));
}

/**
 *  @return std::tuple<...> Sub tuple containing all the Ith elements from the
 *          given input tpl
 *
 *  @param[in] tpl Tuple like object we wish to sub tuple from
 */
template <std::size_t... I, class TplLike>
constexpr auto MakeSubTuple(TplLike&& tpl) noexcept
    -> std::tuple<std::tuple_element_t<I, std::decay_t<TplLike>>...> {
  return {std::get<I>(std::forward<TplLike>(tpl))...};
}

/**
 *  @brief Same as MakeSubTuple(tpl) using an index_sequence helper as
 * argument to automatically deduce indexes
 */
template <std::size_t... I, class TplLike>
constexpr auto MakeSubTuple(std::index_sequence<I...>, TplLike&& tpl) noexcept {
  return MakeSubTuple<I...>(std::forward<TplLike>(tpl));
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
template <std::size_t Begin, std::size_t Size, class TplLike>
constexpr auto SliceTuple(TplLike&& tpl) noexcept {
  constexpr std::size_t TupleSize = std::tuple_size_v<std::decay_t<TplLike>>;
  static_assert(Begin + Size <= TupleSize);

  return MakeSubTuple(MakeIndexSequenceStartingAt<Begin, Size>{},
                      std::forward<TplLike>(tpl));
}

/**
 *  @brief Specialization of SliceTuple() taking only the beginning of the
 *         slice, up until the end of the input tuple
 */
template <std::size_t Begin, class TplLike>
constexpr auto SliceTuple(TplLike&& tpl) noexcept {
  constexpr std::size_t TupleSize = std::tuple_size_v<std::decay_t<TplLike>>;
  static_assert(Begin < TupleSize);

  return SliceTuple<Begin, (TupleSize - Begin)>(std::forward<TplLike>(tpl));
}

}  // namespace lfc::utils
