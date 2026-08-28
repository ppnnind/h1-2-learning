# 里程碑：跑通第一个例程，理解 DDS 通信

**日期**：2026-08-28
**对应阶段**：阶段 1 - SDK 基础
**官方文档**：[H1 快速开始](https://support.unitree.com/home/zh/H1_developer/start)
**状态**：✅ 已完成

---

## 1. 这次做了什么

1. 克隆并编译了 `unitree_sdk2`
2. 编译了 `helloworld` 例程（发布者 + 订阅者）
3. 实际跑通了 DDS 通信：发布者每秒发消息，订阅者收到并打印
4. 验证了"发布/订阅"机制 —— **这是机器人控制的地基**

---

## 2. 遇到的两个坑（重要，下次别踩）

### 坑 1：可执行文件不在源码目录

- **现象**：`./publisher` 报 `No such file or directory`
- **原因**：SDK 的 CMake 配置把所有编译产物输出到 `build/bin/` 目录，不在 `build/example/helloworld/`
- **解决办法**：用 `find ~/unitree_sdk2/build -name "test_publisher"` 找到真实路径
- **教训**：编译产物位置要看 CMake 配置，不要猜

### 坑 2：可执行文件名字不是源码文件名

- **现象**：源码叫 `publisher.cpp`，但可执行文件叫 `test_publisher`
- **原因**：`example/helloworld/CMakeLists.txt` 里 `add_executable(test_publisher ...)` 指定了目标名
- **教训**：可执行文件名看 `CMakeLists.txt` 里的 `add_executable()`，不是看 `.cpp` 文件名

---

## 3. 例程代码核心解读

### 发布者（publisher.cpp）—— 电台主播

```cpp
// 1. 接通通信总机（所有 SDK 程序的第一步）
ChannelFactory::Instance()->Init(0);

// 2. 创建发布者，指定频道 TopicHelloWorld
ChannelPublisher<HelloWorldData::Msg> publisher(TOPIC);

// 3. 启动发布
publisher.InitChannel();

// 4. 无限循环：造消息 → 发出去 → 睡1秒
while (true)
{
    HelloWorldData::Msg msg(当前时间, "HelloWorld.");
    publisher.Write(msg);   // 广播消息
    sleep(1);
}
```

### 订阅者（subscriber.cpp）—— 收音机

```cpp
// 1. 接通通信总机
ChannelFactory::Instance()->Init(0);

// 2. 创建订阅者，调同一个频道
ChannelSubscriber<HelloWorldData::Msg> subscriber(TOPIC);

// 3. 登记回调函数：收到消息就自动调用 Handler
subscriber.InitChannel(Handler);
```

关键概念：**回调函数（callback）** —— Handler 不是我们调用的，是系统收到消息时自动调用的。

---

## 4. 实际输出解读

```
userID:1787913499025, message:HelloWorld.
userID:1787913500026, message:HelloWorld.   ← 每秒一条
reseted. sleep 3                             ← 订阅者代码里"关→停→开"的演示
userID:1787913507028, message:HelloWorld.    ← 重开后继续收到
```

| 输出 | 对应代码 |
|------|---------|
| `userID` | 发布时填的第一个参数（毫秒时间戳） |
| `message` | 发布时填的第二个参数 |
| `reseted` | subscriber 里的 `CloseChannel()` + sleep 演示 |

---

## 5. 和真实机器人控制的联系（重点）

这套机制和真实控制 H1-2 **完全一样**，只是换了两样东西：

| 项目 | helloworld 例程 | 真实控制 H1-2 |
|------|----------------|---------------|
| 话题名 | `TopicHelloWorld` | `rt/lowcmd`（发指令）、`rt/lowstate`（收状态） |
| 消息格式 | `HelloWorldData::Msg` | `LowCmd_`（指令）、`LowState_`（状态） |
| 一端 | publisher/subscriber 两个程序 | 机器人 / 电脑 |

**结论**：我已经理解了 H1-2 控制的通信本质 —— 电脑发 `LowCmd_` 到 `rt/lowcmd`，机器人通过 `rt/lowstate` 回报 `LowState_`。

---

## 6. 下一步

- [ ] 读 `example/h1/README_zh.md`，了解 H1 例程总览
- [ ] 学习 `idl/` 下的 `LowCmd_.idl` 和 `LowState_.idl`（数据结构）
- [ ] 跑通 H1 高层控制例程（接实机前先挂吊绳）

---

## 7. 相关链接

- [SDK 仓库](https://github.com/unitreerobotics/unitree_sdk2)
- [官方文档 - 快速开始](https://support.unitree.com/home/zh/H1_developer/start)
