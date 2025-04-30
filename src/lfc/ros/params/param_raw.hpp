#pragma once

// INTERNAL
#include "param_base.hpp"

// SYSTEM
#include <cstdint>
#include <type_traits>
#include <vector>

namespace lfc::ros {

namespace details {

template <class T, class... Others>
struct IsOneOf : std::disjunction<std::is_same<T, Others>...> {};

template <class T, class... Others>
constexpr bool IsOneOf_v = IsOneOf<T, Others...>::value;

} // namespace details

template <class T>
struct ParamRaw : public ParamBase<ParamRaw<T>> {

  static_assert(
      details::IsOneOf_v<T, bool, std::int64_t, double, std::string,
                         std::vector<bool>, std::vector<std::int64_t>,
                         std::vector<double>, std::vector<std::string>>);

  using value_type = T;
  using Base = ParamBase<ParamRaw<T>>;

  constexpr ParamRaw() = delete;
  constexpr ParamRaw(std::string_view name,
                     value_type default_value = value_type{})
      : Base(name), m_default_value(std::move(default_value)) {}

  constexpr auto DefaultValue() const -> const value_type & {
    return m_default_value;
  }

  constexpr auto DefaultTo(value_type v) -> ParamRaw & {
    m_default_value = std::move(v);
    return *this;
  }

private:
  value_type m_default_value;
};

template <class T,
          std::enable_if_t<std::is_integral_v<T> && std::is_same_v<T, bool>,
                           bool> = true>
ParamRaw(std::string_view, T) -> ParamRaw<bool>;

template <class T,
          std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>,
                           bool> = true>
ParamRaw(std::string_view, T) -> ParamRaw<std::int64_t>;

template <class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
ParamRaw(std::string_view, T) -> ParamRaw<double>;

template <class T, std::enable_if_t<std::is_constructible_v<std::string, T>,
                                    bool> = true>
ParamRaw(std::string_view, T) -> ParamRaw<std::string>;

template <class T>
constexpr auto DeclareParamInto(rclcpp::Node &node,
                                const ParamRaw<T> &param) -> T {
  return node.declare_parameter<T>(param.Name(), param.DefaultValue(),
                                   param.Descr());
}

} // namespace lfc::ros
