#include <ros/ros.h>
#include <livelybot_power/Power_switch.h>
#include <sensor_msgs/Joy.h>  // Joy消息头文件

ros::Publisher power_pub;

// 手柄按键映射参数
int lt_axes = 2;       // 右扳机（RT）默认按钮索引
int rt_axes = 5;       // 左扳机（LT）默认按钮索引
int home_btn = 8;     // XBOX按钮默认索引

// 手柄控制回调函数
void joy_callback(const sensor_msgs::Joy::ConstPtr& msg)
{
    // 检测组合键状态 (RT + LT + XBOX)
    bool rt_pressed = (lt_axes < msg->buttons.size()) ? msg->axes[lt_axes] < -0.5 : false;
    bool lt_pressed = (rt_axes < msg->buttons.size()) ? msg->axes[rt_axes] < -0.5 : false;
    bool xbox_pressed = (home_btn < msg->buttons.size()) ? msg->buttons[home_btn] > 0.5 : false;
    bool combo_pressed = rt_pressed && lt_pressed && xbox_pressed;

    // 仅当组合键新按下时触发
    if (combo_pressed) {
        livelybot_power::Power_switch power_switch;
        power_switch.control_switch = true;
        power_switch.power_switch = false;
        power_pub.publish(power_switch);
        ROS_WARN("Emergency power OFF triggered by controller combo (RT+LT+Xbox)!");
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "robot_emergency_stop");
    ros::NodeHandle nh;

    // 订阅手柄话题
    ros::Subscriber joy_sub = nh.subscribe("/joy", 10, joy_callback);
    
    // 创建电源控制发布器
    power_pub = nh.advertise<livelybot_power::Power_switch>("power_switch_control", 1);
    
    ROS_INFO("Emergency stop controller ready. Press RT+LT+Xbox to cut power.");
    
    ros::spin();  // 使用spin而不是循环+spinOnce
    
    return 0;
}