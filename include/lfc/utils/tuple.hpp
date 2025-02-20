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

/**
 *  @brief Reduce the given tuple, applying init = f(init, e) for each elements
 *         'e' of the given input tuple
 *
 *  @param[in] f A binary operator called with the following signature:
 *               f(T, tuple_element_t<I>) -> T for each elements I of the tuple;
 *  @param[in] tpl The tuple we wish to reduce
 *  @param[in] init The initial value containing the reduced value
 *
 *  @return T Result of the reduction operation
 */
template <class T, class Tpl, class BinaryOp>
constexpr auto ReduceTuple(BinaryOp&& f, Tpl&& tpl, T init = T{}) -> T {
  return std::apply(
      [&](auto&&... v) {
        return ((init = f(init, std::forward<decltype(v)>(v))), ...);
      },
      std::forward<Tpl>(tpl));
}

namespace details {

template <class T>
constexpr auto min(T&& t) -> decltype(auto) {
  return std::forward<T>(t);
}

template <class T0, class T1, class... Tn>
constexpr auto min(T0&& t0, T1&& t1, Tn&&... others) -> decltype(auto) {
  if (t0 < t1) {
    return min(std::forward<T0>(t0), std::forward<Tn>(others)...);
  } else {
    return min(std::forward<T1>(t1), std::forward<Tn>(others)...);
  }
}

template <std::size_t I, class F, class Tpls>
constexpr auto CallAtIndex(F&& f, Tpls&& tpls) {
  // Since we can't expand 2 packs at once (unless they have the same size), we
  // must use a trick in order to have only ONE pack active at time.
  //
  // Here, the tuple pack is forwarded as tuple<tpl ...> and the a SINGLE index
  // is forwarded to this function.
  // Hence we can unfold the tuple pack for a given index I...

  return std::apply(
      [&f](auto&&... tpl) {
        return f(std::get<I>(std::forward<decltype(tpl)>(tpl))...);
      },
      std::forward<Tpls>(tpls));
}

template <std::size_t... I, class F, class Tpls>
constexpr auto TransformTuplesImpl(std::index_sequence<I...>, F&& f,
                                   Tpls&& tpls) {
  // ... And unfold the indexes here
  return std::make_tuple(CallAtIndex<I>(f, tpls)...);
}

}  // namespace details

/**
 *  @brief Create a new tuple containing the tranformation F(...) called by
 *         iterating in parallele over all input tuples
 *
 *  Correspond to the following:
 *  Tuple A  Tuple B  ...  Tuple M                             Result (size N)
 *   [A_1]    [B_1]   ...   [M_1]  -> f(A_1, B_1, ..., M_1) -> [R_1]
 *   [A_2]    [B_2]   ...   [M_2]  -> f(A_2, B_2, ..., M_2) -> [R_2]
 *    ...      ...    ...    ...   -> f(..., ..., ..., ...) ->  ...
 *   [A_N]    [B_N]   ...   [M_N]  -> f(A_N, B_N, ..., M_N) -> [R_N]
 *   [A_N+1]  [B_N+1] ...
 *    ...      ...
 *
 *  With N being the size of the smallest tuple (in this example M).
 *

 *  @param[in] f An operator called with each tuples elements
 *  @param[in] tpls... All tuples we wish to transform
 *
 *  @return std::tuple<...> Containing the result of each function call. Size
 *          matching the smallest input tuple.
 */
template <class F, class... TplLikes>
constexpr auto TransformTuples(F&& f, TplLikes&&... tpls) {
  return details::TransformTuplesImpl(
      std::make_index_sequence<details::min(std::tuple_size_v<TplLikes>...)>{},
      std::forward<F>(f),
      std::forward_as_tuple(std::forward<TplLikes>(tpls)...));
}
}  // namespace lfc::utils
