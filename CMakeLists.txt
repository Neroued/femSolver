# 设置CMake的最低版本
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和版本
project(MyGLFWApp VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

# 包含GLFW库的子目录
add_subdirectory(extern/glfw)

# 添加GLAD库
add_library(glad extern/glad/src/glad.c)
target_include_directories(glad PUBLIC extern/glad/include)

# 定义可执行文件
add_executable(MyGLFWApp src/test_glfw.cpp)
add_executable(test_Tarray src/test_Tarray.cpp)

# 链接GLFW、GLAD和OpenGL库
target_link_libraries(MyGLFWApp PRIVATE glfw glad GL)

# 指定头文件搜索路径
target_include_directories(MyGLFWApp PRIVATE extern/glfw/include extern/glad/include)
