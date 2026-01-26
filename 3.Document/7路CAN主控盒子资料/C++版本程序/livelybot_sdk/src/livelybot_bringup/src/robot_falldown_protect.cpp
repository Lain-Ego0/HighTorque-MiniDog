#include "ros/ros.h"
#ifndef RELEASE
#include "robot.h"
#else
#include "livelybot_serial/hardware/robot.h"
#endif
#include <iostream>
#include <thread>
#include <condition_variable>
#include <livelybot_power/Power_switch.h>
#include <sensor_msgs/Imu.h>
#include <tf/transform_datatypes.h>
#include <yaml-cpp/yaml.h> 

struct AttitudeConfig {
    struct AngleThreshold {
        struct Range {
            double min;
            double max;
            bool rotable;
            bool enabled;
        };
        
        Range roll;
        Range pitch;
        Range yaw;

        bool globalEnabled;
        
        struct WarningLevel {
            double minor;
            double major;
        } warning;
    } thresholds;
};

ros::Subscriber imu_sub;
ros::Publisher power_pub;

AttitudeConfig imu_config;

void imu_callback(const sensor_msgs::Imu::ConstPtr& data);
int load_config(std::string config_path, AttitudeConfig& config);

int main(int argc, char **argv)
{
    ros::init(argc, argv, "robot_falldown_protect");
    ros::NodeHandle n;
    ros::NodeHandle private_nh("~");
    ros::Rate r(100);

    std::string config_path;
    if (!private_nh.getParam("config_path", config_path)) {
        ROS_ERROR("No config_path parameter provided! Using fallback location.");
        
        // 备选默认路径
        config_path = "/home/tang/Desktop/livelybot_sdk/src/livelybot_bringup/cfg/falldown_condition.yaml";
    }

    // std::string config_path = "/home/tang/Desktop/livelybot_sdk/src/livelybot_bringup/cfg/falldown_condition.yaml";
    load_config(config_path,  imu_config);

    imu_sub = n.subscribe("/imu/data", 1, imu_callback);
    power_pub = n.advertise<livelybot_power::Power_switch>("power_switch_control", 1);

    while (ros::ok()) // 此用法为逐个电机发送控制指令
    {
        ros::spinOnce();
        r.sleep();
    }
    return 0;
}

double radian2degree(double radian) {
    return radian * 180 / M_PI;
}

void imu_callback(const sensor_msgs::Imu::ConstPtr& data) {
    // 提取IMU姿态角
    static int cnt = 0;
    double roll, pitch, yaw;
    tf::Quaternion q(
        data->orientation.x,
        data->orientation.y,
        data->orientation.z,
        data->orientation.w
    );
    tf::Matrix3x3 rot_mat(q);
    rot_mat.getRPY(roll, pitch, yaw);
    ROS_INFO("RAW  Roll: %.2f, Pitch: %.2f, Yaw: %.2f", roll, pitch, yaw);
    roll = radian2degree(roll);
    pitch = radian2degree(pitch);
    yaw = radian2degree(yaw);

    if (imu_config.thresholds.roll.rotable && roll < 0)
    {
        roll += 360;
    }

    if (imu_config.thresholds.pitch.rotable && pitch < 0)
    {
        pitch += 360;
    }

    if (imu_config.thresholds.yaw.rotable && yaw < 0)
    {
        yaw += 360;
    }

    ROS_INFO("rot  Roll: %.2f, Pitch: %.2f, Yaw: %.2f", roll, pitch, yaw);

    bool imu_trigger = false;

    if (imu_config.thresholds.globalEnabled) { 
        if (imu_config.thresholds.roll.enabled) {
            if (roll < imu_config.thresholds.roll.min || roll > imu_config.thresholds.roll.max) {
                std::cout << "Roll angle out of range!" << std::endl;
                imu_trigger = true;
            }
        }

        if(imu_trigger == false && imu_config.thresholds.pitch.enabled)
        {
            if (pitch < imu_config.thresholds.pitch.min || pitch > imu_config.thresholds.pitch.max) {
                std::cout << "Pitch angle out of range!" << std::endl;
                imu_trigger = true;
            }
        }

        if(imu_trigger == false && imu_config.thresholds.yaw.enabled)
        {
            if (yaw < imu_config.thresholds.yaw.min || yaw > imu_config.thresholds.yaw.max) {
                std::cout << "Yaw angle out of range!" << std::endl;
                imu_trigger = true;
            }
        }

        if(imu_trigger)
        {
            livelybot_power::Power_switch power_switch;
            power_switch.control_switch = true;
            power_switch.power_switch = false;
            power_pub.publish(power_switch);
            std::cout << "Power switch off!" << std::endl;
        }

    }
    
}


int load_config(std::string config_path, AttitudeConfig& cfg) 
{ 
    try{
        YAML::Node config = YAML::LoadFile(config_path);
        
        // 全局使能
        cfg.thresholds.globalEnabled = config["enable"].as<bool>();
        
        // 告警级别
        YAML::Node warning = config["warning_level"];
        cfg.thresholds.warning.minor = warning["minor"].as<double>();
        cfg.thresholds.warning.major = warning["major"].as<double>();
        
        // 角度阈值
        YAML::Node thresholds = config["thresholds"];

        // 滚转角
        YAML::Node roll = thresholds["roll"];
        cfg.thresholds.roll.min = roll["min"].as<double>();
        cfg.thresholds.roll.max = roll["max"].as<double>();
        cfg.thresholds.roll.rotable = roll["rotable"].as<bool>();
        cfg.thresholds.roll.enabled = roll["enable"].as<bool>();
        
        // 俯仰角
        YAML::Node pitch = thresholds["pitch"];
        cfg.thresholds.pitch.min = pitch["min"].as<double>();
        cfg.thresholds.pitch.max = pitch["max"].as<double>();
        cfg.thresholds.pitch.rotable = pitch["rotable"].as<bool>();
        cfg.thresholds.pitch.enabled = pitch["enable"].as<bool>();
        
        // 滚转角
        YAML::Node yaw = thresholds["yaw"];
        cfg.thresholds.yaw.min = yaw["min"].as<double>();
        cfg.thresholds.yaw.max = yaw["max"].as<double>();
        cfg.thresholds.yaw.rotable = yaw["rotable"].as<bool>();
        cfg.thresholds.yaw.enabled = yaw["enable"].as<bool>();

        // 打印配置信息
        std::cout << "======= 姿态配置 =======" << std::endl;
        std::cout << "全局使能: " << (cfg.thresholds.globalEnabled ? "是" : "否") << std::endl;
        
        if (cfg.thresholds.globalEnabled) {

            std::cout << "\n--- 滚转角 ---" << std::endl;
            std::cout << "状态: " << (cfg.thresholds.roll.enabled ? "启用" : "禁用") << std::endl;
            std::cout << "范围: [" << cfg.thresholds.roll.min << "°, " 
                      << cfg.thresholds.roll.max << "°]" << std::endl;

            std::cout << "\n--- 俯仰角 ---" << std::endl;
            std::cout << "状态: " << (cfg.thresholds.pitch.enabled ? "启用" : "禁用") << std::endl;
            std::cout << "范围: [" << cfg.thresholds.pitch.min << "°, " 
                      << cfg.thresholds.pitch.max << "°]" << std::endl;
            
            std::cout << "\n--- 航向角 ---" << std::endl;
            std::cout << "状态: " << (cfg.thresholds.yaw.enabled ? "启用" : "禁用") << std::endl;
            std::cout << "范围: [" << cfg.thresholds.yaw.min << "°, " 
                      << cfg.thresholds.yaw.max << "°]" << std::endl;
            
            std::cout << "\n--- 告警级别 ---" << std::endl;
            std::cout << "轻微告警: " << cfg.thresholds.warning.minor * 100 << "% 阈值" << std::endl;
            std::cout << "严重告警: " << cfg.thresholds.warning.major * 100 << "% 阈值" << std::endl;
        }
        
        std::cout << "\n配置解析成功!" << std::endl;
        return 0;
    }
    catch(YAML::Exception& e) 
    {
        std::cerr << "配置解析错误: " << e.what() << std::endl;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "系统错误: " << e.what() << std::endl;
        return 1;
    }

}

