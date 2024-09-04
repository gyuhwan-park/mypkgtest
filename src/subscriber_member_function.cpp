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
static const rmw_qos_profile_t my_qos_profile_keepall =
{
    RMW_QOS_POLICY_HISTORY_KEEP_ALL,
    10,
    RMW_QOS_POLICY_RELIABILITY_RELIABLE,
    RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT,
    RMW_QOS_DEADLINE_DEFAULT,
    RMW_QOS_LIFESPAN_DEFAULT,
    RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
    RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
    false
};
static const rmw_qos_profile_t my_qos_profile_keeplast =
{
    RMW_QOS_POLICY_HISTORY_KEEP_LAST,
    10,
    RMW_QOS_POLICY_RELIABILITY_RELIABLE,
    RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT,
    RMW_QOS_DEADLINE_DEFAULT,
    RMW_QOS_LIFESPAN_DEFAULT,
    RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
    RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
    false
};
static const rmw_qos_profile_t my_qos_profile_best =
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
static const rmw_qos_profile_t my_qos_profile_queue =
{
    RMW_QOS_POLICY_HISTORY_KEEP_LAST,
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
    auto qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_keepall.history, 10), my_qos_profile_keepall);
    if (rule == 1)
      qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_keeplast.history, 10), my_qos_profile_keeplast);
    else if (rule == 2)
      qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_best.history, 10), my_qos_profile_best);
    else if (rule == 3)
      qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_queue.history, 100), my_qos_profile_queue);

    publisher_ = this->create_publisher<std_msgs::msg::Header>("uptopic", qos);
    subscription_ = this->create_subscription<std_msgs::msg::Header>(
      "downtopic", qos, std::bind(&MinimalSubscriber::topic_callback, this, _1));

    RCLCPP_INFO(this->get_logger(), "subnode starts 0829 ver");
  }

private:
  void topic_callback(const std_msgs::msg::Header & msg) 
  {
    // RCLCPP_INFO(this->get_logger(), ">>> %f", this->now().seconds());
    std::string content = msg.frame_id;
    auto message = std_msgs::msg::Header();
    message.frame_id = content;
    message.stamp = msg.stamp;

    publisher_->publish(message);

    // RCLCPP_INFO(this->get_logger(), "NUM%s ", content.substr(content.size() - 6).c_str());
    // RCLCPP_INFO(this->get_logger(), "<<< %f", this->now().seconds());
  }
  rclcpp::Publisher<std_msgs::msg::Header>::SharedPtr publisher_;
  rclcpp::Subscription<std_msgs::msg::Header>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  int rule = atoi(argv[1]);

  rclcpp::spin(std::make_shared<MinimalSubscriber>(rule));
  rclcpp::shutdown();
  return 0;
}
