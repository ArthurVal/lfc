#include "lfc/ros/linear_feedback_node.hpp"

#include <cassert>
#include <optional>
#include <string_view>

#include "lfc/linear_model.hpp"

#include "Eigen/Core"
#include "rclcpp/rclcpp.hpp"

namespace lfc::ros {

using gains_t = Eigen::MatrixXd;
using offset_t = Eigen::VectorXd;

namespace {

template <class T>
constexpr auto
DeclareParam(rclcpp::Node &node, std::string_view name, T default_value,
             std::optional<std::string_view> description = std::nullopt) {
  auto descr = rcl_interfaces::msg::ParameterDescriptor{};

  if (description.has_value()) {
    descr.description = *description;
  }

  return node.declare_parameter<T>(std::string{name}, default_value, descr);
}

} // namespace

struct LinearFeedbackNodeImpl {
  gains_t gains = gains_t{};
  offset_t offset = offset_t{};
};

LinearFeedbackNode::LinearFeedbackNode()
    : LinearFeedbackNode(rclcpp::NodeOptions{}) {}

LinearFeedbackNode::LinearFeedbackNode(const rclcpp::NodeOptions &options)
    : rclcpp::Node(/* name = */ "linear_feedback", /* ns = */ "", options),
      m_impl(std::make_unique<LinearFeedbackNodeImpl>()) {
  RCLCPP_DEBUG(get_logger(), "Starting");

  auto use_efforts =
      DeclareParam(*this, "use_efforts", false,
                   "Set to TRUE when you wish to include the effort from the "
                   "joint_states to the computation.");

  RCLCPP_INFO(get_logger(), "%s 'efforts' from joint_states inputs",
              (use_efforts ? "Use" : "Do not use"));

  // TODO: Params
}

LinearFeedbackNode::~LinearFeedbackNode() noexcept {
  RCLCPP_DEBUG(get_logger(), "Shutdown");
}

} // namespace lfc::ros
