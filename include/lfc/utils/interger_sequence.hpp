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

template <class T, T Begin, T Size>
using MakeIntegerSequenceStartingAt =
    details::OffsetIntegerSequenceBy_t<T, Begin,
                                       std::make_integer_sequence<T, Size>>;

template <std::size_t Begin, std::size_t Size>
using MakeIndexSequenceStartingAt =
    MakeIntegerSequenceStartingAt<std::size_t, Begin, Size>;

}  // namespace lfc::utils
