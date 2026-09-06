# 代码练习

> 跟着官方例程写的代码，每个文件都带逐行注释。

## 目录

| 目录/文件 | 说明 | 对应笔记 |
|-----------|------|----------|
| `01-arm-control/arm_state_monitor.cpp` | H1-2 手臂与腰部 15 关节只读状态监视器 | `notes/05-手臂控制/02-只读手臂状态监视器.md` |

后续目录按抓取任务路线逐步增加，不保留尚未实现的占位代码目录。

## 编译方法

每个子目录使用独立 `CMakeLists.txt`。当前手臂控制示例需要显式指定本机 `unitree_sdk2` 源码目录：

```bash
cd ~/h1-2-learning
cmake -S code/01-arm-control \
      -B code/01-arm-control/build \
      -DUNITREE_SDK2_ROOT=/home/yingdongp/unitree_sdk2
cmake --build code/01-arm-control/build --target arm_state_monitor
```

## ⚠️ 安全提醒

- 运行只读程序前，仍需核对可执行文件和源码中不存在 Publisher/`Write()`。
- 任何运动代码必须先通过离线轨迹、限位和碰撞检查，并建立本次动作的测试卡。
- 实机运动测试前确认吊绳、实体急停、遥控器和人员隔离区，不用软件停止代替实体急停。
