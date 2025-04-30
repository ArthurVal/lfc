#pragma once

#include "lfc/internal/has_trait.hpp"

#include <array>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "rclcpp/node.hpp"

#define FWD(v) std::forward<decltype(v)>(v)

namespace lfc::ros {

template <class T>
struct Param {
  using value_type = T;

  constexpr Param() = delete;
  constexpr Param(std::string_view name,
                  value_type default_value = value_type{})
      : m_name(name), m_default_value(std::move(default_value)), m_info() {}

  auto DefaultTo(value_type v) -> Param & {
    m_default_value = std::move(v);
    return *this;
  }

  auto Description(std::string_view descr) -> Param & {
    m_info.description = descr;
    return *this;
  }

  auto Constraints(std::string_view constraints) -> Param & {
    m_info.additional_constraints = constraints;
    return *this;
  }

  auto ReadOnly(bool v = true) -> Param & {
    m_info.read_only = v;
    return *this;
  }

  friend auto DeclareParamInto(rclcpp::Node &node, const Param &param) -> T {
    return node.declare_parameter<T>(std::string{param.m_name},
                                     param.m_default_value, param.m_info);
  }

private:
  std::string_view m_name;
  value_type m_default_value;
  rcl_interfaces::msg::ParameterDescriptor m_info;
};

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
    return DeclareParamInto(node, param);
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
