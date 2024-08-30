// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <functional>
#include <memory>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/header.hpp"

using std::placeholders::_1;
static const rmw_qos_profile_t my_qos_profile =
{
    RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT,
    10,
    RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT,
    RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT,
    RMW_QOS_DEADLINE_DEFAULT,
    RMW_QOS_LIFESPAN_DEFAULT,
    RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
    RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
    false
};

class MinimalSubscriber : public rclcpp::Node
{
public:
  MinimalSubscriber()
  : Node("minimal_subscriber")
  {
    auto qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile.history, 10), my_qos_profile);
    publisher_ = this->create_publisher<std_msgs::msg::Header>("uptopic", qos);
    subscription_ = this->create_subscription<std_msgs::msg::Header>(
      "downtopic", qos, std::bind(&MinimalSubscriber::topic_callback, this, _1));

    RCLCPP_INFO(this->get_logger(), "subnode starts 0829 ver");
  }

private:
  void topic_callback(const std_msgs::msg::Header & msg) 
  {
    std::string content = msg.frame_id;
    auto message = std_msgs::msg::Header();
    message.frame_id = content;
    message.stamp = msg.stamp;

    publisher_->publish(message);
  }
  rclcpp::Publisher<std_msgs::msg::Header>::SharedPtr publisher_;
  rclcpp::Subscription<std_msgs::msg::Header>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}
