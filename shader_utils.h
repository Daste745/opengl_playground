#include <optional>

#pragma once

namespace ShaderUtils {
    enum ShaderType {
        Fragment,
        Vertex,
    };

    struct Program {
        public:
        Program();
        ~Program();
        bool registerShader(const ShaderType shader_type, const char *shader_source);
        bool registerProgram();
        std::optional<unsigned int> getProgram() const;
        bool programIsRegistered() const;

        private:
        std::optional<unsigned int> vertexShader;
        std::optional<unsigned int> fragmentShader;
        std::optional<unsigned int> program;
        bool registered = false;
    };
}
