# 宇树 H1-2 开发学习笔记

> 从零开始学习宇树人形机器人 H1-2 的开发，记录学习过程、问题解答和代码实践。

## 📌 项目简介

本项目是我（编程小白 + Linux 小白 + 机器人开发零基础）学习宇树 H1-2 开发的完整记录。

目标：**一步一步掌握 H1-2 的二次开发，从环境搭建到实机控制。**

## 📂 目录结构

```
h1-2-learning/
├── README.md              # 你正在看的文件
├── ROADMAP.md             # 学习路线图（按阶段规划）
├── notes/                 # 学习笔记（按主题分类）
│   ├── 00-环境搭建/       # Linux、SDK 安装、网络配置
│   ├── 01-SDK基础/        # SDK 架构、目录结构、编译
│   ├── 02-通信协议/       # DDS、话题、数据结构
│   ├── 03-底层控制/       # 电机控制、关节控制
│   ├── 04-高层控制/       # 运动控制、步态、手臂
│   └── 99-概念速查/       # 基础知识速查表
├── qa/                    # 问题与解答（随时问，随时记）
├── code/                  # 代码练习（带逐行注释）
└── resources/             # 官方资源链接汇总
```

## 🔗 官方资源

| 资源 | 链接 |
|------|------|
| 官方文档中心 | https://support.unitree.com/home/zh/H1_developer |
| C++ SDK (unitree_sdk2) | https://github.com/unitreerobotics/unitree_sdk2 |
| Python SDK | https://github.com/unitreerobotics/unitree_sdk2_python |
| 宇树开源页 | https://www.unitree.com/cn/opensource/ |

## 🚀 GitHub 使用入门（小白版）

因为你说要顺便学 GitHub，这里把最常用的操作记下来。

### 基本概念

- **仓库（Repository）**：就是这个项目文件夹，Git 帮你管理所有文件的历史版本。
- **提交（Commit）**：一次"保存快照"，记录你改了什么。
- **推送（Push）**：把本地的提交上传到 GitHub 网站。
- **拉取（Pull）**：把 GitHub 上的最新内容下载到本地。

### 最常用的 5 条命令

```bash
# 1. 查看当前状态（改了哪些文件）
git status

# 2. 把改动的文件加入"暂存区"（准备保存）
git add .                          # 添加所有改动
git add notes/01-SDK基础/xxx.md    # 只添加某个文件

# 3. 提交（保存快照，写一句说明）
git commit -m "添加 SDK 目录结构笔记"

# 4. 推送到 GitHub
git push origin main

# 5. 从 GitHub 拉取最新内容
git pull origin main
```

### 典型工作流

```
改文件 → git add . → git commit -m "说明" → git push
```

就这四步，记住就够用了。

### 第一次推送需要做的事

1. 在 GitHub 网站上创建一个空仓库（不要勾选 README）
2. 复制仓库地址，比如 `https://github.com/你的用户名/h1-2-learning.git`
3. 在本地项目目录执行：
   ```bash
   git remote add origin https://github.com/你的用户名/h1-2-learning.git
   git push -u origin main
   ```

## 📖 如何使用这个项目

1. **跟着 ROADMAP.md 走**：每完成一个阶段，打个勾
2. **读笔记**：每个知识点都有对应的笔记，看不懂的地方随时问
3. **问问题**：任何疑问都可以问，解答后会归档到 `qa/` 目录
4. **写代码**：跟着例程写，代码放在 `code/` 目录，带逐行注释

---

*开始日期：2026-08-23*
*当前阶段：环境搭建 + SDK 基础*
