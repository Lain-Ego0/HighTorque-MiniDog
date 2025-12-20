#pragma once
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <hardware_interface/sensor_interface.hpp>
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <rclcpp/rclcpp.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <dm_imu/uart_port.hpp>
#include <dm_imu/bsp_crc.h>

namespace dm_imu {

class ImuHardware : public hardware_interface::SensorInterface {
public:
  hardware_interface::CallbackReturn on_init(const hardware_interface::HardwareInfo &info) override;
  hardware_interface::CallbackReturn on_activate(const rclcpp_lifecycle::State &previous_state) override;
  hardware_interface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State &previous_state) override;
  hardware_interface::return_type read(const rclcpp::Time &time, const rclcpp::Duration &period) override;
  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

private:
  void run();
  void run_sim();

  std::string port_;
  int baud_{0};
  std::string frame_id_{"imu_link"};
  bool simulate_{false};
  dmbot_serial::UARTPort uart_;
  std::thread worker_;
  std::atomic<bool> running_{false};
  std::mutex mtx_;

  double ori_w_{0.0};
  double ori_x_{0.0};
  double ori_y_{0.0};
  double ori_z_{0.0};
  double gyro_x_{0.0};
  double gyro_y_{0.0};
  double gyro_z_{0.0};
  double acc_x_{0.0};
  double acc_y_{0.0};
  double acc_z_{0.0};
};

} // namespace dm_imu
