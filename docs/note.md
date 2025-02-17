## Jottings

### MSVC /MT & /MD 编译选项异常

检查 /MT 编译模式：仅 `PhysXExtensions_static_64.lib` 有 `/DEFAULTLIB:OLDNAMES`和`/DEFAULTLIB:LIBCMT` 的标志，其他所有库都没有，难怪之前CMake未能成功设置MT编译选项时，报错也仅发生在该lib库中

```powershell
dumpbin /directives PhysXExtensions_static_64.lib | findstr "DEFAULTLIB"
```



### 第三方库跨平台构建

glfw3.4 : 构建为静态连接库

```bash
### MinGW
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=./install
mkdir install
make # mingw32-make
make install

### Ubuntu22.04 (WSL2)
cmake .. -DCMAKE_BUILD_TYPE=Release -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=./install
mkdir install
make
make install

wsl2中已经有WSLGd了（ps aux | grep WSLGd 查看）
- 需要事先安装wayland-scanner, wayland-client, wayland-cursor, wayland-egl, xkbcommon
sudo apt install libwayland-bin libwayland-dev libxkbcommon-dev libegl-dev
# wayland-scanner不在wayland-protocols中而在libwayland-bin中, 而wayland-scanner++则是C++版
- make install时出现的.cmake和.pc分别是用于CMake和Makefile寻找依赖时用的，情境是将库装到电脑本地时用的

### MSVC

```



assimp 原来时3.3.1，有点想换成最新的5.4.3 ： 还是构建为动态链接库比较好

```bash
# MinGW
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_FLAGS="-Wno-error -Wno-unknown-pragmas" -DCMAKE_INSTALL_PREFIX=./install
mkdir install
make -j4
make install

- 加入-Wno-error是因为在contrib/poly2tri/poly2tri/common/dll_symbol.h和shapes.h中都是用了"#pragma warning"这个MSVC特有的宏，所以干脆不管这个了；不止这些地方，还有很多；咦加了-Wno-error还是继续warning当error报错 -> 可能之前使用"-Werror=unknown-pragmas"，此时-Wno-error只能全局禁用将警告视为错误，无法覆盖特定警告被标记为错误的情况
- 这个编译有点慢

cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_CXX_FLAGS="-Wno-error -Wno-unknown-pragmas" -DCMAKE_INSTALL_PREFIX=./install
mkdir install
make -j4
make install


# Ubuntu (WSL2)  反正基本和MinGW一样
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=./install
mkdir install
make -j4
make install

cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=./install
mkdir install
make -j4
make install

# MSVC
cmake .. -G "Visual Studio 17 2022" -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=./install
mkdir install
cmake --build . --config Release
cmake --install . --config Release
或者用cmake图形界面生成内容，进入vs2022进行编译
会出assimp-vc143-mt.lib和zlibstatic.lib

cmake .. -G "Visual Studio 17 2022" -DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded" -DCMAKE_INSTALL_PREFIX=./install
mkdir install
cmake --build . --config Release
cmake --install . --config Release
```



PhysX 5.5.0

```bash
# Ubuntu
generate_projects.sh
cd ./compiler/linux-release
make -j4
make install

# MSVC
generate_projects.bat
没用命令行，即vs2022图形界面操作了; ALL_BUILD生成，然后INSTALL生成

东西在./install中
```



似乎以后应该把构建库的命令整合成脚本，然后第三方库仅使用submodule链接，这样最好



```
# 编译器特性检测
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++17 HAS_CXX17)
if(NOT HAS_CXX17)
    message(FATAL_ERROR "The compiler does not support C++17 standard")
endif()

# 平台特定依赖查找
if(IS_LINUX)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
endif()

# GLFW配置
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(Orrery PRIVATE glfw)

# ASSIMP配置
find_package(assimp REQUIRED)
target_link_libraries(Orrery PRIVATE assimp::assimp)

# PhysX配置
if(ENABLE_PHYSICS)
    if(IS_WINDOWS)
        find_library(PHYSX_LIB PhysX_64 PATHS "${CMAKE_SOURCE_DIR}/thirdparty/lib/${CMAKE_SYSTEM_NAME}")
    elseif(IS_LINUX)
        find_library(PHYSX_LIB PhysX PATHS "${CMAKE_SOURCE_DIR}/thirdparty/lib/${CMAKE_SYSTEM_NAME}")
    endif()
    target_link_libraries(Orrery PRIVATE ${PHYSX_LIB})
endif()

# IrrKlang配置（Windows专用）
if(IS_WINDOWS)
    if(MSVC)
        set(IRRKLANG_LIB "${CMAKE_SOURCE_DIR}/thirdparty/lib/Windows/MSVC/irrKlang.lib")
    else()
        set(IRRKLANG_LIB "${CMAKE_SOURCE_DIR}/thirdparty/lib/Windows/MinGW/libirrklang.a")
    endif()
    target_link_libraries(Orrery PRIVATE ${IRRKLANG_LIB})
endif()

```



