# Orrery

Orrery是一个基于现代OpenGL可编程管线的3D图形交互实验项目，旨在提供一个可视化的环境用于自由探索各种图形和物理效果，作为笔者光栅渲染方式下的实验Playground



目前实现功能如下：

- 窗口显示与交互回调函数设置(窗口调整等)：GLFW库
- 相机设置与移动，当前设置为FOLLOW模式，可通过按键切换模式
- 常用模型导入与显示，带骨骼和动画的模型导入：ASSIMP库
  - 导入静态模型可使用obj，动态模型可用dae, fbx等
  - 支持动画播放的顶点着色器
  - 支持普通或Blinn-Phong光照模型的片段着色器（需与所选导入模型配合，当前未使用光照）
- 背景音乐和动作音效播放：IrrKlang库
- 物理效果模拟与碰撞检测：PhysX库
  - 使用静态刚体和动态刚体，玩家为运动学刚体

![demo](./figs/demo.png)



### 架构介绍

#### 目录结构

```
|
|--- assets : 项目资源
|		|
|		|--- audios : 音频
|		|--- objects: 模型
|		`--- shaders: 着色器代码
|
|---   src  : 项目主要代码
|		|
|		|--- animation  : Animator类用于动画播放，Animation类用于动画数据结构存储
|		|--- application: Application单例类，是应用窗口抽象
|		|--- camera     : Camera类，摄像机
|		|--- light      : Light类，暂无
|		|--- model      : Model类导入存储全模型，Mesh类存储每个网格数据结构，Bone类存储骨骼数据结构
|		|--- physics    : CollosionCB类，自定义PhysX库场景碰撞回调函数类
|		|--- shader     : Shader类，用于着色器编译构建与使用
|		|--- world      : World类代表世界，Object类为所有世界中物体的抽象，Player为特殊的Object
|		|
|		|- assimp2glm.h : ASSIMP和GLM库数学结构转换
|		|- common.h     : 基础头文件，引用常用的第三方库和标准库，通用全局定义，为所有类文件所引用
|		`- utils.h/.cpp : 工具函数
|
|--- thirdparty : 跨平台构建并引用的第三方库
|		|
|		`---...
|
|- main.cpp             : 程序主逻辑
|- main_derecated.cpp   : 旧时测试用逻辑，基本已经没用
`- CMakeLists.txt       : 项目通过CMake在Visual Studio 2022中管理
```

#### 项目架构

基础设计参考LearnOpenGL，在此基础上进一步结构化，以方便自定义导入并放置指定模型，同时也遵循OOP编程原则；项目主干类结构关系如图所示

​                                               ![classed](./figs/classes.png)  

程序主逻辑实现在main.cpp中，可从此处开始梳理项目代码结构关系，代码中也有基本的注释，如果看不懂了再来问吧。项目运行时的基本流程如图所示

![struct](./figs/structure.png)

### How to Use

Method 1: Using Visual Studio CMake (Recommend)

- Open Visual Studio with no code, Select "Open" → "CMake" on the menu, letting VS browsing through the project through CMake
- Switch to Release Mode
- Save CMakeLists.txt to update CMakeCache, which would copy *.dll and assets in the the exe directory, every time before you 



导入自定义模型：在World.cpp文件的`World::initObjects()`函数中，该函数首先编译构建指定着色器，随后导入指定路径的模型，随后依据配置使用指定着色器、模型、位置构建Object，最后可选择性生成Object在PhysX中的物理场景中的形态(刚体)以参与物理模拟

- 分离Model和Object构建的原因在于允许复用模型，即不同Object可指向同个Model，不同Object也可指向同个着色器
- Object的构造函数有两个，可直接传入模型矩阵，也可传入位置和朝向(默认world_up为y轴正方向，**注意欧拉角使用限制，朝向初始不能沿着y轴**)；TODO 添加第三个Object构造函数，直接使用旋转四元数
- 物理刚体创建：根据自己的需求调用`createRigidDynamic`或`createRigidStatic`；Player和Ground的物理初始化相对特殊，不建议更改

程序运行时基本操作方式

- 键盘输入：WASD控制玩家，FOLLOW模式的相机移动；按下空格会在stdout中输出当前角色所在坐标，可用于调试或测量
- 鼠标移动：普通情况控制相机欧拉角，当前FOLLOW模式相机不响应
- 鼠标滚轮：相机视角缩放



### 注意事项

1. 模型主要来源于Sketchfab，该网站模型质量和可用性似乎较高；可自行重新选择，从Blender中导出obj时需注意
   - 确保将**纹理解包出来**，然后在着色器视图中重新添加一次新解包出来的纹理，否则导入时可能丢失纹理（tips:导出后可以直接查看obj文件，如果纹理没有丢失就说明ok）
   - 同时注意导出时的纹理路径，选择“相对路径”，且之后复制到项目assets中时保持同样的相对路径，否则导入将丢失贴图（tips: 可以查看对应的mtl文件中的路径，如果不对可直接修改其中路径为正确的相对路径，前提是确保上一点正确）
   - [new] **本项目的实现和着色器支持纹理贴图的混合**，但是由于Blender导出模型的格式没有一个能正确加载混合纹理的（会丢失）；因此项目仅在显示“签名”的四边形处，手动添加第二张纹理，进行混合；并且由于其他模型均没有`sample2D material.texture_diffuse2`，若使用同个着色器的话这第二章纹理会传导到其他物体的渲染上，所以是现实时为signature重新生成一个Shader对象
2. 带动画的模型可以采用dae格式，但是项目导入Blender中新导出dae文件，渲染的顶点位置会很怪（看起来像是四元数顺序不匹配导致），而当前版本的ASSIMP不支持Blender新导出的glb格式，经测试项目fbx格式的动画导入正常，因而主要使用fbx格式
   - Player对应的模型的骨骼和走路动画是我自己加的，略显粗糙；似乎暂时没能找到带有合理骨骼和动画的角色模型
   - 作为地面的模型也是我自己用Blender建的，即为平面+纹理贴图，比较简陋；似乎也难以找到何时的场景建模
3. [new] 拥有动画的模型，似乎其初始的顶点模型矩阵会特别大且意外的不同，一定要配合上骨骼模型矩阵后才能正常
4. **[new]** 为了减小模型大小，提高资源利用率：地面由9个Object拼成，均指向同一个导入的模型，只不过在位置上做了变化
5. PhysX中的地面不能使用Plane，因为没有厚度动态刚体(RigidDynamic)会直接穿过它，于是在物理模拟时给它增加了厚度
6. Player对应的动态刚体设置了“运动学刚体”`setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true)`，仅受用户输入控制，每帧模拟前使用`rigid_dynamic->setKinematicTarget(transform_to_test)`传入预期位置，随后在`mScene->simulate(...)`时进行判定；角色的物理模型采用了简单的包围盒进行模拟
   - 疑似根据Model建立的`px_triangle_mesh`没法很好的工作，所以Player在PhysX中的物理形状静态配置成了简单的AABB包围盒，`_aabb_hy`为其半高
7. PhysX中运动学刚体和静态刚体RigidStatic之间的碰撞检测尚不成功：尚不知怎样正确的触发回调函数；**运动学刚体和动态刚体之间的碰撞可以生效**，见运行程序行为中角色和桶的碰撞
8. [new] 在world.start中更新`_last_frame`时间，**这很重要**，这样使得第一帧时算出的`_delta_time`不至于过大，而是和之后差不多；从而保证物理模拟与初始化时的结果不至于相差过大
9. **[new]** 增加了随机位置生成，然后增加了各模型生成Object的数量；**增加了PhysX动态刚体和静态刚体生成数量，但这也导致程序运行时的不稳定**，刚体之间的作用有时能成功，有时会莫名遭遇运行时的错误，且发生错误的位置不唯一；
   - 运行时错误发生时可能的位置所在文件（这竟然还不在项目的库文件中，不知道在哪儿）：GuCollisionSDF.h，GuContactMeshMesh.h，PxcNpBatch.cpp ...
10. 世界场地设置了边界`border`，为**±20.0f**的正方形，角色不会超出此范围
11. 导入的走路音效在某些时间段存在失真，不过第一步的声音是正常的，或许可以尝试剪切为仅踏出第一步的时间段，然后循环播放



### 参考引用

第三方库

- OpenGL 4.6
- GLFW 3.4
- Glad
- GLM
- Assimp 3.3.1 -> 5.4.3
- PhysX 5.5.0
- IrrKlang 1.5

参考资料

- LearnOpenGL网站 https://learnopengl-cn.github.io/  项目 https://github.com/JoeyDeVries/LearnOpenGL 
- PhysX官方文档 https://nvidia-omniverse.github.io/PhysX/physx/5.5.0/index.html
  - 初始化示例介绍 https://www.youtube.com/watch?v=zOYpVAoQFyU
  - 项目 https://github.com/kmiloarguello/openGL-physX，本地可编译运行，不过使用的是PhysX 4.1, 与5.5的实现存在区别

辅助工具

- Blender 4.3.2

资源来源

- Sketchfab, CGTrader, Free3D(似乎比较劣质且数量少)
- BGM为魔塔一区BGM，走路音效来自 https://sc.chinaz.com/yinxiao/201201513682.htm



