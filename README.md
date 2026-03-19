<p align="center">
  <img src="5.Images/V3.png" width="780" alt="HTDW4438-OpenDog CAD render" />
</p>

<h1 align="center">HTDW4438-OpenDog</h1>

<p align="center">
  <b>简体中文</b> | <a href="README_EN.md">English</a>
</p>

<p align="center">
  独立开源四足机器人项目：硬件设计、URDF、实机 SDK 与 Sim2Real 协同开发
</p>

<p align="center">
  <a href="https://github.com/Lain-Ego0/HTDW4438-OpenDog">主仓库（当前）</a> ·
  <a href="https://github.com/Lain-Ego0/HTDW4438-sim2real">Sim2Real</a> ·
  <a href="https://github.com/Lain-Ego0/HTDW4438_Isaacgym">Isaac Gym</a> ·
  <a href="https://github.com/Lain-Ego0/HTDW4438_HIMloco">HIMloco</a> ·
  <a href="https://github.com/Lain-Ego0/HTDW4438-AMP">AMP</a> ·
  <a href="https://github.com/Lain-Ego0/LocoWiki">LocoWiki（资料）</a> ·
  <a href="https://locowiki.github.io/">LocoWiki（网站）</a>
</p>

---

## 目录

- 项目定位
- 仓库体系（1+4）
- 本仓库内容
- 快速开始
- 推荐工作流
- 说明与边界

## 项目定位

`HTDW4438` 是一只**完全独立开发**的四足机器人，**不是**在高擎机电（HighTorque）HTM5046 平台上迭代而来。

`HTDW4438-OpenDog` 用于管理机器人本体相关资源，包括：

- 机械与 URDF 资产
- 实机控制 SDK 与脚本
- 文档资料与参考论文

如果你是第一次接触该项目，建议从当前主仓库 [`HTDW4438-OpenDog`](https://github.com/Lain-Ego0/HTDW4438-OpenDog) 开始。

## 仓库体系（1+4）

| 角色 | 仓库 | 用途 |
|---|---|---|
| 主仓库（当前） | [`HTDW4438-OpenDog`](https://github.com/Lain-Ego0/HTDW4438-OpenDog) | 机器人本体、URDF、实机 SDK 与工程资料 |
| 附属仓库 | [`HTDW4438-sim2real`](https://github.com/Lain-Ego0/HTDW4438-sim2real) | Sim2Real 集成流程与部署链路 |
| 附属仓库 | [`HTDW4438_Isaacgym`](https://github.com/Lain-Ego0/HTDW4438_Isaacgym) | Isaac Gym 训练框架与基础策略实验 |
| 附属仓库 | [`HTDW4438_HIMloco`](https://github.com/Lain-Ego0/HTDW4438_HIMloco) | HIMloco 相关算法实验与实现 |
| 附属仓库 | [`HTDW4438-AMP`](https://github.com/Lain-Ego0/HTDW4438-AMP) | AMP 相关运动先验与策略训练 |
| 资料仓库（独立） | [`LocoWiki`](https://github.com/Lain-Ego0/LocoWiki) | 项目知识库、教程、经验沉淀与说明文档 |
| 网站仓库（独立） | [`LocoWiki.github.io`](https://github.com/Lain-Ego0/LocoWiki.github.io) | LocoWiki 网站源码，对应站点：<https://locowiki.github.io/> |

## 本仓库内容

```text
HTDW4438-OpenDog
├── 1.Hardware/
│   ├── htdw_4438_urdf/               # 机器人 URDF 包（meshes/、urdf/、launch/）
│   ├── HTDW4438-OpenDogV1.5/         # 结构迭代文件
│   └── HTDW4438-OpenDog（URDF）/      # 历史/补充 URDF 资料
├── 2.Software/
│   └── livelybot_sdk/                # 实机 SDK 与常用脚本
├── 3.Document/                       # 手册、资料、硬件文档
├── 4.Paper/                          # 参考论文
├── 5.Images/                         # 项目图片
└── assets/                           # README 资源
```

## 快速开始

### 1) 获取仓库

```bash
git clone https://github.com/Lain-Ego0/HTDW4438-OpenDog.git
cd HTDW4438-OpenDog
```

### 2) 查看 URDF 资产

- URDF 主文件：`1.Hardware/htdw_4438_urdf/urdf/htdw_4438.urdf`
- Mesh 目录：`1.Hardware/htdw_4438_urdf/meshes/`
- Launch 文件：`1.Hardware/htdw_4438_urdf/launch/`

### 3) 使用实机 SDK 脚本

- SDK 目录：`2.Software/livelybot_sdk/`
- 常用脚本：
  - `motor_set_zero.sh`：电机置零/标定
  - `motor_move_zero.sh`：移动到零位
  - `motor_feedback.sh`：反馈查看
  - `canboard_update.sh`：CAN 板升级
- 文档说明：
  - `2.Software/livelybot_sdk/readme.md`
  - `2.Software/livelybot_sdk/doc/yaml配置文件说明.md`
  - `2.Software/livelybot_sdk/doc/机器人launch文件说明.md`

> 安全提示：上电、置零、联调前请先抬空机身并准备急停，避免误动作导致损坏。

## 推荐工作流

1. 在本仓库维护机器人结构、URDF、基础参数与 SDK 资料。
2. 在附属仓库（Isaac Gym / HIMloco / AMP）进行策略训练与对比实验。
3. 在附属仓库 `HTDW4438-sim2real` 进行 Sim2Real 集成与部署落地。
4. 在 `LocoWiki` 维护知识库内容，在 `LocoWiki.github.io` 维护网站发布相关内容。

## 说明与边界

- 本仓库聚焦本体资产与资料管理，不承载全部训练管线。
- 若跨仓库文档存在差异，以主仓库 `HTDW4438-OpenDog` 为准；部署细节可参考 `HTDW4438-sim2real`。
- 知识类教程与经验沉淀优先维护在 `LocoWiki`，网站展示与发布在 `LocoWiki.github.io`（<https://locowiki.github.io/>）。
- `3.Document/` 中可能包含供应链或第三方资料，仅用于工程备查，不代表项目来源平台。
