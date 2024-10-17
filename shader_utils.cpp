#include <optional>

#include <GLFW/glfw3.h>

#include "logs.h"
#include "shader_utils.h"

namespace ShaderUtils {
    Program::Program() {}

    Program::~Program() {
        if (vertexShader.has_value())
            glDeleteShader(vertexShader.value());
        if (fragmentShader.has_value())
            glDeleteShader(fragmentShader.value());
        if (registered && program.has_value())
            glDeleteProgram(program.value());
    }

    bool Program::registerShader(const ShaderType shader_type, const char *shader_source) {
        bool isFragmentShader = shader_type == ShaderType::Fragment;

        auto real_shader_type = !isFragmentShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

        auto shader = glCreateShader(real_shader_type);
        // Now, pass the shaders
        glShaderSource(shader, 1, &shader_source, NULL);
        // And now, compile them
        glCompileShader(shader);

        int success = {};
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char errorMessage[1024] = {};
            glGetShaderInfoLog(shader, 1024, NULL, errorMessage);
            if (isFragmentShader) {
                error("Fragment shader compilation error: " << errorMessage);
            }
            else {
                error("Vertex shader compilation error: " << errorMessage);
            }

            return false;
        }

        if (isFragmentShader) {
            fragmentShader = shader;
        }
        else {
            vertexShader = shader;
        }

        return true;
    }

    bool Program::registerProgram() {
        if (registered) {
            error("Program is already registered");
            return false;
        }

        if (!vertexShader.has_value() || !fragmentShader.has_value()) {
            error("Cannot compile program without vertex and fragment shaders");
            return false;
        }

        program = glCreateProgram();
        const unsigned int programValue = program.value();

        glAttachShader(programValue, vertexShader.value());
        glAttachShader(programValue, fragmentShader.value());
        glLinkProgram(programValue);

        int success = {};
        glGetProgramiv(programValue, GL_LINK_STATUS, &success);
        if (!success) {
            char errorMessage[1024] = {};
            glGetProgramInfoLog(programValue, 1024, NULL, errorMessage);
            error("Shader linking error: " << errorMessage);
            return false;
        }

        // We can now delete our vertex and fragment shaders
        glDeleteShader(vertexShader.value());
        glDeleteShader(fragmentShader.value());
        glUseProgram(programValue);
        registered = true;

        return true;
    }

    std::optional<unsigned int> Program::getProgram() const {
        return program;
    }

    bool Program::programIsRegistered() const {
        return registered;
    }
}
