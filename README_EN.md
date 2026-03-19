<p align="center">
  <img src="5.Images/V3.png" width="780" alt="HTDW4438-OpenDog CAD render" />
</p>

<h1 align="center">HTDW4438-OpenDog</h1>

<p align="center">
  <b>English</b> | <a href="README.md">简体中文</a>
</p>

<p align="center">
  Independent open-source quadruped project: hardware design, URDF, real-robot SDK, and Sim2Real collaboration
</p>

<p align="center">
  <a href="https://github.com/Lain-Ego0/HTDW4438-OpenDog">Main Repository (Current)</a> ·
  <a href="https://github.com/Lain-Ego0/HTDW4438-sim2real">Sim2Real</a> ·
  <a href="https://github.com/Lain-Ego0/HTDW4438_Isaacgym">Isaac Gym</a> ·
  <a href="https://github.com/Lain-Ego0/HTDW4438_HIMloco">HIMloco</a> ·
  <a href="https://github.com/Lain-Ego0/HTDW4438-AMP">AMP</a> ·
  <a href="https://github.com/Lain-Ego0/LocoWiki">LocoWiki (Docs)</a> ·
  <a href="https://locowiki.github.io/">LocoWiki (Website)</a>
</p>

---

## Contents

- Project Positioning
- Repository Topology (1+4)
- What This Repository Contains
- Quick Start
- Recommended Workflow
- Notes and Scope

## Project Positioning

`HTDW4438` is a **fully independent** quadruped robot project. It is **not** an iteration of the HighTorque (高擎机电) HTM5046 platform.

`HTDW4438-OpenDog` is the repository for robot-body resources, including:

- Mechanical and URDF assets
- Real-robot SDK and utility scripts
- Documentation and reference papers

If you are new to this project family, start from the current main repository: [`HTDW4438-OpenDog`](https://github.com/Lain-Ego0/HTDW4438-OpenDog).

## Repository Topology (1+4)

| Role | Repository | Purpose |
|---|---|---|
| Main repository (current) | [`HTDW4438-OpenDog`](https://github.com/Lain-Ego0/HTDW4438-OpenDog) | Robot body assets, URDF, real-robot SDK, and engineering references |
| Subordinate repository | [`HTDW4438-sim2real`](https://github.com/Lain-Ego0/HTDW4438-sim2real) | Sim2Real integration flow and deployment pipeline |
| Subordinate repository | [`HTDW4438_Isaacgym`](https://github.com/Lain-Ego0/HTDW4438_Isaacgym) | Isaac Gym training framework and baseline policy experiments |
| Subordinate repository | [`HTDW4438_HIMloco`](https://github.com/Lain-Ego0/HTDW4438_HIMloco) | HIMloco-related algorithm experiments and implementations |
| Subordinate repository | [`HTDW4438-AMP`](https://github.com/Lain-Ego0/HTDW4438-AMP) | AMP-based motion-prior and policy training experiments |
| Documentation repository (independent) | [`LocoWiki`](https://github.com/Lain-Ego0/LocoWiki) | Project knowledge base, tutorials, and long-term notes |
| Website repository (independent) | [`LocoWiki.github.io`](https://github.com/Lain-Ego0/LocoWiki.github.io) | Website source for LocoWiki, published at <https://locowiki.github.io/> |

## What This Repository Contains

```text
HTDW4438-OpenDog
├── 1.Hardware/
│   ├── htdw_4438_urdf/               # Robot URDF package (meshes/, urdf/, launch/)
│   ├── HTDW4438-OpenDogV1.5/         # Structural iteration files
│   └── HTDW4438-OpenDog（URDF）/      # Legacy/supplemental URDF resources
├── 2.Software/
│   └── livelybot_sdk/                # Real-robot SDK and utility scripts
├── 3.Document/                       # Manuals, references, and hardware docs
├── 4.Paper/                          # Reference papers
├── 5.Images/                         # Project images
└── assets/                           # README assets
```

## Quick Start

### 1) Clone

```bash
git clone https://github.com/Lain-Ego0/HTDW4438-OpenDog.git
cd HTDW4438-OpenDog
```

### 2) Inspect URDF assets

- Main URDF: `1.Hardware/htdw_4438_urdf/urdf/htdw_4438.urdf`
- Mesh directory: `1.Hardware/htdw_4438_urdf/meshes/`
- Launch files: `1.Hardware/htdw_4438_urdf/launch/`

### 3) Use real-robot SDK scripts

- SDK location: `2.Software/livelybot_sdk/`
- Common scripts:
  - `motor_set_zero.sh`: motor zeroing/calibration
  - `motor_move_zero.sh`: move joints to zero pose
  - `motor_feedback.sh`: check motor feedback
  - `canboard_update.sh`: CAN board firmware update
- Documentation:
  - `2.Software/livelybot_sdk/readme.md`
  - `2.Software/livelybot_sdk/doc/yaml配置文件说明.md`
  - `2.Software/livelybot_sdk/doc/机器人launch文件说明.md`

> Safety note: Before power-on, zeroing, or bring-up, lift the robot off the ground and keep an emergency-stop ready.

## Recommended Workflow

1. Maintain robot structure, URDF, base parameters, and SDK resources in this repository.
2. Run training and algorithm comparisons in the subordinate repositories (Isaac Gym / HIMloco / AMP).
3. Use subordinate repository `HTDW4438-sim2real` for Sim2Real integration and deployment.
4. Maintain documentation content in `LocoWiki` and website publishing assets in `LocoWiki.github.io`.

## Notes and Scope

- This repository focuses on robot-body assets and documentation, not the full training pipeline.
- If docs conflict across repositories, use `HTDW4438-OpenDog` as source of truth; use `HTDW4438-sim2real` for deployment-specific details.
- Tutorials and accumulated engineering know-how are primarily maintained in `LocoWiki`, while website presentation is maintained in `LocoWiki.github.io` (<https://locowiki.github.io/>).
- `3.Document/` may include supplier or third-party references for engineering lookup only; these do not define the project's origin platform.
