# 设置CMake的最低版本
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和版本
project(MyGLFWApp VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

# 包含GLFW库的子目录
add_subdirectory(extern/glfw)

# 添加GLAD库
add_library(glad extern/glad/src/glad.c)
target_include_directories(glad PUBLIC extern/glad/include)

set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/extern/glm")
include_directories(${GLM_INCLUDE_DIR})

# 定义可执行文件
add_executable(test_glfw src/test_glfw.cpp)
add_executable(test_TArray src/test_TArray.cpp)
add_executable(test_cube src/test_cube.cpp)
add_executable(test_systemSolve src/test_systemSolve.cpp)
add_executable(test_viewer src/test_viewer.cpp)



# 链接GLFW、GLAD和OpenGL库
target_link_libraries(test_glfw PRIVATE glfw glad GL)
target_link_libraries(test_viewer PRIVATE glfw glad GL)

# 指定头文件搜索路径
target_include_directories(test_glfw PRIVATE extern/glfw/include extern/glad/include)
target_include_directories(test_viewer PRIVATE extern/glfw/include extern/glad/include)

target_compile_options(test_cube PRIVATE -O3) # 启用 O3 优化
target_compile_options(test_cube PUBLIC -Wall -Wextra -march=native)

