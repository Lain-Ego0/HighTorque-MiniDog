cmake_minimum_required(VERSION 3.5)
project(dm_imu)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)

include_directories(
  include
)

add_executable(${PROJECT_NAME}_node
  src/bsp_crc.cpp
  src/uart_port.cpp
  src/imu_driver.cpp
  src/test_imu.cpp
)

ament_target_dependencies(${PROJECT_NAME}_node
  rclcpp
  sensor_msgs
  geometry_msgs
  std_msgs
  tf2
  tf2_geometry_msgs
)

add_library(dm_imu_hardware SHARED
  src/hardware/imu_hardware.cpp
  src/uart_port.cpp
  src/bsp_crc.cpp
)

ament_target_dependencies(dm_imu_hardware
  rclcpp
  tf2
  tf2_geometry_msgs
  hardware_interface
  pluginlib
)

pluginlib_export_plugin_description_file(hardware_interface resource/dm_imu_hardware_plugin.xml)


install(TARGETS ${PROJECT_NAME}_node
  DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS dm_imu_hardware
  EXPORT dm_imu_hardwareTargets
  LIBRARY DESTINATION lib
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY resource/
  DESTINATION share/${PROJECT_NAME}/resource
)

install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

install(DIRECTORY urdf/
  DESTINATION share/${PROJECT_NAME}/urdf
)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

ament_package()
