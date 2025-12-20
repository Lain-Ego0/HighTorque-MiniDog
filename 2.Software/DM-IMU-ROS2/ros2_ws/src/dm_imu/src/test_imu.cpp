#include "rclcpp/rclcpp.hpp"

#include <dm_imu/imu_driver.h>
#include <iostream>
#include <thread>
#include <condition_variable>

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("dm_imu_node");

    auto imuInterface = std::make_shared<dmbot_serial::DmImu>(node);

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}




