#!/usr/bin/env python3

import rospy
from std_msgs.msg import UInt8
# from sensor_msgs.msg import JoyFeedbackArray, JoyFeedback
import pygame
import time

class BatteryVibrator:
    def __init__(self):
        # 初始化节点
        rospy.init_node('battery_vibrator', anonymous=True)
        pygame.init()
        pygame.joystick.init()

        if pygame.joystick.get_count() < 1:
            print("未检测到手柄")
            exit(1)

        self.last_time = time.time() * 1000
        self.vib_triger_flag = False

        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()
        print(f"检测到手柄：{self.joystick.get_name()}\n")

        if hasattr(self.joystick, "rumble"):
            print("手柄支持震动功能")
        
        # 订阅电池电量话题
        rospy.Subscriber('/battery_level', UInt8, self.battery_callback)
        
        # 设置参数
        self.vibration_enabled = rospy.get_param('~vibration_enabled', True)   # 是否启用震动功能
        self.warning_level = rospy.get_param('~warning_level', 20)  # 警告阈值
        self.vibration_intensity = rospy.get_param('~vibration_intensity', 0.8)  # 震动强度 0.0-1.0
        self.vibration_duration = rospy.get_param('~vibration_duration', 500)  # 震动持续时间(毫秒)
        
        rospy.loginfo(f"Battery vibrator started. Will warn at {self.warning_level}% battery.")
    
    def battery_callback(self, msg):
        current_level = msg.data
        
        # 检查是否需要触发低电量警告
        if current_level <= self.warning_level:
            if self.vibration_enabled:
                rospy.logwarn(f"LOW BATTERY WARNING! ({current_level}% remaining)")
                self.vib_triger_flag = True
                self.last_time = time.time() * 1000
            else:
                rospy.logwarn(f"LOW BATTERY WARNING! ({current_level}% remaining) - Vibration disabled")
            
        
        # 检查是否恢复出低电量状态
        elif current_level > self.warning_level:
            rospy.loginfo(f"Battery recovered to {current_level}%")
            self.vib_en = False
        
        # 更新上次记录的电量
        self.last_warning_level = current_level
    
    def joy_vibration(self):
        if hasattr(self.joystick, "rumble"):
            self.joystick.rumble(self.warning_level, self.vibration_intensity, self.vibration_duration)

    def stop_vibration(self):
        if hasattr(self.joystick, "rumble"):
            self.joystick.stop_rumble()

if __name__ == '__main__':
    try:
        vib = BatteryVibrator()
        while not rospy.is_shutdown():    
            # rospy.spinOnce()
            if vib.vib_triger_flag: # 接收到振动触发信号
                if time.time() * 1000 - vib.last_time < vib.vibration_duration:
                    vib.joy_vibration()
                else:
                    vib.vib_triger_flag = False
                    vib.stop_vibration()
            rospy.sleep(0.01)
            pass
    except rospy.ROSInterruptException:
        pass