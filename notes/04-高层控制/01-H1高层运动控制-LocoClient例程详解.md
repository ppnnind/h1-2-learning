# H1 高层运动控制：LocoClient 例程详解

**日期**：2026-08-29
**对应阶段**：阶段 4 - 高层控制
**源码位置**：`example/h1/high_level/h1_loco_client_example.cpp`（299 行）
**官方文档**：[H1 快速开始](https://support.unitree.com/home/zh/H1_developer/start)
**状态**：✅ 已完成精读

---

## 1. 整体定位

这个程序是 H1 的**高层命令行遥控器**：在终端输入 `--stand_up`、`--move="0.5 0 0"` 这样的参数，它就调用 `LocoClient` 的对应方法，让机器人做动作。

**它不直接操作电机**（那是底层控制干的事），而是调用宇树封装好的高层接口——你说"站起来"，它就帮你站起来。

---

## 2. 代码结构总览

```
h1_loco_client_example.cpp
├── 开头：include + stringToFloatVector 辅助函数
├── main()
│   ├── 初始化 args 字典（默认 network_interface=lo）
│   ├── 解析命令行参数（--key=value 格式）
│   ├── 【核心】初始化通信 + 创建 LocoClient
│   └── 大循环：遍历每个参数，调用对应的 client 方法
│       ├── 查询类（Get 开头）
│       ├── 设置类（Set 开头）
│       ├── 动作类（damp/start/stand_up/move...）
│       └── 其他（odom/gait/foot/task）
```

---

## 3. 开头部分

### 3.1 include

```cpp
#include <chrono>          // C++ 标准库：计时
#include <iostream>        // C++ 标准库：输入输出
#include <thread>          // C++ 标准库：线程/睡眠

#include <unitree/robot/h1/loco/h1_loco_api.hpp>      // H1 运动控制接口定义
#include <unitree/robot/h1/loco/h1_loco_client.hpp>   // LocoClient 类实现
```

**关键区别**：helloworld 用的是 `channel_publisher.hpp`（自己发 DDS 消息），这里用的是 `h1_loco_client.hpp`——宇树已经把发消息的细节封装好了，你只要调方法就行。**这就是"高层控制"的含义。**

### 3.2 辅助函数

```cpp
std::vector<float> stringToFloatVector(const std::string &str)
```

把字符串 `"0.5 0 0"` 转成浮点数数组 `[0.5, 0, 0]`。因为命令行参数都是字符串，而控制接口要的是数字。不用深究，知道作用即可。

---

## 4. 命令行参数解析（略读）

```cpp
std::map<std::string, std::string> args = {{"network_interface", "lo"}};
// 解析 --key=value 格式的参数
```

作用：把终端输入的 `--stand_up --move="0.5 0 0"` 解析成字典：

```
args = {
    "network_interface": "lo",
    "stand_up": "",
    "move": "0.5 0 0"
}
```

这部分是纯 C++ 字符串处理，和机器人控制无关，知道作用即可。

---

## 5. 核心初始化（重点！）

```cpp
// 第 1 步：初始化通信（注意第二个参数！）
unitree::robot::ChannelFactory::Instance()->Init(0, args["network_interface"]);

// 第 2 步：创建遥控器
unitree::robot::h1::LocoClient client;

// 第 3 步：启动遥控器
client.Init();
client.SetTimeout(10.f);
```

### 5.1 为什么 Init 多了一个参数？

| 场景 | Init 写法 | 原因 |
|------|----------|------|
| helloworld（同电脑） | `Init(0)` | 用 `lo` 回环网卡，自己跟自己说话 |
| 控制真实机器人 | `Init(0, 网卡名)` | 电脑和机器人是两台设备，必须指定用哪张网卡通信 |

**这就是官方文档反复强调"运行例程要传网卡名"的原因。**

### 5.2 LocoClient 是什么？

`LocoClient` = 运动控制客户端，是你的"遥控器"。它内部封装了 DDS 通信，你不用管发什么话题、填什么数据结构，只要调 `client.StandUp()` 这种方法。

### 5.3 SetTimeout

`client.SetTimeout(10.f)` = 发命令后最多等机器人回应 10 秒，超时就报错。

---

## 6. 所有功能分类

后面 200 多行全是 `if (arg_pair.first == "xxx") { client.XXX(); }` 结构。按功能分四类：

### 6.1 查询类（Get 开头）—— 读状态，不改变机器人

| 命令 | 方法 | 含义 |
|------|------|------|
| `get_fsm_id` | `GetFsmId(fsm_id)` | 读当前状态机编号 |
| `get_fsm_mode` | `GetFsmMode(fsm_mode)` | 读当前模式 |
| `get_balance_mode` | `GetBalanceMode(...)` | 读平衡模式 |
| `get_swing_height` | `GetSwingHeight(...)` | 读抬腿高度 |
| `get_stand_height` | `GetStandHeight(...)` | 读站立高度 |
| `get_phase` | `GetPhase(phase)` | 读步态相位 |
| `get_odom` | `GetOdom(x,y,yaw)` | 读里程计（位置+朝向） |

### 6.2 设置类（Set 开头）—— 改参数

| 命令 | 方法 | 含义 |
|------|------|------|
| `set_fsm_id=5` | `SetFsmId(5)` | 切换状态机（核心！） |
| `set_balance_mode=1` | `SetBalanceMode(1)` | 切换平衡模式 |
| `set_swing_height=0.1` | `SetSwingHeight(0.1)` | 设置抬腿高度 |
| `set_stand_height=0.7` | `SetStandHeight(0.7)` | 设置站立高度 |
| `set_velocity="0.5 0 0 2"` | `SetVelocity(vx,vy,omega,duration)` | 以指定速度走 duration 秒 |
| `set_phase="0 0.5"` | `SetPhase(param)` | 设置步态相位 |
| `set_target_pos="1 0 0"` | `SetTargetPos(x,y,yaw,false)` | 走到绝对位置 |
| `set_target_pos_relative="1 0 0"` | `SetTargetPos(x,y,yaw)` | 走到相对位置 |

### 6.3 动作类 —— 直接做动作

| 命令 | 方法 | 含义 |
|------|------|------|
| `damp` | `Damp()` | 阻尼模式（电机松劲，可手动摆） |
| `start` | `Start()` | 开始运动（进入行走状态） |
| `stand_up` | `StandUp()` | 站起来 |
| `zero_torque` | `ZeroTorque()` | 零力矩（电机完全释放，⚠️危险） |
| `stop_move` | `StopMove()` | 停止移动 |
| `high_stand` | `HighStand()` | 高站立 |
| `low_stand` | `LowStand()` | 低站立 |
| `balance_stand` | `BalanceStand()` | 平衡站立 |
| `shake_hand` | `ShakeHand(0/1)` | 握手（开始/结束） |
| `wave_hand` | `WaveHand()` | 挥手 |
| `move="0.5 0 0"` | `Move(vx,vy,omega)` | 以指定速度移动 |

### 6.4 其他

| 命令 | 含义 |
|------|------|
| `enable_odom` / `disable_odom` | 开启/关闭里程计 |
| `continous_gait=true/false` | 连续步态开关 |
| `switch_move_mode=true/false` | 切换运动模式 |
| `set_next_foot=0/1` | 设置下一只迈的脚 |
| `set_task_id=1` | 设置任务 ID |

---

## 7. 关键概念：FSM 状态机（必须理解）

### 7.1 什么是 FSM？

**FSM = Finite State Machine（有限状态机）**。

机器人不是随时能做任何动作的——它有一组固定的状态，每个状态下只能做特定的事，状态之间通过 `SetFsmId` 切换。

> 类比：手机有"锁屏"、"主屏幕"、"相机"、"通话中"等状态。在"锁屏"状态下不能拍照，必须先解锁（切换状态）。

### 7.2 H1 的常见状态（fsm_id）

| fsm_id | 状态 | 含义 |
|--------|------|------|
| 1 | Damp | 阻尼模式（电机松劲） |
| 2 | StandUp | 站起来 |
| ... | ... | 中间过渡状态 |
| 204 | Start | 开始行走 |

> 具体编号以官方文档为准，可用 `get_fsm_id` 查询当前状态。

### 7.3 为什么重要？—— 操作顺序必须正确

```
damp（松劲）→ stand_up（站起来）→ start（进入行走）→ move（走路）
```

如果机器人还躺着，直接 `move`，它不会理你（因为不在行走状态）。

---

## 8. 实际运行方法

编译产物在 `build/bin/` 目录。

```bash
# 1. 查网卡名
ifconfig
# 或 ip addr

# 2. 让机器人站起来（假设网卡是 enp3s0）
./h1_loco_client_example --network_interface=enp3s0 --stand_up

# 3. 开始行走
./h1_loco_client_example --network_interface=enp3s0 --start

# 4. 以 0.5m/s 向前走 2 秒
./h1_loco_client_example --network_interface=enp3s0 --set_velocity="0.5 0 0 2"

# 5. 停止
./h1_loco_client_example --network_interface=enp3s0 --stop_move
```

### ⚠️ 安全提醒

- 第一次接实机**务必挂吊绳**
- 先从 `damp`、`get_fsm_id` 这种**只读/安全**命令开始，确认通信正常
- 再碰 `stand_up`、`move` 等动作命令
- `zero_torque` 会让电机完全释放，机器人可能直接瘫倒，慎用！

---

## 9. 和 helloworld 的对比（建立全局观）

| | helloworld | h1_loco_client_example |
|---|---|---|
| 控制层级 | 底层（自己发 DDS 消息） | 高层（调用封装好的方法） |
| 你写的代码 | `publisher.Write(msg)` | `client.StandUp()` |
| 通信对象 | 另一个程序（同电脑） | 真实机器人（通过网卡） |
| Init 参数 | `Init(0)` | `Init(0, 网卡名)` |
| 难度 | 理解通信机制 | 理解状态机和方法调用 |

---

## 10. 下一步

- [ ] 接实机测试 `get_fsm_id`（挂吊绳，确认通信）
- [ ] 测试 `damp` → `stand_up` → `start` → `move` 完整流程
- [ ] 学习 `LocoClient` 的头文件 `h1_loco_client.hpp`，了解所有可用方法
- [ ] 读手臂控制例程 `h1_2_arm_sdk_dds_example.cpp`

---

## 11. 相关链接

- [SDK 仓库](https://github.com/unitreerobotics/unitree_sdk2)
- [官方文档 - 快速开始](https://support.unitree.com/home/zh/H1_developer/start)
- [DeepWiki - H1 运动控制](https://deepwiki.com/unitreerobotics/unitree_sdk2/4.1-h1-locomotion-api)
