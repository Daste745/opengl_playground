#pragma once

#include <optional>

struct Program {
    enum ShaderType {
        Fragment,
        Vertex,
    };

    public:
    Program();
    ~Program();
    bool registerShader(const char* source, ShaderType type);
    bool registerProgram();
    [[nodiscard]] unsigned int getId() { return id.value(); }
    [[nodiscard]] bool isRegistered() { return id.has_value(); }

    private:
    std::optional<unsigned int> id;
    std::optional<unsigned int> fragmentShader;
    std::optional<unsigned int> vertexShader;
};
