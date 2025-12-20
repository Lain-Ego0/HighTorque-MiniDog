#include "dm_imu/imu_driver.h"
#include "std_msgs/msg/float64_multi_array.hpp"
#include <chrono>

namespace dmbot_serial
{
DmImu::DmImu(rclcpp::Node::SharedPtr node)
{
  node_ = node;
  node_->declare_parameter<std::string>("port", "/dev/ttyACM0");
  node_->declare_parameter<int>("baud", 921600);
  node_->get_parameter("port", imu_serial_port);
  node_->get_parameter("baud", imu_seial_baud);

  imu_msgs.header.frame_id = "imu_link";

  init_imu_serial();

  imu_pub_ = node_->create_publisher<sensor_msgs::msg::Imu>("imu/data", rclcpp::QoS(2));
  imu_pose_pub_ = node_->create_publisher<geometry_msgs::msg::PoseStamped>("pose", rclcpp::QoS(100));
  euler2_pub_ = node_->create_publisher<std_msgs::msg::Float64MultiArray>("dm/imu", rclcpp::QoS(9));

  enter_setting_mode();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  turn_on_accel();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  turn_on_gyro();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  turn_on_euler();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  turn_off_quat();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  set_output_1000HZ();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  save_imu_para();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  
  exit_setting_mode();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  rec_thread = std::thread(&DmImu::get_imu_data_thread, this);

  RCLCPP_INFO(node_->get_logger(), "imu init complete");
}

DmImu::~DmImu()
{ 
  RCLCPP_INFO(node_ ? node_->get_logger() : rclcpp::get_logger("dm_imu"), "enter ~DmImu()");
  stop_thread_ = true;
  if (uart_.is_open())
  {
    uart_.close();
  }
  if(rec_thread.joinable())
  {
    rec_thread.join();
  }
}

void DmImu::init_imu_serial()
{
    uart_.configure(imu_serial_port, static_cast<uint32_t>(imu_seial_baud), 100);
    if (!uart_.open())
    {
        RCLCPP_ERROR(node_->get_logger(), "In initialization,Unable to open imu serial port ");
        exit(0);
    }
    RCLCPP_INFO(node_->get_logger(), "In initialization,Imu Serial Port initialized");
}

void DmImu::enter_setting_mode()
{
  uint8_t txbuf[4]={0xAA,0x06,0x01,0x0D};
  for(int i=0;i<5;i++)
  {
    uart_.write(txbuf,sizeof(txbuf));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DmImu::turn_on_accel()
{
  uint8_t txbuf[4]={0xAA,0x01,0x14,0x0D};
  for(int i=0;i<5;i++)
  {
    uart_.write(txbuf,sizeof(txbuf));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DmImu::turn_on_gyro()
{
  uint8_t txbuf[4]={0xAA,0x01,0x15,0x0D};
  for(int i=0;i<5;i++)
  {
    uart_.write(txbuf,sizeof(txbuf));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DmImu::turn_on_euler()
{
  uint8_t txbuf[4]={0xAA,0x01,0x16,0x0D};
  for(int i=0;i<5;i++)
  {
    uart_.write(txbuf,sizeof(txbuf));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DmImu::turn_off_quat()
{
  uint8_t txbuf[4]={0xAA,0x01,0x07,0x0D};
  for(int i=0;i<5;i++)
  {
    uart_.write(txbuf,sizeof(txbuf));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DmImu::set_output_1000HZ()
{
  uint8_t txbuf[5]={0xAA,0x02,0x01,0x00,0x0D};
  for(int i=0;i<5;i++)
  {
    uart_.write(txbuf,sizeof(txbuf));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DmImu::save_imu_para()
{
  uint8_t txbuf[4]={0xAA,0x03,0x01,0x0D};
  for(int i=0;i<5;i++)
  {
    uart_.write(txbuf,sizeof(txbuf));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DmImu::exit_setting_mode()
{
  uint8_t txbuf[4]={0xAA,0x06,0x00,0x0D};
  for(int i=0;i<5;i++)
  {
    uart_.write(txbuf,sizeof(txbuf));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DmImu::restart_imu()
{
  uint8_t txbuf[4]={0xAA,0x00,0x00,0x0D};
  for(int i=0;i<5;i++)
  {
    uart_.write(txbuf,sizeof(txbuf));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DmImu::get_imu_data_thread()
{ 
  int error_num=0;
  while (rclcpp::ok()&&!stop_thread_)
  {    
    if (!uart_.is_open())
    {
      RCLCPP_WARN(node_->get_logger(), "In get_imu_data_thread,imu serial port unopen");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }       

    auto start_time = node_->get_clock()->now();

    auto n1 = uart_.read((uint8_t*)(&receive_data.FrameHeader1),4);
    if (n1 != 4) { continue; }
    if(receive_data.FrameHeader1==0x55&&receive_data.flag1==0xAA&&receive_data.slave_id1==0x01&&receive_data.reg_acc==0x01)
    {
      auto n2 = uart_.read((uint8_t*)(&receive_data.accx_u32),57-4);
      if (n2 != 57-4) { continue; }

      if(Get_CRC16((uint8_t*)(&receive_data.FrameHeader1), 16)==receive_data.crc1)
      {
        //std::cerr<<"calculate1: "<<Get_CRC16((uint8_t*)(&receive_data.FrameHeader1), 16)<<std::endl;
        //std::cerr<<"actuaal1: "<<receive_data.crc1<<std::endl;
        data.accx =*((float *)(&receive_data.accx_u32));
        data.accy =*((float *)(&receive_data.accy_u32));
        data.accz =*((float *)(&receive_data.accz_u32));
      }
      if(Get_CRC16((uint8_t*)(&receive_data.FrameHeader2), 16)==receive_data.crc2)
      {
        //std::cerr<<"calculate2: "<<Get_CRC16((uint8_t*)(&receive_data.FrameHeader2), 16)<<std::endl;
        //std::cerr<<"actuaal2: "<<receive_data.crc2<<std::endl;
        data.gyrox =*((float *)(&receive_data.gyrox_u32));
        data.gyroy =*((float *)(&receive_data.gyroy_u32));
        data.gyroz =*((float *)(&receive_data.gyroz_u32));
      }
      if(Get_CRC16((uint8_t*)(&receive_data.FrameHeader3), 16)==receive_data.crc3)
      { 
        //std::cerr<<"calculate3: "<<Get_CRC16((uint8_t*)(&receive_data.FrameHeader3), 16)<<std::endl;
        //std::cerr<<"actuaal3: "<<receive_data.crc3<<std::endl;
        data.roll =*((float *)(&receive_data.roll_u32));
        data.pitch =*((float *)(&receive_data.pitch_u32));
        data.yaw =*((float *)(&receive_data.yaw_u32));
        
      }
        // publish imu message
        imu_msgs.header.stamp = node_->get_clock()->now();
        tf2::Quaternion q;
        q.setRPY(
          data.roll/ 180.0 * M_PI,
          data.pitch/ 180.0 * M_PI,
          data.yaw/ 180.0 * M_PI);
        imu_msgs.orientation = tf2::toMsg(q);

        imu_msgs.angular_velocity.x = data.gyrox;
        imu_msgs.angular_velocity.y = data.gyroy;
        imu_msgs.angular_velocity.z = data.gyroz;
    
        imu_msgs.linear_acceleration.x = data.accx;
        imu_msgs.linear_acceleration.y = data.accy;
        imu_msgs.linear_acceleration.z = data.accz;
    
        imu_pub_->publish(imu_msgs);

        geometry_msgs::msg::PoseStamped pose;
        pose.header.frame_id = "imu_link";
        pose.header.stamp = imu_msgs.header.stamp;
        pose.pose.position.x = 0.0;
        pose.pose.position.y = 0.0;
        pose.pose.position.z = 0.0;
        pose.pose.orientation.w = imu_msgs.orientation.w;
        pose.pose.orientation.x = imu_msgs.orientation.x;
        pose.pose.orientation.y = imu_msgs.orientation.y;
        pose.pose.orientation.z = imu_msgs.orientation.z;
        
        imu_pose_pub_->publish(pose);

        std_msgs::msg::Float64MultiArray euler2_msg;
        euler2_msg.data.resize(9);
        euler2_msg.data[0]=data.gyrox;
        euler2_msg.data[1]=data.gyroy;
        euler2_msg.data[2]=data.gyroz;
        euler2_msg.data[3]=data.accx;
        euler2_msg.data[4]=data.accy;
        euler2_msg.data[5]=data.accz;
        euler2_msg.data[6]=data.roll/ 180.0 * M_PI;
        euler2_msg.data[7]=data.pitch/ 180.0 * M_PI;
        euler2_msg.data[8]=data.yaw/ 180.0 * M_PI;
        //euler2_pub_->publish(euler2_msg);
    }
    else
    { 
      error_num++;
      if(error_num>1200)
      {
        std::cerr<<"fail to get the correct imu data,finding header 0x55"<<std::endl;
      }
    }
  }
}

}
