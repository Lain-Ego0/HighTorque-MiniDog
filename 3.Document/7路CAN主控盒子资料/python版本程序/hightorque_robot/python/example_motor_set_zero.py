#!/usr/bin/env python3
"""
Example Python script demonstrating hightorque_robot library usage.
This script shows how to control motors using the Python bindings.
"""

import time
import sys
import os
import signal
import threading
import numpy as np

# Add the python directory to the path to import the module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

try:
    import hightorque_robot_py as hr
except ImportError as e:
    print(f"Error importing hightorque_robot_py: {e}")
    print("Make sure you have built the Python bindings using CMake")
    sys.exit(1)


# Global flag for clean shutdown
shutdown_flag = threading.Event()

def signal_handler(signum, frame):
    """Handle Ctrl+C signal"""
    print("\n\nReceived interrupt signal. Shutting down gracefully...")
    shutdown_flag.set()


def main():
    print("<<< motor_set_zero >>>")
    
    # Reset shutdown flag
    shutdown_flag.clear()
    
    # Create robot instance
    robot = hr.Robot()
    
    # Enable LCM communication
    robot.lcm_enable()
    
    # Get number of motors
    motor_count = len(robot.Motors)
    print(f"Found {motor_count} motors")
    
    if motor_count == 0:
        print("No motors found. Check your configuration and connections.")
        return
    
    # Print motor information
    for i, motor in enumerate(robot.Motors):
        print(f"Motor {i}: ID={motor.get_motor_id()}, "
              f"Type={motor.get_motor_enum_type()}, "
              f"Name={motor.get_motor_name()}")
    
    # Set up signal handler
    signal.signal(signal.SIGINT, signal_handler)
    
    # Motor control loop
    print("Starting motor control loop...")
    print("Press Ctrl+C to stop...")
    robot.set_reset_zero()
    time.sleep(1.5)
    
    try:
        while not shutdown_flag.is_set():

            robot.send_get_motor_state_cmd()
            
            for i, motor in enumerate(robot.Motors):
                state = motor.get_current_motor_state()
                print(f"Motor {state.ID}: mode={state.mode}, "
                      f"fault=0x{state.fault:02X}, "
                      f"pos={state.position:.6f}, "
                      f"vel={state.velocity:.6f}, "
                      f"tor={state.torque:.6f}")
            
            
            # Use shorter sleep with check for shutdown
            for _ in range(50):  # 50 * 10ms = 500ms total
                if shutdown_flag.is_set():
                    break
                time.sleep(0.001)
                      
    except Exception as e:
        print(f"\nError occurred: {e}")
    
    finally:
        print("\nStopping motors...")
        robot.set_stop()
        robot.motor_send_2()  # Send stop command
        time.sleep(0.1)  # Give time for stop command to be sent
        print("Motors stopped.")
        
        # Force exit after a short delay to ensure cleanup
        print("Forcing exit...")
        os._exit(0)

if __name__ == "__main__":
    print("Hightorque Robot Python Interface Demo")
    print("=" * 40)
    
    try:

        main()

            
    except KeyboardInterrupt:
        print("\nExiting...")
    except Exception as e:
        print(f"Error: {e}")
        print(f"Error: {e}")