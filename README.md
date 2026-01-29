# HTDW4438-OpenDog

## English Version

### 🐾 Project Overview

**HTDW4438-OpenDog** is a full-stack open-source quadruped robot project. It provides a comprehensive framework ranging from mechanical URDF design to advanced reinforcement learning (RL) locomotion control. The project is a specialized iteration based on the **HighTorque HTM5046** open-source platform.

### 🚀 Key Features

* **Agile Locomotion**: Developed based on the **HIMloco** (Hybrid Internal Model) algorithm for robust movement.
* **High-Performance Training**: Utilizes **NVIDIA Isaac Gym** for accelerated RL training.
* **Sim-to-Real Ready**: Includes a specialized `livelybot_sdk` and deployment scripts to bridge the gap between simulation and physical hardware.
* **Documentation**: Comprehensive technical notes and progress tracking via Feishu Wiki.

### 📂 Repository Structure

#### 1. HTDW4438-OpenDog (Main Repo)

* **`1.Hardware/`**: Contains URDF models (V1.5) and mechanical assembly files.
* **`2.Software/livelybot_sdk/`**: Core SDK for motor control and communication.
* `motor_set_zero.sh`: Calibration script for motor zeroing.
* `canboard_update.sh`: Firmware update tool for CAN control boards.


* **`3.Document/`**: Includes BOM (Bill of Materials) and motor technical manuals.
* **`4.Paper/`**: Reference research papers including HIMloco.

#### 2. [Lain_isaacgym](https://github.com/Lain-Ego0/Lain_isaacgym) (Training Framework)

* **`legged_gym/`**: Core environment for quadruped training.
* **`deploy/`**: Scripts and tools for exporting models to the real robot.
* **`resources/robots/`**: Contains the `htdw_4438.urdf` used in simulation.
* **`Lain_env.yml`**: Conda environment configuration for easy setup.

### 🛠️ Quick Start

1. **Environment Setup**:
```bash
conda env create -f Lain_env.yml
```


2. **Training**: Navigate to the `Lain_isaacgym` directory and start the RL training process using PPO.
3. **Hardware Deployment**: Use the provided shell scripts in `2.Software/` to initialize and test the motors.

### 🔗 Links

* **Technical Wiki**: [Lain's Note Library (Feishu)](https://www.google.com/search?q=https://wcn9j5638vrr.feishu.cn/wiki/space/757098375279517715)
* **Project Predecessor**: [HighTorque HTM5046](https://www.google.com/search?q=https://www.hightorque.cn/)

---

## 中文版本

### 🐾 项目简介

**HTDW4438-OpenDog** 是一个全栈开源四足机器人项目。本项目提供了从机械 URDF 设计到高级强化学习 (RL) 运动控制的完整框架。该项目是在 **高擎机电 HTM5046** 开源平台基础上进行的针对性迭代与优化。

### 🚀 核心特性

* **敏捷运动能力**：基于 **HIMloco** (Hybrid Internal Model) 算法开发，实现稳健的运动表现。
* **高性能训练**：利用 **NVIDIA Isaac Gym** 进行硬件加速的强化学习训练。
* **仿真到实机 (Sim-to-Real)**：集成 `livelybot_sdk` 和部署脚本，打通仿真环境与物理实机的部署链路。
* **详尽文档**：通过飞书 Wiki 维护完整的技术笔记和项目进度。

### 📂 仓库目录结构

#### 1. HTDW4438-OpenDog (主仓库)

* **`1.Hardware/`**：包含 URDF 模型 (V1.5) 及机械组装文件。
* **`2.Software/livelybot_sdk/`**：电机控制与通信的核心 SDK。
* `motor_set_zero.sh`：电机校准与归零脚本。
* `canboard_update.sh`：CAN 主控板固件更新工具。


* **`3.Document/`**：包含 BOM 表、电机技术手册及 CAN 控制资料。
* **`4.Paper/`**：核心参考论文，涵盖 HIMloco 等前沿研究。

#### 2. [Lain_isaacgym](https://github.com/Lain-Ego0/Lain_isaacgym) (训练框架)

* **`legged_gym/`**：四足机器人训练的核心环境代码。
* **`deploy/`**：用于将训练好的模型导出至实机的工具包。
* **`resources/robots/`**：仿真中使用的 `htdw_4438.urdf` 机器人描述文件。
* **`Lain_env.yml`**：Conda 环境配置文件，方便快速搭建开发环境。

### 🛠️ 快速开始

1. **环境配置**：
```bash
conda env create -f Lain_env.yml
```


2. **启动训练**：进入 `Lain_isaacgym` 目录，使用 PPO 算法开启强化学习训练。
3. **硬件部署**：利用 `2.Software/` 目录下的脚本对电机进行初始化测试。

### 🔗 相关链接

* **核心技术文档**：[Lain 的笔记库 (飞书 Wiki)](https://www.google.com/search?q=https://wcn9j5638vrr.feishu.cn/wiki/space/757098375279517715)
* **项目前身**：[高擎机电 HTM5046 开源版](https://www.google.com/search?q=https://www.hightorque.cn/)