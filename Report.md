Link to the Video:

### Dependencies

third-party libraries

- OpenGL 4.6
- GLFW 3.4：/MT build
- Glad
- GLM
- Assimp 3.3.1
- PhysX 5.5：/MT build
- IrrKlang 1.5



### Programming Patterns

双缓冲模式 Double Buffer Pattern

- 使用了GLFW提供的`glfwSwapBuffer()`进行frame buffer的双缓冲输出

游戏循环模式 Game Loop Pattern

- `main.cpp`中游戏的主循环为

  ```cpp
  APP->init();
  world.init();
  world.start();
  while (APP->update()) {
      world.update();
      world.render();
  }
  APP->destroy();
  ```

- 每一帧一循环，依次处理：

  - 用户输入处理：键盘输入、鼠标移动和滚动
  - 每个物体更新：Player位姿移动，有动画的Object使用Animator播放动画(更新Bone Matrices)
  - 物理模拟与获取结果
  - 每个物体更新&修正：根据物理模拟结果，更新刚体间碰撞后的位姿变更，Player运动学刚体的位置检查
  - 物体渲染：对每个物体应用着色器，传入MVP等信息，OpenGL绘制

更新方法模式 Update Method Pattern

- 一个World中包含所有生成的Objects对象，每一帧都会调用`world.update()`，在其中会调用每个对象实现一个更新方法，以处理对象在一帧内的行为。每一帧中，游戏循环对集合中的每一个对象进行更新



---

单例模式 Singleton Pattern

- Application类为单例类，全局只有一个`Application::mInstance`，是对GLFW窗口配置和回调函数注册的整合；这是因为全局只有一个窗口。

命令模式 Command Pattern

- PhysX的碰撞回调函数类：`MyCollisionCallback`类，继承`physx::PxSimulationEventCallback`类，注册到全局的mScene中，重写了onContact；会在当物体发生接触时回调
- 可以改为命令模式但还未：用户交互Interact的响应（鼠标、键盘）可以抽象为Interactable接口类

享元模式 Flyweight Pattern

- Object类的抽象：内部用**共享指针**指向Model和Shader，这样以共享的方式高效地支持使用大量的细粒度的对象，单个Model资源可生成很多Object；着色器Shader和模型Model可以多种组合生成不同的Object。如此模型加载的时间缩短了，模型存储的容量也缩短了，还可以生成更多的Object

原型模式 Prototype Pattern

- Object类是Player类的原型，Player继承于Object并重写了一些仅属于玩家角色的方法



### Game Mechanics

交互方式

- 内容介绍：键盘WASD移动角色或相机，SPACE打印角色位置信息，C切换相机模式；鼠标移动控制相机视角旋转；鼠标缩放控制相机缩放
- 实现方式：通过注册GLFW的回调函数：先用Application用**已有函数**直接注册为GLFW的回调函数，已有函数会在内部调用函数指针指向的函数；对外提供函数指针的set方法，再于main中正式设置自实现的回调函数；这些函数会调用World中应对交互的函数`processXXX`，而World的`processXXX`函数又会根据需求调用Player和Camera的`processXXX`函数

摄像机模式

- 内容介绍：初始为FOLLOW模式，相机锁定旋转，位置上保持当前与角色的位移，随角色的移动而移动，此时WASD操纵角色，鼠标移动对相机无效；按C会切换到FREE模式，角色位置被锁定，相机解锁，可自由在空间中移动和旋转
- 实现方式：Camera类中定义状态，然后定义follow和free函数，统一通过switchMode函数切换，在World的`processXXX`中进行控制。follow时记录传入Player的共享指针，同时锁定Player；free时解锁Player并清理指针，转变状态

物体随机分布

- 内容介绍：初始化世界时，会根据所选方法生成指定数量的随机位置共之后的Object生成使用；随机位置可限定生成范围(方形或环形)和最小间隔，以免冲突
- 实现方式：重载overload 实现`generateRandomPoints`函数，提供用普通坐标限制和极坐标限制两种生成方式，从而可限制不同生成区域；生成算法即为每次随机生成一个坐标，然后和已有坐标进行距离判断，符合就加入；所以其实生成数量不能设置的过大，以陷入死锁

模型加载和动画播放

- 内容介绍：可加载市面上基本格式的模型和动画
- 实现方式：使用ASSIMP库，导入后转写为自定义的数据结构 Model、Mesh、Vertices；动画数据通过插值可生成不同时间点所有骨骼的模型矩阵，从而进行转换；骨骼矩阵也随MVP矩阵一起传入顶点着色器中

玩家角色走路动画与音效控制，位移限制

- 内容介绍：玩家角色行走时，播放行走动画，同时播放行走音效；角色不能移动出世界border范围(空气墙 ±20.0f)
- 实现方式：Player类中定义状态，在Player的update函数中，根据状态不同分支操作；WALKING状态下播放动画和音效，IDLE状态下重置**动画**和**音效**；每次移动后时检测Player的_position，超过范围则将其limit到极限范围位置

物理模拟刚体间的碰撞

- 内容介绍：实现的世界有物理效果，桶和地面，桶和箱子，角色和桶之间可发生碰撞并按物理规则运动
- 实现方式：引入PhysX库，建立RigidDynamic和RigidStatics，并赋予角色eKinematics运动学刚体属性；物体在update中更新位置，在mScene->simulate(delta_time)中模拟，最后在updateSimulateResult中更新修正位置；完成后再进行渲染操作

随机飞鸟

- 内容介绍：场景中存在随机飞过的飞鸟
- 实现方式：为Object添加ramdom_move选项，在此模式下，每次会在世界border范围外随机生成一点作为from，然后以(0.0f, 0.0f)为中心对称生成to位置，然后根据设定的高度和速度，每帧进行移动；当移动到指定位置(和to位置的差距小于velocity时)，将会重新生成from和to进行下一轮移动



### Software Engineering Issues

模型资源和着色器的复用：见前享元模式的介绍

模型修改与贴图压缩

- 由于文件容量的限制，所以不能无限制的使用obj模型；于是对一些大的纹理贴图进行了压缩(通常大的都是diffuse和normal贴图)，在不影响基本外观的情况下限制assets容量
- 同时对于地面的模型，仅使用了简单的四边形平面加贴图，但是大小不合适，缩放的话纹理也会跟着变大；于是就使用单地面模型，生成了九宫格位置的9个Object作为地面，这样既大幅减少了模型大小，又能生成合理的地面，加载时间也不会增加

顶点骨骼依赖数量

- 每个顶点设置4个印象的骨骼id和权重；实际运行时输出顶点数据发现，其实一般的小模型，每个顶点依赖的骨骼数量很少，4个根本用不满



### UML Diagram

正规的UML图我Visual Studio生成不出来，然后用doxygen生成的协作关系都没显示，也不好看；自己画的话太庞大了。就用README上的那张简略的图作为大致的结构示意图吧。但注意，**那个不是UML图** 



### Sample Screen





### Further Details

README 的注意事项中摘一些吧：**第一条**最好说明一下



### Evaluation

Achieve：掌握了OpenGL基础，也可以加载各种模型，也学会为了需求通过Blender绘制模型/绑骨骼/做动画，加载动画并操纵；

Todo：增加PBR光照，增加多种动作等，更好的类继承关系设计，敌人类；PhysX库更加广泛和正确的运用（目前只用了一点功能，再多就容易运行模拟中报错）