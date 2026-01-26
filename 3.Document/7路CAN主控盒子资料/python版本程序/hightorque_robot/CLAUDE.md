# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This is a CMake-based C++ project for high-torque robot motor control. Build commands:

```bash
mkdir -p build
cd build
cmake ..
make
```

The build produces:
- `libhightorque_robot.so` - Main shared library
- `motor_run` - Motor control example  
- `motor_msg_subscriber` - LCM message subscriber example

## Dependencies

Required system dependencies:
- `libserialport` - Serial port communication
- `serial` - Serial driver library
- `lcm` - Lightweight Communications and Marshalling
- `yaml-cpp` - YAML configuration parsing

## Architecture

### Core Components

- **Robot Class** (`include/hardware/robot.hpp`): Main robot interface managing multiple CAN boards and motors
- **Motor Control** (`include/hardware/motor.hpp`): Individual motor control with position, velocity, torque modes
- **CAN Communication** (`include/hardware/canboard.hpp`, `canport.hpp`): CAN bus interface for motor communication
- **Serial Driver** (`include/serial_driver.hpp`): Serial port communication layer
- **Configuration** (`include/parse_robot_params.hpp`): YAML-based robot parameter loading

### Communication Protocol

Uses custom serial protocol with CRC validation:
- CRC8 for header validation
- CRC16 for data integrity
- Multiple control modes: position, velocity, torque, combined control
- LCM messaging for robot state publishing

### Configuration System

Robot configurations stored in `robot_param/` directory:
- `robot_config.yaml` - Main configuration file
- DOF-specific parameter files (e.g., `1dof_STM32H730_model_test_Orin_params.yaml`)
- Supports different motor controller types (STM32H730, HPM6E00)

### Motor Control Modes

Available control modes defined in `serial_struct.hpp`:
- `MODE_POSITION` (0x80) - Position control
- `MODE_VELOCITY` (0x81) - Velocity control  
- `MODE_TORQUE` (0x82) - Torque control
- `MODE_POS_VEL_TQE` (0x90) - Combined position/velocity/torque control
- Various PID parameter control modes

## Running Examples

After building, examples can be run from the build directory:

```bash
# Run motor control demo
./motor_run

# Subscribe to motor status messages via LCM
./motor_msg_subscriber
```

The `motor_run` example demonstrates basic motor control with alternating position commands and real-time status feedback.

## Python Bindings

Python bindings are available using pybind11. To build and use:

### Build Python Module

```bash
# Install pybind11
pip install pybind11

# Build with CMake (recommended)
mkdir -p build
cd build
cmake ..
make
```

If pybind11 is found, the Python module `hightorque_robot_py.so` will be built in the `python/` directory.

### Python Usage

```python
import hightorque_robot_py as hr

# Create robot and enable LCM
robot = hr.Robot()
robot.lcm_enable()

# Control motors
for motor in robot.Motors:
    motor.pos_vel_MAXtqe(0.5, 0.1, 10.0)  # position, velocity, max_torque
robot.motor_send_2()

# Read motor states
for motor in robot.Motors:
    state = motor.get_current_motor_state()
    print(f"Motor {state.ID}: pos={state.position:.3f}")
```

### Python Examples

```bash
# Run interactive Python demo
cd python/
python example_motor_control.py
```

The Python bindings provide full access to robot and motor control functionality with the same API as the C++ library.