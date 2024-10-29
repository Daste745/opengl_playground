#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else // __APPLE__
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#endif

#include <fstream>
#include <cstddef>
#include <cmath>
#include <chrono>
#include <thread>

#include <GLFW/glfw3.h>

#include "logs.h"
#include "vertex.h"
#include "program.h"

const size_t WIDTH = 800;
const size_t HEIGHT = 800;
const char* WINDOW_TITLE = "Test OpenGL";
const unsigned int TARGET_FRAMERATE = 60;

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
        error("Could not initialize GLFW3");
        return -1;
    }

    auto window = initWindow();
    if (!window) {
        glfwTerminate();
        return -1;
    }

#if defined(__glew_h__)
    glewExperimental = GL_TRUE;
    auto glew_init_result = glewInit();
    if (glew_init_result != GLEW_OK) {
        error("Could not initialize GLEW: " << glewGetErrorString(glew_init_result));
        glfwTerminate();
        return -1;
    }
#endif

    info("Renderer: " << glGetString(GL_RENDERER));
    info("OpenGL version: " << glGetString(GL_VERSION));

    // Register program and shaders
    auto program = Program();
    auto vertexSource = readFile("shaders/vertex.glsl");
    if (!program.registerShader(vertexSource.c_str(), Program::ShaderType::Vertex)) {
        glfwTerminate();
        return -1;
    }

    auto fragmentSource = readFile("shaders/fragment.glsl");
    if (!program.registerShader(fragmentSource.c_str(), Program::ShaderType::Fragment)) {
        glfwTerminate();
        return -1;
    }

    if (!program.registerProgram()) {
        glfwTerminate();
        return -1;
    }

    // Triangle vertices
    vertex vertices[] = {
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

    unsigned int frame = 0;
    while (!glfwWindowShouldClose(window)) {
        auto start = std::chrono::high_resolution_clock::now();

        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program.getId());
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        glfwPollEvents();

        glfwSwapBuffers(window);

        for (int i = 0; i <= 2; i++) {
            // Rotate the triangle
            unsigned int rotationOffset = (360 / 3) * i;
            float degrees = (frame + rotationOffset) % 360;
            auto radians = degrees * M_PI / 180;
            float distanceFromCenter = 0.5;
            vertices[i][0] = distanceFromCenter * sin(radians);
            vertices[i][1] = distanceFromCenter * cos(radians);

            // Color shift, kinda working
            float cyclePercent = (-cos(radians) + 1) / 2;
            for (int j = 0; j <= 2; j++) {
                float colorAmount = ((i + j + 1) % 3);
                vertices[i][3 + j] = (1.0f / 3) * colorAmount * 1.0f * cyclePercent;
            }

            // debug('[' << i << "] (" << vertices[i][3] << ", " << vertices[i][4] << ", " << vertices[i][5] << ")");
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, drawBufferSize, vertices, GL_STATIC_DRAW | GL_MAP_READ_BIT);

        using std::chrono::duration_cast;
        using std::chrono::microseconds;
        using std::chrono::milliseconds;

        auto frameTime = std::chrono::high_resolution_clock::now() - start;
        auto targetFrameTime = milliseconds(1000) / TARGET_FRAMERATE;
        if (frameTime < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        } else {
            warning(
                "Frame took longer than "
                << duration_cast<milliseconds>(targetFrameTime).count() << "ms: "
                << duration_cast<microseconds>(frameTime).count() / 1000.0f << "ms"
            );
        }

        frame++;
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
