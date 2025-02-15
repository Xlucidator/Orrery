

```cpp
# 视口
glViewport(GLint x, GLint y, GLsizei width, GLsizei height); // 压缩而非剪裁

glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha); // set
glClear(GL_COLOR_BUFFER_BIT); // operate
glfwSwapBuffers(GLFWwindows* window); // 双缓冲切换 operate

# 错误处理
GLenum glGetError();
//	GL_INVALID_ENUM   		
//  GL_INVALID_VALUE 		参数错误
// 	GL_INVALID_OPERATION	操作错误
//	GL_OUT_OF_MEMORY		内存耗尽
```



---

VBO：顶点缓冲对象 vertex buffer object

```cpp
void glGenBuffers(GLsizei n, GLuint* arrays); 				// 分配buffer id
void glBindBuffer(GLenum target, GLuint buffer); 	// 绑定状态机插槽
void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
		// 填入数据，真正分配显存， 比较耗时，尽量少次
		// usage: GL_STATIC_DRAW, GL_DYNAMIC_DRAW
		// target: GL_ARRAY_BUFFER 当前vbo插槽

// 多属性数据
// vertices, colors, ...
// 1. single buffer 分开存储
// 2. interleaved buffer 交叉式存储
```

VAO：顶点数组对象 vertex array object 

- 存储Mesh网格所有**顶点属性描述信息** 

```cpp
// size = 每个顶点几个数据
// type = 每个数据的类型
// stride = 顶点数据的步长 = \sum{size * type} （交叉存储时有用，就不是单纯顶点的size*type了）
// offset = 该属性数据在顶点内的offset
// vbo号 = 指定读哪个vbo（不同的vao可以指同一个vbo）

void glGenVertexArrays(GLsizei n, GLuint* arrays);
void glBindVertexArray(GLuint array); // 绑定当前状态机vao插槽
void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei strid, const void* pointer); // 向vao中加入描述信息
void glDeleteVertexArrays(GLsizei n, GLuint* arrays);
```

---

GLSL

- 彼此之间无法通信，仅通过输入和输出承接

```glsl
/* vertex shader: get NDC coordinate and output */

#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
```

```glsl
/* fragment shader: get NDC coordinate and output */

#version 460 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
```

shader编译

```cpp
GLuint glCreateShader(GLenum type);
GLuint glShaderSource(
    GLuint shader, 
    GLsizei count, /* 字符串数组长度 */
    const GLchar* const* string, /* 字符串数组 */
    const GLint* length /* 记录每个字符串的长度 */)
```

---

几何数据：vao和vbo

材质数据：vs和fs

```cpp
void glUseProgram();
void glBindVertexArray();
void glDrawArrays(mode, first, count); // 发出渲染指令 DrawCall
 // mode
 // GL_TRIANGLES
 // GL_TRIANGLE_STRIP:  n is even -> [n-2, n-1, n]; n is odd -> [n-1, n-2, n] 
 // 						保证每个三角形同向，都是逆时针  
 // GL_TRIANGLE_FANS:  以v0为起点
 // GL_LINES, GL_LINE_STRIP
```

---

EBO：索引缓冲对象 Element Buffer Object

- 用STRIP和FAN太死板
- 但TRIANGLES不复用
- 记录顶点索引

```cpp
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);
 // mode: GL_TRIANGLES, GL_LINES
 // count: 绘制索引数组中的几个点
 // type: 索引类型
 // indices: 
 // 	使用了ebo, 则代表索引内偏移， 3*sizeof(float)等，一般就是0
 //     没使用ebo，则直接传入索引数组，不推荐，因为每次都去发
```



任何属性都可插值，只要在vs中自定义输出，到了fs中就会是插值后的结果

---

uv坐标：表达当前像素/顶点对应图片上**百分比位置** 

纹理对象：GPU端以一定格式存放纹理图片描述信息和数据信息的对象，图片 $\to$ GPU 中存着

采样：GPU端，根据**uv坐标**以**一定算法**从纹理内容中获取颜色的**过程**；执行采样的对象为**采样器** 

**纹理单元**：用于链接采样器和纹理对象，让Sampler知道去哪个纹理对象采样

```cpp
void glGenTextures(GLsizei n, GLuint* textures); // 创建纹理对象，类似vao，只是一个编号id
void glActiveTexture(GLenum textureUnit); // 激活纹理单元
void glBindTexture(Glenum target, Glenum texture);
	// target: 一般是GL_TEXTURE_2D，当前纹理状态插槽
	//	还有GL_TEXTURE_1D,GL_TEXTURE_2D_ARRAY,GL_TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_3D等
    // texture: 纹理对象id，函数会将 该纹理对象 与 激活的纹理单元绑定（如没有激活默认0号）
void glTexImage2D(
    GLenum target,
    GLint level,  // 向mipmap哪个层级传输数据
    GLint internalformat,  // 希望纹理对象中的图片像素格式: rgb, rgba, ..
    GLsizei width, GLsizei height,
    GLint border, 	// 历史遗留
    GLenum format, 	// 原始图片格式
    GLenum type,	// 单通道数据格式
    const void* pixels  // 具体数据
); // 开辟显存，向GPU传纹理内容，对应于当前纹理插槽中绑定的纹理对象
```



---

坐标变换

万向节死锁 √

四元数 √  https://www.bilibili.com/video/BV14t421h7M4/



---

PhysX

https://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/Index.html

https://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Index.html

https://nvidia-omniverse.github.io/PhysX/physx/5.5.0/index.html



```
PxBase
 └── PxActor
      └── PxRigidActor
           ├── PxRigidStatic
           └── PxRigidBody
                ├── PxRigidDynamic
                └── PxArticulationLink
```





```cpp
class PxSimulationEventCallback {
public:
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) = 0;
    virtual void onWake(PxActor** actors, PxU32 count) = 0;
    virtual void onSleep(PxActor** actors, PxU32 count) = 0;
    virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) = 0;
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) = 0;
    virtual void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) = 0;
    virtual ~PxSimulationEventCallback() {}
}
```

