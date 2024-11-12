#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

std::string readShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Tetrahedron with Colored Edges", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置白色背景
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // 顶点数据
    float vertices[] = {
        -0.5f, -0.5f, 0.2f,  1.0f, 0.0f, 0.0f,  // 第一个点，红色
         0.5f, -0.5f, 0.8f,  0.0f, 1.0f, 0.0f,  // 第二个点，绿色
         0.0f,  0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  // 第三个点，蓝色
         0.0f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f  // 第四个点，黄色
    };

    // 索引数组
    unsigned int indices_faces[] = {
        0, 1, 2,  // 面 1
        0, 1, 3,  // 面 2
        0, 2, 3,  // 面 3
        1, 2, 3   // 面 4
    };

    unsigned int indices_edges[] = {
        0, 1,  // 边 1
        0, 2,  // 边 2
        0, 3,  // 边 3
        1, 2,  // 边 4
        1, 3,  // 边 5
        2, 3   // 边 6
    };

    unsigned int VBO, VAO_faces, EBO_faces, VAO_edges, EBO_edges;
    glGenVertexArrays(1, &VAO_faces);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO_faces);

    // 设置面数据
    glBindVertexArray(VAO_faces);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_faces);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_faces), indices_faces, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 设置边数据
    glGenVertexArrays(1, &VAO_edges);
    glGenBuffers(1, &EBO_edges);

    glBindVertexArray(VAO_edges);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_edges);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_edges), indices_edges, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::string vertexCode = readShaderSource("shader/vertex_shader.glsl");
    std::string fragmentCode = readShaderSource("shader/fragment_shader.glsl");

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // 绘制四面体表面
        glUniform1i(glGetUniformLocation(shaderProgram, "isEdge"), GL_FALSE);
        glBindVertexArray(VAO_faces);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        // 绘制四面体边
        glUniform1i(glGetUniformLocation(shaderProgram, "isEdge"), GL_TRUE);
        glBindVertexArray(VAO_edges);
        glDrawElements(GL_LINES, 12, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO_faces);
    glDeleteVertexArrays(1, &VAO_edges);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO_faces);
    glDeleteBuffers(1, &EBO_edges);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
