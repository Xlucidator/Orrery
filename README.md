# Orrery

Orrery 是一个基于 OpenGL 可编程管线的交互式光栅化图形实验项目，旨在提供可视化环境，用于探索图形渲染和物理效果。

### 功能概述

- **窗口管理**：基于 GLFW，支持窗口调整和交互回调
- **相机控制**：自由移动视角和锁定角色(Player)视角双模式
- **模型加载**：支持静态 (OBJ) 和动态 (DAE、FBX) 模型，带骨骼动画 (Assimp)
- **光照模型**：支持普通和 Blinn-Phong 光照
- **音效支持**：背景音乐及动作音效 (IrrKlang)
- **物理模拟**：静态/动态刚体，基于 PhysX，支持碰撞检测

### 项目结构

```
|--- assets       # 资源文件 (音频、模型、着色器等)
|--- src          # 核心代码
|    |--- animation  # 动画处理
|    |--- application # 窗口管理
|    |--- camera      # 摄像机
|    |--- model       # 模型管理 (Mesh、Bone)
|    |--- physics     # 物理引擎接口
|    |--- shader      # 着色器管理
|    |--- world       # 世界、物体、玩家
|    |--- utils.h/.cpp # 工具函数
|--- thirdparty    # 第三方库
|--- main.cpp      # 主程序入口
|--- CMakeLists.txt
```

### 编译与运行

#### 构建方式

- **CMake 构建**：使用 Visual Studio 2022，切换至 `x64-Release` 模式
- **编译选项**：`ENABLE_PHYSICS` 开启物理模拟
- **跨平台**：理论支持 Linux，但未测试 ([依赖库本地编译记录](./docs/note.md))

#### 运行方式

- **键盘**：WASD 控制玩家移动，空格输出坐标 (调试)，C 切换相机模式
- **鼠标**：控制相机角度，滚轮缩放视角 

### 示例场景

![example](./figs/demo.png)

### 其他说明

- 角色 (Player) 采用**运动学刚体** (`eKINEMATIC`)，仅受用户输入控制
- 碰撞检测：动态刚体与静态/运动学刚体交互，PhysX 回调机制尚未理解和正确实现，过多刚体实例化会导致稳定运行时崩溃
- 地面采用多个物体拼接，并增加厚度防止刚体穿透问题
- Assimp库期望从3.3.1更新未5.4.3，但尚未完成更新后的兼容更正

### 参考与资源

#### 依赖库

- OpenGL 4.6, GLFW 3.4, Glad, GLM
- Assimp 5.4.3, PhysX 5.5.0, IrrKlang 1.5

#### 参考资料

- [LearnOpenGL](https://learnopengl-cn.github.io/)
- [PhysX 官方文档](https://nvidia-omniverse.github.io/PhysX/physx/5.5.0/index.html)

#### 资源来源

- 模型：Sketchfab, CGTrader, Free3D
- 音效：BGM (魔塔一区BGM), 走路音效 ([站长素材](https://sc.chinaz.com/yinxiao/201201513682.htm))

