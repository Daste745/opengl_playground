#version 410 core

layout(location = 0) in vec3 fragmentColor;

layout(location = 0) out vec4 finalColor;

void main() {
    finalColor = vec4(fragmentColor, 1.0);
}
