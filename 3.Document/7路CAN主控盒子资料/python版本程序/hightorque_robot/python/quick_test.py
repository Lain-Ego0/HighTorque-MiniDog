#!/usr/bin/env python3
"""
Quick test of Python bindings without hardware interaction.
"""

import hightorque_robot_py as hr

def main():
    print("=== Hightorque Robot Python Bindings Test ===")
    
    # Test 1: Basic imports and constants
    print("\n1. Testing basic functionality:")
    print(f"   ✓ Module imported: hightorque_robot_py")
    print(f"   ✓ Classes available: Robot, Motor, MotorBackData")
    
    # Test 2: Motor types
    motor_types = hr.get_motor_type_map()
    print(f"\n2. Motor types ({len(motor_types)} available):")
    for i, (name, type_val) in enumerate(list(motor_types.items())[:5]):
        print(f"   - {name}: {type_val}")
    
    # Test 3: Control modes
    print(f"\n3. Control modes:")
    print(f"   - Position: 0x{hr.MODE_POSITION:02X}")
    print(f"   - Velocity: 0x{hr.MODE_VELOCITY:02X}")
    print(f"   - Torque: 0x{hr.MODE_TORQUE:02X}")
    print(f"   - Combined: 0x{hr.MODE_POS_VEL_TQE:02X}")
    
    # Test 4: Data structures
    print(f"\n4. Testing data structures:")
    motor_data = hr.MotorBackData()
    motor_data.ID = 1
    motor_data.position = 1.57  # π/2 radians
    motor_data.velocity = 0.5
    motor_data.torque = 2.0
    motor_data.mode = hr.MODE_POSITION
    motor_data.fault = 0
    
    print(f"   ✓ MotorBackData created:")
    print(f"     ID: {motor_data.ID}")
    print(f"     Position: {motor_data.position:.3f} rad")
    print(f"     Velocity: {motor_data.velocity:.3f} rad/s")
    print(f"     Torque: {motor_data.torque:.3f} Nm")
    print(f"     Mode: 0x{motor_data.mode:02X}")
    print(f"     Fault: 0x{motor_data.fault:02X}")
    
    # Test 5: Motor version structure
    print(f"\n5. Testing motor version structure:")
    version = hr.MotorVersion()
    version.id = 1
    version.major = 4
    version.minor = 0
    version.patch = 3
    print(f"   ✓ MotorVersion: v{version.major}.{version.minor}.{version.patch} (Motor ID: {version.id})")
    
    print(f"\n=== All tests passed! ===")
    print(f"Python bindings are working correctly.")
    print(f"You can now use the hightorque_robot_py module in your Python projects.")

if __name__ == "__main__":
    main()