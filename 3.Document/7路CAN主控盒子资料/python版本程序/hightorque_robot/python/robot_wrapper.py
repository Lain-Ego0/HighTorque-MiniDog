#!/usr/bin/env python3
"""
Wrapper class for hightorque_robot with improved signal handling and cleanup.
This wrapper ensures proper cleanup even when the underlying C++ threads
are blocked on I/O operations.
"""

import sys
import os
import signal
import threading
import atexit
import time

# Add the python directory to the path to import the module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

try:
    import hightorque_robot_py as hr
except ImportError as e:
    print(f"Error importing hightorque_robot_py: {e}")
    sys.exit(1)


class RobotWrapper:
    """Wrapper class for Robot with better cleanup handling"""
    
    def __init__(self):
        self._robot = None
        self._shutdown_flag = threading.Event()
        self._cleanup_done = False
        
        # Register cleanup on exit
        atexit.register(self._cleanup)
        
        # Set up signal handlers
        signal.signal(signal.SIGINT, self._signal_handler)
        signal.signal(signal.SIGTERM, self._signal_handler)
        
    def _signal_handler(self, signum, frame):
        """Handle signals for clean shutdown"""
        print(f"\n\nReceived signal {signum}. Initiating shutdown...")
        self._shutdown_flag.set()
        
        # Give a moment for graceful shutdown
        time.sleep(0.5)
        
        # Force exit if not already cleaned up
        if not self._cleanup_done:
            print("Forcing exit...")
            os._exit(0)
    
    def _cleanup(self):
        """Clean up resources"""
        if self._cleanup_done:
            return
            
        self._cleanup_done = True
        
        if self._robot is not None:
            print("\nCleaning up robot resources...")
            try:
                self._robot.set_stop()
                self._robot.motor_send_2()
                time.sleep(0.1)
            except:
                pass
            
            # Delete the robot object to trigger its destructor
            self._robot = None
            
    def __enter__(self):
        """Context manager enter"""
        self._robot = hr.Robot()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self._cleanup()
        
    def __getattr__(self, name):
        """Delegate attribute access to the wrapped robot"""
        if self._robot is None:
            raise RuntimeError("Robot not initialized. Use 'with RobotWrapper() as robot:' syntax")
        return getattr(self._robot, name)
    
    @property
    def shutdown_requested(self):
        """Check if shutdown was requested"""
        return self._shutdown_flag.is_set()
    
    @property
    def robot(self):
        """Get the underlying robot object"""
        return self._robot
    
    def initialize(self):
        """Initialize the robot if not using context manager"""
        if self._robot is None:
            self._robot = hr.Robot()
        return self._robot


def example_with_wrapper():
    """Example using the wrapper class"""
    print("Robot Wrapper Example")
    print("=" * 40)
    
    with RobotWrapper() as robot:
        robot.lcm_enable()
        
        motor_count = len(robot.Motors)
        print(f"Found {motor_count} motors")
        
        if motor_count == 0:
            print("No motors found.")
            return
            
        print("Press Ctrl+C to stop...")
        
        angle = 0.5
        count = 0
        
        while not robot.shutdown_requested:
            # Control motors
            for i, motor in enumerate(robot.Motors):
                if i < motor_count // 2:
                    motor.pos_vel_MAXtqe(angle, 0.1, 10.0)
                else:
                    motor.pos_vel_MAXtqe(-angle, 0.1, 10.0)
            
            robot.motor_send_2()
            
            # Print status occasionally
            if count % 10 == 0:
                for motor in robot.Motors:
                    state = motor.get_current_motor_state()
                    print(f"Motor {state.ID}: pos={state.position:.3f}")
            
            count += 1
            if count >= 250:
                count = 0
                angle *= -1
                print(f"Reversing direction, angle: {angle}")
            
            # Check for shutdown more frequently
            for _ in range(10):
                if robot.shutdown_requested:
                    break
                time.sleep(0.05)
    
    print("Example completed successfully!")


if __name__ == "__main__":
    example_with_wrapper()