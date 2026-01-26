#!/usr/bin/env python3
"""
Simple test script to verify Python bindings functionality.
"""

import hightorque_robot_py as hr

def test_basic_functionality():
    print("Testing hightorque_robot_py Python bindings...")
    
    # Test module import
    print("✓ Module imported successfully")
    
    # Test available classes
    print(f"✓ Robot class available: {hasattr(hr, 'Robot')}")
    print(f"✓ Motor class available: {hasattr(hr, 'Motor')}")
    print(f"✓ MotorBackData class available: {hasattr(hr, 'MotorBackData')}")
    
    # Test enums
    motor_types = hr.get_motor_type_map()
    print(f"✓ Motor types available: {len(motor_types)} types")
    print(f"  Examples: {list(motor_types.keys())[:3]}")
    
    # Test constants
    print("✓ Control mode constants:")
    print(f"  MODE_POSITION: 0x{hr.MODE_POSITION:02X}")
    print(f"  MODE_VELOCITY: 0x{hr.MODE_VELOCITY:02X}")
    print(f"  MODE_TORQUE: 0x{hr.MODE_TORQUE:02X}")
    print(f"  MODE_POS_VEL_TQE: 0x{hr.MODE_POS_VEL_TQE:02X}")
    
    # Test robot creation (this might fail without hardware)
    try:
        robot = hr.Robot()
        print(f"✓ Robot object created successfully")
        print(f"✓ Robot has {len(robot.Motors)} motors configured")
        
        # Test robot methods (basic ones that don't require hardware)
        print("✓ Robot methods available:")
        methods = ['lcm_enable', 'motor_send_2', 'set_stop', 'set_reset']
        for method in methods:
            available = hasattr(robot, method)
            print(f"  {method}: {'✓' if available else '✗'}")
            
    except Exception as e:
        print(f"⚠ Robot creation failed (expected without hardware): {e}")
    
    # Test motor back data structure
    try:
        motor_data = hr.MotorBackData()
        motor_data.position = 1.5
        motor_data.velocity = 0.5
        motor_data.torque = 2.0
        print(f"✓ MotorBackData structure works")
        print(f"  Set position: {motor_data.position}")
        print(f"  Set velocity: {motor_data.velocity}")
        print(f"  Set torque: {motor_data.torque}")
    except Exception as e:
        print(f"✗ MotorBackData test failed: {e}")
    
    print("\nPython bindings test completed!")

if __name__ == "__main__":
    test_basic_functionality()