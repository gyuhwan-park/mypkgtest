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

using namespace std::chrono_literals;
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

/* For this example, we will be creating a publishing node like the one in minimal_publisher.
 * We will have a single subscriber node running 2 threads. Each thread loops at different speeds, and
 * just repeats what it sees from the publisher to the screen.
 */

class MinimalPublisher : public rclcpp::Node
{
public:
  MinimalPublisher(int msg_size, int rule, int time_rule)
  : Node("minimal_publisher"), count_(0), pkg_size(msg_size)
  {
    auto qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_re.history, 5), my_qos_profile_re);
    if (rule == 1)
      qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_be.history, 5), my_qos_profile_be);
    else if (rule == 2)
      qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_queue.history, 100), my_qos_profile_queue);

    publisher_ = this->create_publisher<std_msgs::msg::Header>("downtopic", qos);
    
    if (time_rule == 0)
      timer_ = this->create_wall_timer(100us, std::bind(&MinimalPublisher::timer_callback, this));
    else if (time_rule == 1)
      timer_ = this->create_wall_timer(5ms, std::bind(&MinimalPublisher::timer_callback, this));
    else
      timer_ = this->create_wall_timer(10ms, std::bind(&MinimalPublisher::timer_callback, this));
    
    RCLCPP_INFO(this->get_logger(), "pubnode starts 0829 ver");
  }

private:
  void timer_callback()
  {
    auto message = std_msgs::msg::Header();
    // message.data = "Time : " + std::to_string(this->now().seconds()) + "000" + std::to_string(count_++);
    message.frame_id = std::string(pkg_size, '0') + std::to_string(count_);
    message.stamp = this->get_clock()->now();

    // RCLCPP_INFO(this->get_logger(), ">>>>'%s'", message.data.c_str());
    publisher_->publish(message);
    count_++;
  }
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::Header>::SharedPtr publisher_;
  size_t count_;
  int pkg_size;
};

class ResponseReader : public rclcpp::Node
{
public:
  ResponseReader(int rule)
  : Node("ResponseReader")
  {
    auto qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_re.history, 5), my_qos_profile_re);
    if (rule == 1)
      qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_be.history, 5), my_qos_profile_be);
    else if (rule == 2)
      qos = rclcpp::QoS(rclcpp::QoSInitialization(my_qos_profile_queue.history, 100), my_qos_profile_queue);
    
    subscription_ = this->create_subscription<std_msgs::msg::Header>(
      "uptopic", qos, std::bind(&ResponseReader::topic_callback, this, _1));
  }

private:
  void topic_callback(const std_msgs::msg::Header & msg) const
  {
    std::string content = msg.frame_id;
    std::string countnum = content.substr(content.size() - 6);

    RCLCPP_INFO(this->get_logger(), "NUM%s : %d.%d - %f", countnum.c_str(),
                                                          msg.stamp.sec, msg.stamp.nanosec/1000,
                                                          this->now().seconds());
  }
  rclcpp::Subscription<std_msgs::msg::Header>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  int msg_size = atoi(argv[1]);
  int rule = atoi(argv[2]);
  int time_rule = atoi(argv[3]);

  // You MUST use the MultiThreadedExecutor to use, well, multiple threads
  rclcpp::executors::MultiThreadedExecutor executor;
  auto pubnode = std::make_shared<MinimalPublisher>(msg_size, rule, time_rule);
  auto subnode = std::make_shared<ResponseReader>(rule);

  executor.add_node(pubnode);
  executor.add_node(subnode);

  executor.spin();
  rclcpp::shutdown();
  return 0;
}