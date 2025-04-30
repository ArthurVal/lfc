#include "lfc/ros/linear_feedback_node.hpp"

// System
#include <cstdarg>
#include <cstdint>
#include <cstdio>

// Internal lfc - PUBLIC
#include "lfc/linear_model.hpp"

// Internal lfc - PRIVATE
#include "macros.h"
#include "params/declare_params.hpp"
#include "params/param_eigen.hpp"
#include "params/param_raw.hpp"

// Ext libs
// -- Eigen
#include "Eigen/Core"

// -- ROS
#include "rclcpp/exceptions/exceptions.hpp"
#include "rclcpp/logging.hpp"
#include "rclcpp/qos.hpp"

namespace lfc::ros {

using gains_t = Eigen::MatrixXd;
using offset_t = Eigen::VectorXd;

using joint_state_t = sensor_msgs::msg::JointState;
using input_t = Eigen::VectorXd;

struct LinearFeedbackNodeImpl {
  gains_t gains = gains_t{};
  offset_t offset = offset_t{};
};

namespace {

auto MakeStringFrom(const char *fmt,
                    va_list args) noexcept -> std::optional<std::string> {

  std::optional<std::string> out = std::nullopt;
  std::va_list args_cpy;
  va_copy(args, args_cpy);

  const auto expected_size = std::vsnprintf(nullptr, 0, fmt, args);
  if (expected_size >= 0) {
    auto str = std::string{};
    str.resize(static_cast<std::size_t>(expected_size));
    if (std::vsprintf(str.data(), fmt, args_cpy) >= 0) {
      out = std::move(str);
    }
  }

  va_end(args_cpy);
  return out;
}

__attribute__((format(printf, 1, 2))) auto
MakeStringFrom(const char *fmt, ...) noexcept -> std::optional<std::string> {
  std::va_list args;
  va_start(args, fmt);
  auto out = MakeStringFrom(fmt, args);
  va_end(args);
  return out;
}

template <class ExceptionType,
          std::enable_if_t<
              std::is_base_of_v<std::exception, std::decay_t<ExceptionType>>,
              bool> = true>
auto LogAndThrow(const rclcpp::Logger &logger, ExceptionType &&ex) -> void {
  RCLCPP_FATAL(logger, "%s", ex.what());
  throw FWD(ex);
}

} // namespace

LinearFeedbackNode::LinearFeedbackNode()
    : LinearFeedbackNode(rclcpp::NodeOptions{}) {}

LinearFeedbackNode::LinearFeedbackNode(const rclcpp::NodeOptions &options)
    : rclcpp::Node(/* name = */ "linear_feedback", /* ns = */ "", options),
      m_impl(std::make_unique<LinearFeedbackNodeImpl>()), m_input(nullptr) {
  RCLCPP_DEBUG(get_logger(), "Starting: ...");

  auto &gains = m_impl->gains;
  auto &offset = m_impl->offset;

  // PARAMETERS
  RCLCPP_DEBUG(get_logger(), "Declaring parameters: ...");

  // -- > Init the gains/offset
  {
    std::tie(gains, offset) = DeclareParams(
        *this, ParamMatrix<gains_t>("gains"), ParamVector<offset_t>("offset"));

    if (gains.rows() != offset.size()) {
      LogAndThrow(
          get_logger(),
          rclcpp::exceptions::InvalidParametersException{
              MakeStringFrom("Size mismatch between 'offset/size' and "
                             "'gains/shape/rows' (%ld vs %ld)",
                             offset.size(), gains.rows())
                  .value_or(std::string{FILE_LINE} +
                            ": MakeStringFrom failed: " + std::strerror(errno)),
          });
    }

    RCLCPP_INFO(get_logger(),
                "Initial shapes:"
                "\n - Gains : [%ldx%ld] (ROWSxCOLS)"
                "\n - Offset: [%ld]",
                gains.rows(), gains.cols(), offset.size());

    RCLCPP_DEBUG_STREAM(get_logger(), "Initial values:\n - Gains :\n"
                                          << gains << "\n - Offset:\n"
                                          << offset);
  }

  RCLCPP_INFO(get_logger(), "Declaring parameters: DONE");

  // PUBLISHERS
  RCLCPP_DEBUG(get_logger(), "Declaring publishers: ...");
  RCLCPP_INFO(get_logger(), "Declaring publishers: DONE");

  // SUBSCRIBERS
  RCLCPP_DEBUG(get_logger(), "Declaring subscribers: ...");
  // m_input = create_subscription<joint_state_t>(
  //     "joint_state", rclcpp::QoS{/* depth = */ 5},
  //     [](const joint_state_t &joint_state) {
  //       // TODO
  //       (void)joint_state;
  //     });
  RCLCPP_INFO(get_logger(), "Declaring subscribers: DONE");

  RCLCPP_INFO(get_logger(), "Starting: DONE");
}

LinearFeedbackNode::~LinearFeedbackNode() noexcept {
  RCLCPP_DEBUG(get_logger(), "Shutdown: ...");
  RCLCPP_INFO(get_logger(), "Shutdown: DONE");
}

} // namespace lfc::ros
