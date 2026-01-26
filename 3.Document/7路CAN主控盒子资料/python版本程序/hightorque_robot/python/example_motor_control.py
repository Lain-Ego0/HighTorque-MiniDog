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
    print("Initializing hightorque robot...")
    
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
    angle = np.pi
    count = 0
    
    try:
        while not shutdown_flag.is_set():
            # Control motors
            for i, motor in enumerate(robot.Motors):
                if i < motor_count:
                    # First half of motors move in one direction
                    motor.pos_vel_MAXtqe(angle, 0.5, 10.0)
                else:
                    # Second half move in opposite direction
                    motor.pos_vel_MAXtqe(-angle, 0.5, 10.0)
            
            # Send commands to motors
            robot.motor_send_2()
            
            # Read motor states
            for i, motor in enumerate(robot.Motors):
                state = motor.get_current_motor_state()
                print(f"Motor {state.ID}: mode={state.mode}, "
                      f"fault=0x{state.fault:02X}, "
                      f"pos={state.position:.6f}, "
                      f"vel={state.velocity:.6f}, "
                      f"tor={state.torque:.6f}")
            
            # Reverse direction every 250 cycles
            count += 1
            if count >= 250:
                count = 0
                angle *= -1
                print(f"Reversing direction, new angle: {angle}")
            
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


def demo_motor_types():
    """Demonstrate motor type enumeration"""
    print("\nAvailable motor types:")
    for name, motor_type in hr.get_motor_type_map().items():
        print(f"  {name}: {motor_type}")
    
    print(f"\nControl mode constants:")
    print(f"  MODE_POSITION: 0x{hr.MODE_POSITION:02X}")
    print(f"  MODE_VELOCITY: 0x{hr.MODE_VELOCITY:02X}")
    print(f"  MODE_TORQUE: 0x{hr.MODE_TORQUE:02X}")
    print(f"  MODE_POS_VEL_TQE: 0x{hr.MODE_POS_VEL_TQE:02X}")


def simple_position_control():
    """Simple position control example"""
    print("\nSimple position control example...")
    
    # Reset shutdown flag
    shutdown_flag.clear()
    
    # Set up signal handler
    signal.signal(signal.SIGINT, signal_handler)
    
    robot = hr.Robot()
    robot.lcm_enable()
    
    if len(robot.Motors) == 0:
        print("No motors found.")
        return
    
    motor = robot.Motors[0]  # Use first motor
    print(f"Using motor ID: {motor.get_motor_id()}")
    print("Press Ctrl+C to stop...")
    
    # Move to different positions
    positions = [0.0, 0.5, -0.5, 0.0]
    
    try:
        for pos in positions:
            if shutdown_flag.is_set():
                break
                
            print(f"Moving to position: {pos}")
            motor.position(pos)
            robot.motor_send_2()
            
            # Wait and check position
            for i in range(1000):
                if shutdown_flag.is_set():
                    break
                
                state = motor.get_current_motor_state()
                print(f"Current position: {state.position:.3f}, target: {pos}")
                time.sleep(0.001)
                robot.send_get_motor_state_cmd()
                robot.motor_send_2()
                # Check if close enough to target
                if abs(state.position - pos) < 0.01:
                    break
        
        if not shutdown_flag.is_set():
            print("Position control demo completed.")
            
    except Exception as e:
        print(f"\nError occurred: {e}")
        
    finally:
        print("\nStopping motor...")
        # robot.set_stop()
        # robot.motor_send_2()
        time.sleep(0.1)
        print("Motor stopped.")
        
        # Force exit
        print("Forcing exit...")
        os._exit(0)

def get_motor_state():
    print("Get_motor_state...")
    
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
    angle = np.pi
    count = 0
    robot.set_stop()
    robot.motor_send_2()
    robot.send_get_motor_state_cmd()
    try:
        while not shutdown_flag.is_set():
            # Control motors

            # Send commands to motors
            robot.motor_send_2()
            
            # Read motor states
            for i, motor in enumerate(robot.Motors):
                state = motor.get_current_motor_state()
                print(f"Motor {state.ID}: mode={state.mode}, "
                      f"fault=0x{state.fault:02X}, "
                      f"pos={state.position:.6f}, "
                      f"vel={state.velocity:.6f}, "
                      f"tor={state.torque:.6f}")
            
            # Reverse direction every 250 cycles
            count += 1
            if count >= 250:
                count = 0
                angle *= -1
                print(f"Reversing direction, new angle: {angle}")
            
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
    
    # Show motor types and constants
    demo_motor_types()
    
    print("\nChoose demo:")
    print("1. Full motor control loop")
    print("2. Simple position control")
    print("3. Get motor state")
    print("4. Exit")
    
    try:
        choice = input("Enter choice (1-3): ").strip()
        
        if choice == "1":
            main()
        elif choice == "2":
            simple_position_control()
        elif choice == "3":
            get_motor_state()
        elif choice == "4":
            print("Exiting...")
        else:
            print("Invalid choice.")
            
    except KeyboardInterrupt:
        print("\nExiting...")
    except Exception as e:
        print(f"Error: {e}")