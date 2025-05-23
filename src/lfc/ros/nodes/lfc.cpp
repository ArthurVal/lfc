#include "lfc/ros/linear_feedback_node.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<lfc::ros::LinearFeedbackNode>());
  rclcpp::shutdown();
  return 0;
}
