#pragma once

#include "lfc/linear_model.hpp"
#include "rclcpp/rclcpp.hpp"

namespace lfc::ros {

struct LFCNode : public rclcpp::Node {
  LFCNode();
};

} // namespace lfc::ros
