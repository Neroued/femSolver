# 设置CMake的最低版本
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和版本
project(MyGLFWApp VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/viewer)
include_directories(${PROJECT_SOURCE_DIR}/include/mesh)

# 包含GLFW库的子目录
add_subdirectory(extern/glfw)

# 添加GLAD库
add_library(glad extern/glad/src/glad.c)
target_include_directories(glad PUBLIC extern/glad/include)

set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/extern/glm")
include_directories(${GLM_INCLUDE_DIR})

# 定义可执行文件
add_executable(MyGLFWApp src/test_glfw.cpp)
add_executable(test_TArray src/test_TArray.cpp)
add_executable(test_cube src/test_cube.cpp)



# 链接GLFW、GLAD和OpenGL库
target_link_libraries(MyGLFWApp PRIVATE glfw glad GL)

# 指定头文件搜索路径
target_include_directories(MyGLFWApp PRIVATE extern/glfw/include extern/glad/include)

target_compile_options(test_cube PRIVATE -O3) # 启用 O3 优化
target_compile_options(test_cube PUBLIC -Wall -Wextra -ffast-math -march=native)
