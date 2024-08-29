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

class MinimalSubscriber : public rclcpp::Node
{
public:
  MinimalSubscriber()
  : Node("minimal_subscriber")
  {
    publisher_ = this->create_publisher<std_msgs::msg::Header>("uptopic", 10);
    subscription_ = this->create_subscription<std_msgs::msg::Header>(
      "downtopic", 10, std::bind(&MinimalSubscriber::topic_callback, this, _1));

    RCLCPP_INFO(this->get_logger(), "subnode starts 0829 ver");
  }

private:
  void topic_callback(const std_msgs::msg::Header & msg) 
  {
    std::string content = msg.frame_id;
    auto message = std_msgs::msg::Header();
    message.frame_id = std::to_string(msg.stamp.sec)+"."+std::to_string(msg.stamp.nanosec/1000) + " - "
                        +std::to_string(this->now().seconds()) + content.substr(content.size() - 963);
    message.stamp = this->get_clock()->now();

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
