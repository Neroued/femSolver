#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <FEMdata.h>
#include <vector>
#include <algorithm>
#include <random>

class Viewer
{
public:
    Viewer(int width, int height, const char *title);
    ~Viewer();

    void initialize();                         // 初始化 OpenGL
    void runMesh();                            // 运行渲染循环
    void setupMesh(const Mesh &mesh);          // 显示 Mesh 网格
    void setupFEMData(const FEMData &femdata); // 显示FEMData
    void runFEMData();

private:
    GLFWwindow *window;
    unsigned int VAO, VBO, EBO, CBO; // VAO: 顶点数组对象; VBO: 顶点缓冲对象; CBO: 颜色缓冲对象; EBO: 索引缓冲对象
    unsigned int shaderProgram;
    int width, height;
    const Mesh *currentMesh;
    const FEMData *currentfemdata;

    void initializeShaders();
    std::string loadShaderSource(const char *filepath);
    unsigned int compileShader(unsigned int type, const std::string &source); // 编译着色器
    std::vector<Vec3> generateColors();                                       // 生成颜色数据
    std::vector<Vec3> randomGenerateColors();
};

Viewer::Viewer(int width, int height, const char *title) : width(width), height(height), VAO(0), VBO(0), EBO(0), currentMesh(nullptr), currentfemdata(nullptr)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glEnable(GL_DEPTH_TEST);

    // 初始化着色器
    initializeShaders();
}

Viewer::~Viewer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Viewer::initialize()
{
    // 设置清空颜色为白色背景
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Viewer::initializeShaders()
{
    // 加载并编译顶点着色器
    std::string vertexCode = loadShaderSource("./shader/vertex_shader.glsl");
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);

    // 加载并编译片段着色器
    std::string fragmentCode = loadShaderSource("./shader/fragment_shader.glsl");
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    // 链接着色器程序
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram); // 使用链接好的着色器程序
}

std::string Viewer::loadShaderSource(const char *filepath)
{
    std::ifstream file(filepath);
    if (!file)
    {
        throw std::runtime_error("Failed to open shader file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Viewer::compileShader(unsigned int type, const std::string &source)
{
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    return shader;
}

void Viewer::setupMesh(const Mesh &mesh)
{
    currentMesh = &mesh;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, currentMesh->vertex_count() * sizeof(Vec3), currentMesh->vertices.data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentMesh->triangle_count() * 3 * sizeof(uint32_t), currentMesh->indices.data, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(Vec3), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // 解绑 VAO
}

void Viewer::runMesh()
{
    if (!currentMesh)
        return;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 获取当前时间，单位为秒
        float timeValue = glfwGetTime();
        float rotateSpeed = 0.5f;

        // 设置模型矩阵，随时间旋转
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, 30.0f, glm::vec3(0.0f, -0.5f, 1.0f)); // 绕 y 轴旋转
        // model = glm::rotate(model, glm::radians(32.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // 例如绕 x 轴旋转
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.5f));                    // 视图矩阵，将相机向后移动
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f); // 透视投影矩阵

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        int isFEMDataLoc = glGetUniformLocation(shaderProgram, "isFEMData");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1i(isFEMDataLoc, GL_FALSE);

        // 绘制三角形面
        glUniform1i(glGetUniformLocation(shaderProgram, "isEdgeMode"), GL_FALSE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, currentMesh->triangle_count() * 3, GL_UNSIGNED_INT, 0);

        // 绘制边
        glUniform1i(glGetUniformLocation(shaderProgram, "isEdgeMode"), GL_TRUE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.0f); // 设置线宽
        glEnable(GL_LINE_SMOOTH);
        glDrawElements(GL_TRIANGLES, currentMesh->triangle_count() * 3, GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Viewer::setupFEMData(const FEMData &femdata)
{
    currentfemdata = &femdata;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 设置顶点位置
    const Mesh &meshnow = currentfemdata->mesh;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, meshnow.vertex_count() * sizeof(Vec3), meshnow.vertices.data, GL_STATIC_DRAW);

    /* glVertexAttribPointer： 配置 VAO 中的位置数据的解析方式。
    参数：
    0： 顶点属性索引（Location 0，对应顶点着色器中的位置属性）。
    3： 每个顶点包含 3 个分量（x, y, z）。
    GL_DOUBLE： 数据类型为 double。
    GL_FALSE： 不需要归一化。
    sizeof(Vec3)： 每个顶点的步长（即每个顶点数据的总大小）。
    (void*)0： 顶点数据的起始偏移量，从缓冲区的起始位置开始。
    glEnableVertexAttribArray(0)： 启用顶点属性 Location 0。*/
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(Vec3), (void *)0);
    glEnableVertexAttribArray(0);

    // 生成颜色数据
    std::vector<Vec3> colors = generateColors();
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Vec3), colors.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, sizeof(Vec3), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshnow.triangle_count() * 3 * sizeof(uint32_t),
                 meshnow.indices.data, GL_STATIC_DRAW);
}

std::vector<Vec3> Viewer::randomGenerateColors()
{
    // 初始化随机数生成器
    std::random_device rd;                                 // 随机种子
    std::mt19937 gen(rd());                                // 随机数生成器
    std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 随机数范围 [0, 1]

    // 获取顶点数量
    const Vec &u = currentfemdata->u;
    size_t vertex_count = u.size;

    // 为每个顶点生成随机颜色
    std::vector<Vec3> colors(vertex_count);
    for (size_t i = 0; i < vertex_count; ++i)
    {
        colors[i] = Vec3(dis(gen), dis(gen), dis(gen)); // 随机生成 R, G, B 分量
    }

    return colors;
}

std::vector<Vec3> Viewer::generateColors()
{
    const Vec &u = currentfemdata->u;
    double u_min = *std::min_element(u.begin(), u.end());
    double u_max = *std::max_element(u.begin(), u.end());
    if (u_max == u_min)
    {
        return std::vector<Vec3>(u.size, Vec3{0.0f, 0.0f, 1.0f}); // 全部设为蓝色
    }

    double mult = 1 / (u_max - u_min);

    std::vector<Vec3> colors(u.size);
    for (size_t i = 0; i < u.size; ++i)
    {
        // 归一化到[0,1]
        float normed = (u[i] - u_min) * mult;

        // 热力图映射（蓝绿红）
        if (normed < 0.5)
        {
            colors[i] = {0.0f, normed * 2.0f, 1.0f - normed * 2.0f};
        }
        else
        {
            colors[i] = {(normed - 0.5f) * 2.0f,
                         1.0f - (normed - 0.5f) * 2.0f,
                         0.0f};
        }
    }

    return colors;
}

void Viewer::runFEMData()
{
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 获取当前时间，单位为秒
        float timeValue = glfwGetTime();
        float rotateSpeed = 0.5f;

        // 设置模型矩阵，随时间旋转
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, timeValue * rotateSpeed, glm::vec3(0.0f, -0.5f, 1.0f));                 // 绕 y 轴旋转
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.5f));                    // 视图矩阵
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f); // 投影矩阵

        // 获取 Uniform 变量位置
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        int isEdgeModeLoc = glGetUniformLocation(shaderProgram, "isEdgeMode");
        int isFEMDataLoc = glGetUniformLocation(shaderProgram, "isFEMData");

        // 向着色器传递矩阵
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1i(isEdgeModeLoc, GL_FALSE);
        glUniform1i(isFEMDataLoc, GL_TRUE);

        // **绘制三角形面**
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 填充模式
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, currentfemdata->mesh.triangle_count() * 3, GL_UNSIGNED_INT, 0);

        // 解绑 VAO
        glBindVertexArray(0);

        // 交换缓冲区并处理事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
