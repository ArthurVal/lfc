#pragma once

#include <utility>  // interger_sequence

namespace lfc::utils {

namespace details {

/// Create a new integer_sequence offseted by N
template <class T, T Offset, class IntSeq>
struct OffsetIntegerSequenceBy;

template <class T, T Offset, T... Ints>
struct OffsetIntegerSequenceBy<T, Offset, std::integer_sequence<T, Ints...>> {
  using type = std::integer_sequence<T, (Ints + Offset)...>;
};

template <class T, T Offset, class IntSeq>
using OffsetIntegerSequenceBy_t =
    typename OffsetIntegerSequenceBy<T, Offset, IntSeq>::type;

}  // namespace details

/**
 *  @return An std::integer_sequence<> representing a continous range of Size
 *          ints T, starting at Begin
 */
template <class T, T Begin, T Size>
constexpr auto SliceOfInts() {
  return details::OffsetIntegerSequenceBy_t<
      T, Begin, std::make_integer_sequence<T, Size>>{};
}

/**
 *  @return An std::index_sequence<> representing a continous range of
 *          Size std::size_t, starting at Begin
 */
template <std::size_t Begin, std::size_t Size>
constexpr auto SliceOfIndex() {
  return SliceOfInts<std::size_t, Begin, Size>();
}

}  // namespace lfc::utils
