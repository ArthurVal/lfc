#pragma once

#include "lfc/internal/has_trait.hpp"
#include "macros.h"

#include <array>
#include <tuple>
#include <type_traits>

#include "rclcpp/node.hpp"

namespace lfc::ros {

namespace details {

template <class T>
using DeclareParamIntoFreeFunction = decltype(DeclareParamInto(
    std::declval<rclcpp::Node &>(), std::declval<T>()));

}

template <class AnyParam, class... Others>
auto DeclareParams(rclcpp::Node &node, AnyParam &&param, Others &&...others) {

  static_assert(
      (internal::HasTrait_v<details::DeclareParamIntoFreeFunction, AnyParam> &&
       ... &&
       internal::HasTrait_v<details::DeclareParamIntoFreeFunction, Others>),
      "All Params given are expected to declare the following interface:"
      "\n - 'DeclareParamInto(Node&, Param) -> T'");

  if constexpr (sizeof...(Others) == 0) {
    return DeclareParamInto(node, FWD(param));
  } else {
    if constexpr ((std::is_same_v<decltype(DeclareParamInto(node, FWD(param))),
                                  decltype(DeclareParamInto(node,
                                                            FWD(others)))> &&
                   ...)) {
      return std::array{DeclareParamInto(node, FWD(param)),
                        DeclareParamInto(node, FWD(others))...};
    } else {
      return std::make_tuple(DeclareParamInto(node, FWD(param)),
                             DeclareParamInto(node, FWD(others))...);
    }
  }
}

} // namespace lfc::ros
