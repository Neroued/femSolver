# 设置 CMake 的最低版本
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和版本
project(femSolver VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置编译类型和编译选项
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# 检查 OpenMP 支持
find_package(OpenMP REQUIRED)

# 添加子目录
add_subdirectory(lib)               # 子项目 lib
add_subdirectory(extern/glfw)       # 子项目 glfw

# 添加 GLAD 库
add_library(glad extern/glad/src/glad.c)
target_include_directories(glad PUBLIC extern/glad/include)

# 设置 GLM 的头文件路径
set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/extern/glm")
include_directories(${GLM_INCLUDE_DIR})

# 定义公共链接的库
set(COMMON_LIBRARIES glfw glad GL Lib OpenMP::OpenMP_CXX)

# 定义公共头文件路径
set(COMMON_INCLUDE_DIRS 
    lib/include
    extern/glfw/include
    extern/glad/include
    ${GLM_INCLUDE_DIR}
)

# 函数：为目标添加公共配置
function(configure_executable target)
    target_link_libraries(${target} PRIVATE ${COMMON_LIBRARIES})
    target_include_directories(${target} PRIVATE ${COMMON_INCLUDE_DIRS})
endfunction()

# 定义可执行文件并配置公共属性
set(TEST_TARGETS 
    test_viewer
    test_fem
    test_femdata
    test_CSR
    test_NS
    test
)

foreach(target ${TEST_TARGETS})
    add_executable(${target} src/${target}.cpp)
    configure_executable(${target})
endforeach()

# 设置编译优化选项
target_compile_options(test_fem PRIVATE -O3 -ffast-math)
target_compile_options(test_femdata PRIVATE -O3 -ffast-math)
target_compile_options(test_CSR PRIVATE -O3 -ffast-math -fopenmp)
target_compile_options(test_NS PRIVATE -O3 -ffast-math -fopenmp)
target_compile_options(test PRIVATE -O3 -ffast-math -fopenmp)

