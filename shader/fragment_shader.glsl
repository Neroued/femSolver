#version 330 core
in vec3 vertexColor; // 从顶点着色器传入的颜色
out vec4 FragColor;

uniform bool isEdgeMode;  // true: 绘制边，false: 绘制面
uniform bool isFEMData;   // true: 绘制FEMData, false: 绘制Mesh

void main() {
    if (isEdgeMode) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // 边：黑色
    } else if (isFEMData) {
        FragColor = vec4(vertexColor, 1.0);   // 面：使用顶点颜色
    } else {
        FragColor = vec4(1.0, 0.5 , 1.0, 1.0);  // 面
    }
}
