#version 330 core
out vec4 FragColor;

uniform bool isEdgeMode;  // true: 绘制边，false: 绘制面

void main() {
    if (isEdgeMode) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);  // 边
    } else {
        FragColor = vec4(1.0, 0.5 , 1.0, 1.0);  // 面
    }
}
