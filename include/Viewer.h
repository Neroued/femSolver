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

class Viewer
{
public:
    Viewer(int width, int height, const char *title);
    ~Viewer();

    void initialize();                  // 初始化 OpenGL
    void run();                         // 运行渲染循环
    void displayMesh(const Mesh &mesh); // 显示 Mesh 网格

private:
    GLFWwindow *window;
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;
    int width, height;
    const Mesh *currentMesh;

    void setupMesh(); // 设置 VBO 和 EBO，使用 currentMesh 数据
    void initializeShaders();
    std::string loadShaderSource(const char *filepath);
    unsigned int compileShader(unsigned int type, const std::string &source); // 编译着色器
};

Viewer::Viewer(int width, int height, const char *title) : width(width), height(height), VAO(0), VBO(0), EBO(0), currentMesh(nullptr)
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

void Viewer::setupMesh()
{
    if (!currentMesh)
        return;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, currentMesh->vertex_count() * sizeof(Vec3), currentMesh->vertices.data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentMesh->triangle_count() * 3 * sizeof(uint32_t), currentMesh->indices.data, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // 解绑 VAO
}

void Viewer::displayMesh(const Mesh &mesh)
{
    currentMesh = &mesh;
    setupMesh();
}

void Viewer::run()
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
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.5f)); // 视图矩阵，将相机向后移动
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f); // 透视投影矩阵

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // 绘制三角形面
        glUniform1i(glGetUniformLocation(shaderProgram, "isEdgeMode"), GL_FALSE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, currentMesh->triangle_count() * 3, GL_UNSIGNED_INT, 0);

        // 绘制边
        glUniform1i(glGetUniformLocation(shaderProgram, "isEdgeMode"), GL_TRUE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.0f);  // 设置线宽
        glEnable(GL_LINE_SMOOTH);
        glDrawElements(GL_TRIANGLES, currentMesh->triangle_count() * 3, GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
