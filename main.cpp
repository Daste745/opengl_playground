#include <fstream>
#ifdef __APPLE__
// Defined before OpenGL and GLUT includes to avoid deprecation messages
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#endif

#include "logs.h"
#include "shader_utils.h"
#include "vertex.h"
#include <cstddef>

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const char* WINDOW_TITLE = "Test OpenGL";

static void keyCallback(GLFWwindow *window, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLFWwindow* initWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    auto window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        error("Could not open window with GLFW3");
        glfwTerminate();
        return nullptr;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, WIDTH, HEIGHT);

    return window;
}

std::string readFile(const char* path) {
    auto stream = std::ifstream(path);

    constexpr size_t read_size = 4096;
    auto buf = std::string(read_size, '\0');
    auto out = std::string();
    while (stream.read(&buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());

    return out;
}

int main(int argc, char **argv) {
    if (!glfwInit()) {
        error("Could not start GLFW3");
        return -1;
    }

    auto window = initWindow();
    if (!window) {
        glfwTerminate();
        return -1;
    }

    info("Renderer: " << glGetString(GL_RENDERER));
    info("OpenGL version: " << glGetString(GL_VERSION));

    // Init shaders
    auto shader_utils = ShaderUtils::Program();

    auto vertexSource = readFile("shaders/vertex.glsl");
    if (!shader_utils.registerShader(ShaderUtils::ShaderType::Vertex, vertexSource.c_str())) {
        glfwTerminate();
        return -1;
    }

    auto fragmentSource = readFile("shaders/fragment.glsl");
    if (!shader_utils.registerShader(ShaderUtils::ShaderType::Fragment, fragmentSource.c_str())) {
        glfwTerminate();
        return -1;
    }

    if (!shader_utils.registerProgram()) {
        glfwTerminate();
        return -1;
    }

    // Triangle vertices
    vertex vertices[3] = {
        { 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f}, // Top
        { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f}, // Right
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f}, // Bottom left
    };
    const unsigned int vertexCount = sizeof(vertices) / sizeof(*vertices);
    const unsigned int drawBufferSize = vertexCount * sizeof(vertex);


    // Vertex Buffer Object = VBO
    GLuint VBO = {};
    glGenBuffers(1, &VBO);
    if (glGetError() != GL_NO_ERROR) {
        error("Couldn't generate buffers");
        glfwTerminate();
        return -1;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, drawBufferSize, vertices, GL_STATIC_DRAW | GL_MAP_READ_BIT);

    // Vertex Arrays Object = VAO
    GLuint VAO = {};
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Specify position attribute -> 0 as offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Specify color attribute -> 3 as offset
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int tick = 0;
    float perTick = 0.01f;
    float rate = 30.f;
    vertices[0][0] -= perTick * rate;
    vertices[1][0] -= perTick * rate;
    vertices[2][0] -= perTick * rate;
    vertices[0][1] -= perTick * rate;
    vertices[1][1] -= perTick * rate;
    vertices[2][1] -= perTick * rate;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_utils.getProgram().value());
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwPollEvents();

        glfwSwapBuffers(window);

        float offset = perTick * sin(tick / rate);
        // x
        vertices[0][0] += offset;
        vertices[1][0] += offset;
        vertices[2][0] += offset;
        // y
        vertices[0][1] += offset;
        vertices[1][1] += offset;
        vertices[2][1] += offset;

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, drawBufferSize, vertices, GL_STATIC_DRAW | GL_MAP_READ_BIT);

        tick++;
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
