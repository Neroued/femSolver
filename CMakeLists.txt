# 设置CMake的最低版本
cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

# 定义项目名称和版本
project(MyGLFWApp VERSION 1.0)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 检查 OpenMP 支持
find_package(OpenMP REQUIRED)

include_directories(include/linalg include/Matrix include/Mesh include/utils extern/json)

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
add_executable(test_fem src/test_fem.cpp)
add_executable(test_femdata src/test_femdata.cpp)
add_executable(test_CSR src/test_CSR.cpp)
add_executable(test_NS src/test_NS.cpp)



# 链接GLFW、GLAD和OpenGL库
target_link_libraries(test_glfw PRIVATE glfw glad GL)
target_link_libraries(test_viewer PRIVATE glfw glad GL)
target_link_libraries(test_femdata PRIVATE glfw glad GL)
target_link_libraries(test_NS PRIVATE glfw glad GL)

# OpenMP
target_link_libraries(test_CSR PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(test_NS PUBLIC OpenMP::OpenMP_CXX)

# 指定头文件搜索路径
target_include_directories(test_glfw PRIVATE extern/glfw/include extern/glad/include)
target_include_directories(test_viewer PRIVATE extern/glfw/include extern/glad/include)
target_include_directories(test_femdata PRIVATE extern/glfw/include extern/glad/include)
target_include_directories(test_NS PRIVATE extern/glfw/include extern/glad/include)

target_compile_options(test_cube PRIVATE -O3) # 启用 O3 优化
target_compile_options(test_cube PUBLIC -Wall -Wextra -march=native)
target_compile_options(test_fem PRIVATE -O3 -ffast-math) # 启用 O3 优化
target_compile_options(test_femdata PRIVATE -O3 -ffast-math)
target_compile_options(test_CSR PRIVATE -O3 -ffast-math -fopenmp)
target_compile_options(test_NS PRIVATE -O3 -ffast-math -fopenmp)

