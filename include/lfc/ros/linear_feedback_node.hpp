#pragma once

#include <memory>

// Internal
#include "lfc/export.h"

// ROS
#include "rclcpp/node.hpp"

namespace lfc::ros {

/// PIMPL used by the LinearFeedbackNode. Contains internal impl details.
struct LFC_PRIVATE LinearFeedbackNodeImpl;

struct LFC_PUBLIC LinearFeedbackNode : public rclcpp::Node {

  /// Default construct the node (name: "linear_feedback", ns: "")
  LinearFeedbackNode();

  /// Construct the node with the specified node \arg options
  LinearFeedbackNode(const rclcpp::NodeOptions &options);

  /// Destruct the node and free allocated memory
  virtual ~LinearFeedbackNode() noexcept;

private:
  std::unique_ptr<LinearFeedbackNodeImpl> m_impl; /*!< PIMPL */
};

} // namespace lfc::ros
