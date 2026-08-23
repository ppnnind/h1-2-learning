# SDK 开发指南导读：目录结构全景

**日期**：2026-08-23
**对应阶段**：阶段 1 - SDK 基础
**官方文档**：[获取 SDK](https://support.unitree.com/home/zh/H1_developer/Get_SDK)、[H1 快速开始](https://support.unitree.com/home/zh/H1_developer/start)
**状态**：已完成

---

## 1. 这是什么（一句话概括）

这篇笔记帮你搞清楚 `unitree_sdk2` 这个仓库里**每个文件夹是干什么的**，以及你应该**按什么顺序去看**。重点讲两个目录：
- `include/` —— **软件接口服务目录**（你能用的所有"工具"都在这里）
- `example/` —— **参考例程目录**（官方写好的"作业参考答案"）

---

## 2. 先搞懂：SDK 到底是个啥？

用大白话说：

> SDK 就是宇树给你的一个**"遥控器工具箱"**。
>
> 机器人本身很复杂，你不可能直接去操作每个电机的电信号。宇树把这些复杂操作封装成了一个个**函数**，你调用函数就能控制机器人。这些函数的集合 + 通信机制 + 示例代码，就是 SDK。

类比一下：
- 你买了个单反相机，SDK 就是相机的**说明书 + 遥控器**
- `include/` 是说明书里的**功能列表**（告诉你有哪些按钮）
- `example/` 是**拍摄示例**（告诉你怎么用这些按钮拍好人像）

### 两个核心概念：高层控制 vs 底层控制

| | 高层控制（High-level） | 底层控制（Low-level） |
|---|---|---|
| **你说的话** | "向前走"、"站起来" | "左腿第3个电机转到30度" |
| **难度** | 简单，调用现成功能 | 难，需要懂运动学 |
| **灵活度** | 低，只能用官方给的动作 | 高，想怎么控就怎么控 |
| **对应目录** | `example/h1/high_level/` | `example/h1/low_level/` |
| **适合阶段** | 初期先跑通 | 后期深入开发 |

**建议**：先从高层控制例程跑通，建立信心；再去啃底层控制。

---

## 3. SDK 仓库总览

把仓库克隆下来后，你会看到这些：

```
unitree_sdk2/
├── include/          ← 【软件接口】所有头文件，告诉你"能调用什么"
├── example/          ← 【参考例程】官方写好的示例代码
├── lib/              ← 预编译的库文件（不用管，直接用）
├── thirdparty/       ← 第三方依赖库（不用管）
├── cmake/            ← CMake 辅助脚本（不用管）
├── CMakeLists.txt    ← 编译配置文件（要看懂）
└── README.md         ← 项目说明
```

**你只需要关注前两个**：`include/` 和 `example/`。其他的在现阶段都是"黑盒"，知道存在就行。

---

## 4. 软件接口服务目录：`include/` 详解

这是 SDK 的**核心**，所有你能调用的函数、能使用的数据结构都定义在这里。

```
include/unitree/
├── common/           # 通用工具（暂时不用看）
├── dds_wrapper/      # 【重要】DDS 通信封装
├── idl/              # 【重要】数据结构定义
└── robot/            # 【重要】各机器人的控制接口
    ├── channel/      #   通信通道（发布/订阅）
    ├── client/       #   客户端基类
    ├── h1/loco/      #   H1 运动控制接口（高层）
    ├── g1/           #   G1 机器人接口
    ├── go2/          #   Go2 机器人接口
    └── ...           #   其他型号
```

### 4.1 `idl/` —— 数据结构定义（最基础）

**这是什么**：IDL（Interface Definition Language）文件定义了**数据长什么样**。

比如 `LowCmd_.idl` 定义了"控制指令"这个数据包里有哪些字段：
- 每个电机的目标位置 `q`
- 目标速度 `dq`
- 目标力矩 `tau`
- 刚度 `kp`、阻尼 `kd`
- ...

`LowState_.idl` 定义了"机器人状态"这个数据包里有什么：
- 每个电机当前的位置、速度、力矩
- IMU（陀螺仪）数据
- 电池状态
- ...

**为什么重要**：你和机器人通信，本质就是**发 LowCmd 包，收 LowState 包**。不了解数据结构，就不知道该填什么、该读什么。

**怎么看**：先找到 `LowCmd_.idl` 和 `LowState_.idl`，大概浏览一下字段名，不用全记住，用到的时候回来查。

### 4.2 `dds_wrapper/` —— 通信封装

**这是什么**：DDS 是电脑和机器人之间的**通信协议**。这个目录把 DDS 的复杂操作封装成了简单的类。

**核心类**：
- `ChannelPublisher` —— 发布者（往机器人发数据）
- `ChannelSubscriber` —— 订阅者（从机器人收数据）

**类比**：
- Publisher 像**电台主播**，往一个频道（话题）发消息
- Subscriber 像**收音机**，调到同一个频道就能收到消息
- 话题名（Topic）就是**频道频率**，比如 `"rt/lowcmd"` 是控制指令频道

**现阶段**：知道有这两个类就行，具体用法看例程。

### 4.3 `robot/channel/` —— 通信通道

**这是什么**：在 dds_wrapper 基础上，进一步封装了**机器人专用的通信通道**。

你会在例程里看到这样的代码：
```cpp
// 创建一个发布者，往 "rt/lowcmd" 话题发 LowCmd_ 数据
auto lowcmd_publisher = std::make_shared<unitree::robot::ChannelPublisher<unitree_go::msg::dds_::LowCmd_>>("rt/lowcmd");
lowcmd_publisher->InitChannel();

// 创建一个订阅者，监听 "rt/lowstate" 话题的 LowState_ 数据
auto lowstate_subscriber = std::make_shared<unitree::robot::ChannelSubscriber<unitree_go::msg::dds_::LowState_>>("rt/lowstate");
```

不用完全看懂，先有个印象：**发指令用 Publisher，收状态用 Subscriber**。

### 4.4 `robot/h1/loco/` —— H1 高层运动控制

**这是什么**：H1 机器人的**高层控制接口**，比如让机器人站起来、走路、切换状态。

核心类是 `LocoClient`，提供这些功能：
- `SetFsmId()` —— 切换状态（比如站立、行走、阻尼）
- 速度控制 —— 让机器人以指定速度行走

**注意**：H1-2 和 H1 共用这个目录的接口。H1-2 的区别主要在电机数量（H1 是19个，H1-2 是27个，多了手腕和脚踝的并联机构）。

### 4.5 `include/` 阅读顺序建议

```
第1步：idl/ 下的 LowCmd_.idl 和 LowState_.idl
       → 知道"发什么、收什么"

第2步：robot/channel/ 下的头文件
       → 知道"怎么发、怎么收"

第3步：robot/h1/loco/ 下的头文件
       → 知道"高层控制有哪些功能"

第4步：dds_wrapper/ （可选，想深入通信机制再看）
```

---

## 5. 参考例程目录：`example/` 详解

这是**最好的学习资料**。官方写了大量示例代码，你要做的就是：**读例程 → 跑例程 → 改例程**。

```
example/
├── h1/               ← 【重点】H1/H1-2 的例程
│   ├── high_level/   #   高层控制例程（运动、手臂）
│   ├── low_level/    #   底层控制例程（电机直接控制）
│   ├── doc/images/   #   文档图片
│   ├── CMakeLists.txt
│   └── README_zh.md  #   中文说明（先看这个！）
├── h2/               # H2 机器人例程
├── g1/               # G1 机器人例程
├── go2/              # Go2 四足机器人例程
├── b2/               # B2 机器人例程
├── helloworld/       # 【入门】最简单的 DDS 通信示例
├── state_machine/    # 状态机示例
├── wireless_controller/  # 无线遥控器示例
└── CMakeLists.txt
```

### 5.1 `helloworld/` —— 你的第一个例程

**这是什么**：最简单的 DDS 通信示例，不涉及机器人控制，只演示"怎么发消息、怎么收消息"。

**为什么先看这个**：它把通信机制剥离出来，代码量少，容易看懂。理解了这个，再看机器人控制例程就不会被通信代码搞晕。

**包含**：
- `test_publisher.cpp` —— 发布者示例
- `test_subscriber.cpp` —— 订阅者示例

### 5.2 `h1/high_level/` —— 高层控制例程

**这是什么**：使用官方高层接口控制 H1 的示例。

典型例程：
- `h1_loco_example_client` —— 运动控制（站立、行走）
- `h1_arm_sdk_dds_example` —— 手臂控制

**特点**：
- 代码量适中
- 调用 `LocoClient` 等高层类
- 适合**第一次接实机**测试

### 5.3 `h1/low_level/` —— 底层控制例程

**这是什么**：直接控制电机的示例，最灵活也最危险。

典型例程：
- `h1_low_level_example` —— 底层控制基础例程
- `h1_2_ankle_track` —— H1-2 脚踝并联机构控制（H1-2 特有）

**特点**：
- 直接操作 `LowCmd_` 数据结构
- 可以控制每个电机的位置/速度/力矩
- **⚠️ 危险**：参数填错可能导致机器人剧烈动作，一定要挂吊绳！

### 5.4 `example/` 阅读顺序建议

```
第1步：helloworld/
       → 理解 DDS 发布/订阅机制（不接机器人也能跑）

第2步：h1/README_zh.md
       → 了解 H1 例程的整体说明

第3步：h1/high_level/ 下的例程
       → 跑通高层控制，建立信心（接实机，挂吊绳）

第4步：h1/low_level/ 下的例程
       → 深入底层电机控制（接实机，务必挂吊绳+小幅度测试）
```

---

## 6. 两个目录的对应关系

把 `include/` 和 `example/` 对照起来看，你会发现：

| include/ 里的接口 | example/ 里的例程 |
|---|---|
| `dds_wrapper/` + `robot/channel/` | `helloworld/` |
| `robot/h1/loco/`（LocoClient） | `h1/high_level/h1_loco_example_client` |
| `idl/LowCmd_.idl` + `idl/LowState_.idl` | `h1/low_level/h1_low_level_example` |

**学习方法**：看例程代码时，遇到不认识的类或函数，就去 `include/` 里找对应的头文件，看注释和声明。

---

## 7. 你现在应该做什么

按照这个顺序行动：

1. **克隆 SDK 仓库**
   ```bash
   git clone https://github.com/unitreerobotics/unitree_sdk2
   ```

2. **浏览目录结构**，对照这篇笔记，确认每个文件夹的位置

3. **读 `example/h1/README_zh.md`**，了解 H1 例程说明

4. **读 `example/helloworld/` 里的两个文件**，理解发布/订阅

5. **尝试编译整个 SDK**（遇到报错随时问）
   ```bash
   cd unitree_sdk2
   mkdir build && cd build
   cmake ..
   make
   ```

6. **编译成功后**，我们再一步步跑例程

---

## 8. 常见坑

- **坑1：看到这么多文件就慌了**
  - 不用慌，90%的文件你现阶段不需要看。只关注 `include/unitree/idl/`、`include/unitree/robot/channel/`、`include/unitree/robot/h1/` 和 `example/h1/`、`example/helloworld/` 就够了。

- **坑2：纠结 C++ 语法看不懂**
  - 正常。先抓主干：这个例程**创建了什么对象 → 调用了什么函数 → 实现了什么效果**。具体语法细节遇到再查。

- **坑3：想一次全搞懂再动手**
  - 错。机器人开发是**实践驱动**的，先跑起来，再逐步理解。跑通一个例程比看10篇文档有用。

---

## 9. 我的疑问

- [ ] H1-2 和 H1 在 SDK 层面的具体差异除了电机数量还有什么？
- [ ] `MotionSwitcherClient` 是干什么的？（在 low_level 例程里看到）
- [ ] 脚踝并联机构的 PR Mode 具体怎么切换？

---

## 10. 相关链接

- [SDK 仓库](https://github.com/unitreerobotics/unitree_sdk2)
- [官方文档 - 获取 SDK](https://support.unitree.com/home/zh/H1_developer/Get_SDK)
- [官方文档 - 快速开始](https://support.unitree.com/home/zh/H1_developer/start)
- [DeepWiki - SDK 代码解读](https://deepwiki.com/unitreerobotics/unitree_sdk2)（AI 生成的代码导航，很有用）
