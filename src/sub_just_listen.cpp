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

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/header.hpp"

using std::placeholders::_1;
static const rmw_qos_profile_t my_qos_profile_re =
{
    RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT,
    5,
    RMW_QOS_POLICY_RELIABILITY_RELIABLE,
    RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT,
    RMW_QOS_DEADLINE_DEFAULT,
    RMW_QOS_LIFESPAN_DEFAULT,
    RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
    RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
    false
};
static const rmw_qos_profile_t my_qos_profile_be =
{
    RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT,
    5,
    RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT,
    RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT,
    RMW_QOS_DEADLINE_DEFAULT,
    RMW_QOS_LIFESPAN_DEFAULT,
    RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
    RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
    false
};
static const rmw_qos_profile_t my_qos_profile_queue =
{
    RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT,
    100,
    RMW_QOS_POLICY_RELIABILITY_RELIABLE,
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
  MinimalSubscriber(int rule)
  : Node("minimal_subscriber")
  {
    auto qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_re.history, 5), my_qos_profile0);
    if (rule == 1)
      qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_be.history, 5), my_qos_profile1);
    else if (rule == 2)
      qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_queue.history, 100), my_qos_profile2);

    subscription_ = this->create_subscription<std_msgs::msg::Header>(
      "downtopic", qos, std::bind(&MinimalSubscriber::topic_callback, this, _1));

    RCLCPP_INFO(this->get_logger(), "subnode starts 0829 ver");
  }

private:
  void topic_callback(const std_msgs::msg::Header & msg) 
  {
    std::string content = msg.frame_id;

    RCLCPP_INFO(this->get_logger(), "NUM%s ", content.substr(content.size() - 6).c_str());
  }
  rclcpp::Subscription<std_msgs::msg::Header>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  int rule = atoi(argv[1]);

  rclcpp::executors::MultiThreadedExecutor executor;
  auto subnode = std::make_shared<MinimalSubscriber>(rule);

  executor.add_node(subnode);

  executor.spin();
  rclcpp::shutdown();
  return 0;
}
