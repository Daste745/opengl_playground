#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "program.h"
#include "logs.h"

Program::Program() {}

Program::~Program() {
    if (fragmentShader.has_value())
        glDeleteShader(fragmentShader.value());
    if (vertexShader.has_value())
        glDeleteShader(vertexShader.value());
    if (id.has_value())
        glDeleteProgram(id.value());
}

bool Program::registerShader(const char *source, ShaderType type) {
    bool isFragmentShader = type == ShaderType::Fragment;
    auto realShaderType = isFragmentShader ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;
    auto shader = glCreateShader(realShaderType);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success = {};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char errorMessage[1024] = {};
        glGetShaderInfoLog(shader, 1024, NULL, errorMessage);
        error("Shader compilation error: " << errorMessage)
        return false;
    }

    if (isFragmentShader) fragmentShader = shader;
    else vertexShader = shader;

    return true;
}

bool Program::registerProgram() {
    if (id.has_value()) {
        error("Program is already registered");
        return false;
    }

    if (!vertexShader.has_value() || !fragmentShader.has_value()) {
        error("Cannot register program without vertex and fragment shaders");
        return false;
    }

    id = glCreateProgram();
    glAttachShader(id.value(), vertexShader.value());
    glAttachShader(id.value(), fragmentShader.value());
    glLinkProgram(id.value());

    int success = {};
    glGetProgramiv(id.value(), GL_LINK_STATUS, &success);
    if (!success) {
        char errorMessage[1024] = {};
        glGetProgramInfoLog(id.value(), 1024, nullptr, errorMessage);
        error("Failed to link program: " << errorMessage);
        return false;
    }

    glUseProgram(id.value());

    return true;
}
