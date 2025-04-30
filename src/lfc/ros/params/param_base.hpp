#pragma once

// SYSTEM
#include <string>
#include <string_view>

// EXT
// -- ROS
#include "rclcpp/node.hpp"

namespace lfc::ros {

template <class Derived>
struct ParamBase {

  constexpr ParamBase() = delete;
  constexpr ParamBase(std::string_view name) : m_name(name), m_descr() {}

  constexpr auto WithDescription(std::string_view descr) -> Derived & {
    m_descr.description = descr;
    return AsChild();
  }

  constexpr auto WithConstraints(std::string_view constraints) -> Derived & {
    m_descr.additional_constraints = constraints;
    return AsChild();
  }

  constexpr auto ReadOnly(bool v = true) -> Derived & {
    m_descr.read_only = v;
    return AsChild();
  }

  constexpr auto Name() const -> const std::string & { return m_name; }

  constexpr auto
  Descr() const -> const rcl_interfaces::msg::ParameterDescriptor & {
    return m_descr;
  }

private:
  constexpr auto AsChild() & -> Derived & {
    return static_cast<Derived &>(*this);
  }

  constexpr auto AsChild() const & -> const Derived & {
    return static_cast<const Derived &>(*this);
  }

  constexpr auto AsChild() && -> Derived && {
    return static_cast<Derived &&>(*this);
  }

  constexpr auto AsChild() const && -> const Derived && {
    return static_cast<const Derived &&>(*this);
  }

  std::string m_name;
  rcl_interfaces::msg::ParameterDescriptor m_descr;
};

} // namespace lfc::ros
