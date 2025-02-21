#pragma once

#include <functional>  // std::invoke
#include <tuple>

namespace lfc::utils {

/**
 *  @brief Call f with the elements of tpl filtered by indexes I
 *
 *  @note Same as std::apply but can be called with a subset of elements
 *
 *  @param[in] f Callable taking all the Ith elements of tpl as arguments
 *  @param[in] tpl Tuple like object
 */
template <std::size_t... I, class F, class Tpl>
constexpr auto Apply(F&& f, Tpl&& tpl) noexcept -> decltype(auto) {
  if constexpr (sizeof...(I) == 0) {
    return std::apply(std::forward<F>(f), std::forward<Tpl>(tpl));
  } else {
    return std::invoke(std::forward<F>(f),
                       std::get<I>(std::forward<Tpl>(tpl))...);
  }
}

/**
 *  @brief Same as Apply(f, tpl) using an index_sequence helper as
 *         argument to automatically deduce indexes
 */
template <class F, class Tpl, std::size_t... I>
constexpr auto Apply(F&& f, Tpl&& tpl,
                     std::index_sequence<I...>) noexcept -> decltype(auto) {
  return Apply<I...>(std::forward<F>(f), std::forward<Tpl>(tpl));
}

/**
 *  @brief Call v for each elements contained within tpl
 *
 *  @note When the visitors takes an std::size_t as 2nd argument, the index of
 *        the element is forwarded with it
 *
 *  @note Silently ignore calling v when the visitor doesn't handle the given
 *        type (i.e. v(T{}) is not defined)
 *
 *  @param[in] v Visitor call for each elements
 *  @param[in] tpl Tuple like object
 */
template <class Visitor, class Tpl>
constexpr auto VisitTuple(Visitor&& v, Tpl&& tpl) noexcept -> void {
  /// Handle the call, based on v signature
  constexpr auto DoCall = [](auto&& f, auto&& value, std::size_t i) {
    if constexpr (std::is_invocable_v<decltype(f), decltype(value),
                                      std::size_t>) {
      std::invoke(std::forward<decltype(f)>(f),
                  std::forward<decltype(value)>(value), i);
    } else if constexpr (std::is_invocable_v<decltype(f), decltype(value)>) {
      std::invoke(std::forward<decltype(f)>(f),
                  std::forward<decltype(value)>(value));
    } else {
      // Type not handled
    }
  };

  Apply(
      [&](auto&&... values) -> void {
        std::size_t i = 0;
        (DoCall(v, std::forward<decltype(values)>(values), i++), ...);
      },
      std::forward<Tpl>(tpl));
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
  VisitTuple(
      [&init, &f](auto&& v) {
        init = std::invoke(f, init, std::forward<decltype(v)>(v));
      },
      std::forward<Tpl>(tpl));
  return init;
}

namespace details {

template <class T0, class T1, class... Tn>
constexpr auto min(T0&& t0, T1&& t1, Tn&&... others) -> decltype(auto) {
  if constexpr (sizeof...(Tn) == 0) {
    return t0 < t1 ? std::forward<T0>(t0) : std::forward<T1>(t1);
  } else {
    if (t0 < t1) {
      return min(std::forward<T0>(t0), std::forward<Tn>(others)...);
    } else {
      return min(std::forward<T1>(t1), std::forward<Tn>(others)...);
    }
  }
}

template <std::size_t I, class F, class Tpls>
constexpr auto TransformTuplesImpl(F&& f, Tpls&& tpls) {
  // Since we can't expand 2 packs at once (unless they have the same size), we
  // must use a trick in order to have only ONE pack active at time.
  //
  // Here, the tuple pack is forwarded as tuple<tpl ...> and the a SINGLE index
  // is forwarded to this function.
  // Hence we can unfold the tuple pack for a given index I...

  return std::apply(
      [&f](auto&&... tpl) {
        return std::invoke(f, std::get<I>(std::forward<decltype(tpl)>(tpl))...);
      },
      std::forward<Tpls>(tpls));
}

template <class F, class Tpls, std::size_t... I>
constexpr auto TransformTuplesImpl(F&& f, Tpls&& tpls,
                                   std::index_sequence<I...>) {
  // ... And unfold the indexes here
  return std::make_tuple(TransformTuplesImpl<I>(f, tpls)...);
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
      std::forward<F>(f),
      std::forward_as_tuple(std::forward<TplLikes>(tpls)...),
      std::make_index_sequence<details::min(std::tuple_size_v<TplLikes>...)>{});
}

/**
 *  @brief Apply Transform on the input tuple, then Reduce.
 *
 *  Correspond to the following:
 *  Tuple A  Tuple B  ...  Tuple M         TRANSFORM         Tuple R (size N)
 *   [A_1]    [B_1]   ...   [M_1]  -> t(A_1, B_1, ..., M_1) -> [R_1]
 *   [A_2]    [B_2]   ...   [M_2]  -> t(A_2, B_2, ..., M_2) -> [R_2]
 *    ...      ...    ...    ...   -> t(..., ..., ..., ...) ->  ...
 *   [A_N]    [B_N]   ...   [M_N]  -> t(A_N, B_N, ..., M_N) -> [R_N]
 *   [A_N+1]  [B_N+1] ...
 *    ...      ...                                               |
 *    ...      ...                                               V
 *                                                |  init = r(init, [R_1])
 *                                         REDUCE |  init = r(init, [R_2])
 *                                                |           ...
 *                                                |  init = r(init, [R_N])
 *                                                               |
 *                                                               V
 *                                                             init
 *
 *  With N being the size of the smallest tuple (in this example M).
 *
 *  @param[in] init The initial value containing the reduced value
 *  @param[in] r The reduction operator
 *  @param[in] t The transform operator
 *  @param[in] tpls... All tuples we wish to transform
 *
 *  @return T Result of the reduction operation, follwing the transformation
 */
template <class T, class ReduceOp, class TransformOp, class... TplLikes>
constexpr auto TransformReduceTuples(T init, ReduceOp&& r, TransformOp&& t,
                                     TplLikes&&... tpls) {
  return ReduceTuple(std::forward<ReduceOp>(r),
                     TransformTuples(std::forward<TransformOp>(t),
                                     std::forward<TplLikes>(tpls)...),
                     std::move(init));
}

}  // namespace lfc::utils
