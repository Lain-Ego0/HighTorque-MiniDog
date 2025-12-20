#include <dm_imu/hardware/imu_hardware.hpp>
#include <dm_imu/imu_driver.h>
#include <pluginlib/class_list_macros.hpp>
#include <rclcpp/rclcpp.hpp>
#include <cmath>
#include <chrono>

namespace dm_imu {

using hardware_interface::CallbackReturn;
using hardware_interface::return_type;

CallbackReturn ImuHardware::on_init(const hardware_interface::HardwareInfo &info) {
  if (hardware_interface::SensorInterface::on_init(info) != CallbackReturn::SUCCESS) {
    return CallbackReturn::ERROR;
  }
  if (info.hardware_parameters.find("port") != info.hardware_parameters.end()) {
    port_ = info.hardware_parameters.at("port");
  }
  if (info.hardware_parameters.find("baud") != info.hardware_parameters.end()) {
    baud_ = std::stoi(info.hardware_parameters.at("baud"));
  }
  if (info.hardware_parameters.find("frame_id") != info.hardware_parameters.end()) {
    frame_id_ = info.hardware_parameters.at("frame_id");
  }
  if (info.hardware_parameters.find("simulate") != info.hardware_parameters.end()) {
    auto v = info.hardware_parameters.at("simulate");
    simulate_ = (v == "true" || v == "1" || v == "True");
  }
  if (port_ == "mock" || baud_ == 0) {
    simulate_ = true;
  }
  RCLCPP_INFO(rclcpp::get_logger("dm_imu::ImuHardware"), "init: port=%s baud=%d frame_id=%s simulate=%s", port_.c_str(), baud_, frame_id_.c_str(), simulate_ ? "true" : "false");
  return CallbackReturn::SUCCESS;
}

CallbackReturn ImuHardware::on_activate(const rclcpp_lifecycle::State &previous_state) {
  if (simulate_) {
    running_ = true;
    worker_ = std::thread(&ImuHardware::run_sim, this);
    RCLCPP_WARN(rclcpp::get_logger("dm_imu::ImuHardware"), "activate: simulate mode enabled");
    return CallbackReturn::SUCCESS;
  }
  uart_.configure(port_, static_cast<uint32_t>(baud_), 100);
  if (!simulate_ && !uart_.open()) {
    simulate_ = true;
  }
  running_ = true;
  if (simulate_) {
    worker_ = std::thread(&ImuHardware::run_sim, this);
    RCLCPP_WARN(rclcpp::get_logger("dm_imu::ImuHardware"), "activate: failed to open serial on %s, switching to simulate", port_.c_str());
  } else {
    worker_ = std::thread(&ImuHardware::run, this);
    RCLCPP_INFO(rclcpp::get_logger("dm_imu::ImuHardware"), "activate: serial opened on %s at %d", port_.c_str(), baud_);
  }
  return CallbackReturn::SUCCESS;
}

CallbackReturn ImuHardware::on_deactivate(const rclcpp_lifecycle::State &previous_state) {
  running_ = false;
  if (worker_.joinable()) worker_.join();
  uart_.close();
  return CallbackReturn::SUCCESS;
}

return_type ImuHardware::read(const rclcpp::Time &time, const rclcpp::Duration &period) {
  std::lock_guard<std::mutex> lk(mtx_);
  return return_type::OK;
}

std::vector<hardware_interface::StateInterface> ImuHardware::export_state_interfaces() {
  std::vector<hardware_interface::StateInterface> states;
  const auto name = info_.sensors.empty() ? info_.name : info_.sensors[0].name;
  states.emplace_back(name, std::string("orientation.w"), &ori_w_);
  states.emplace_back(name, std::string("orientation.x"), &ori_x_);
  states.emplace_back(name, std::string("orientation.y"), &ori_y_);
  states.emplace_back(name, std::string("orientation.z"), &ori_z_);
  states.emplace_back(name, std::string("angular_velocity.x"), &gyro_x_);
  states.emplace_back(name, std::string("angular_velocity.y"), &gyro_y_);
  states.emplace_back(name, std::string("angular_velocity.z"), &gyro_z_);
  states.emplace_back(name, std::string("linear_acceleration.x"), &acc_x_);
  states.emplace_back(name, std::string("linear_acceleration.y"), &acc_y_);
  states.emplace_back(name, std::string("linear_acceleration.z"), &acc_z_);
  return states;
}

void ImuHardware::run() {
  dmbot_serial::IMU_Receive_Frame receive_data{};
  while (running_) {
    auto n1 = uart_.read((uint8_t*)(&receive_data.FrameHeader1),4);
    if (n1 != 4) {
      continue;
    }
    if(receive_data.FrameHeader1==0x55 && receive_data.flag1==0xAA && receive_data.slave_id1==0x01 && receive_data.reg_acc==0x01) {
      auto n2 = uart_.read((uint8_t*)(&receive_data.accx_u32),57-4);
      if (n2 != 57-4) {
        continue;
      }
      double accx = 0.0, accy = 0.0, accz = 0.0;
      double gyrox = 0.0, gyroy = 0.0, gyroz = 0.0;
      double roll = 0.0, pitch = 0.0, yaw = 0.0;
      if(Get_CRC16((uint8_t*)(&receive_data.FrameHeader1), 16)==receive_data.crc1) {
        accx = *((float *)(&receive_data.accx_u32));
        accy = *((float *)(&receive_data.accy_u32));
        accz = *((float *)(&receive_data.accz_u32));
      }
      if(Get_CRC16((uint8_t*)(&receive_data.FrameHeader2), 16)==receive_data.crc2) {
        gyrox = *((float *)(&receive_data.gyrox_u32));
        gyroy = *((float *)(&receive_data.gyroy_u32));
        gyroz = *((float *)(&receive_data.gyroz_u32));
      }
      if(Get_CRC16((uint8_t*)(&receive_data.FrameHeader3), 16)==receive_data.crc3) {
        roll = *((float *)(&receive_data.roll_u32));
        pitch = *((float *)(&receive_data.pitch_u32));
        yaw = *((float *)(&receive_data.yaw_u32));
      }
      tf2::Quaternion q;
      q.setRPY(roll/180.0*M_PI, pitch/180.0*M_PI, yaw/180.0*M_PI);
      std::lock_guard<std::mutex> lk(mtx_);
      ori_w_ = q.w();
      ori_x_ = q.x();
      ori_y_ = q.y();
      ori_z_ = q.z();
      gyro_x_ = gyrox;
      gyro_y_ = gyroy;
      gyro_z_ = gyroz;
      acc_x_ = accx;
      acc_y_ = accy;
      acc_z_ = accz;
    }
  }
}

void ImuHardware::run_sim() {
  double t = 0.0;
  while (running_) {
    t += 0.01;
    tf2::Quaternion q;
    q.setRPY(std::sin(t) * 0.1, std::cos(t) * 0.1, std::sin(t * 0.5) * 0.1);
    std::lock_guard<std::mutex> lk(mtx_);
    ori_w_ = q.w();
    ori_x_ = q.x();
    ori_y_ = q.y();
    ori_z_ = q.z();
    gyro_x_ = 0.0;
    gyro_y_ = 0.0;
    gyro_z_ = 0.0;
    acc_x_ = 0.0;
    acc_y_ = 0.0;
    acc_z_ = 0.0;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

} // namespace dm_imu

PLUGINLIB_EXPORT_CLASS(dm_imu::ImuHardware, hardware_interface::SensorInterface)
