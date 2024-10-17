#ifdef __APPLE__
// Defined before OpenGL and GLUT includes to avoid deprecation messages
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#endif

#include "logs.h"
#include "shader_utils.h"
#include "math_utils.h"
#include <cstddef>

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const char* WINDOW_TITLE = "Test OpenGL";

static void keyCallback(GLFWwindow *window, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(int argc, char **argv) {
    if (!glfwInit()) {
        error("Could not start GLFW3");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    auto window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        error("Could not open window with GLFW3");
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, WIDTH, HEIGHT);

    info("Renderer: " << glGetString(GL_RENDERER));
    info("OpenGL version: " << glGetString(GL_VERSION));

    // Init shaders
    auto shader_utils = ShaderUtils::Program();

    const char *vertexSource = R"GLSL(
        #version 410 core
        layout (location = 0) in vec3 vertexPosition;
        layout (location = 1) in vec3 vertexColor;
        layout (location = 0) out vec3 fragmentColor;

        void main() {
            gl_Position = vec4(vertexPosition, 1.0); // `w` is used for perspective
            fragmentColor = vertexColor;
        }
    )GLSL";
    if (!shader_utils.registerShader(ShaderUtils::ShaderType::Vertex, vertexSource)) {
        glfwTerminate();
        return -1;
    }

    const char *fragmentSource = R"GLSL(
        #version 410 core
        layout (location = 0) in vec3 fragmentColor;
        out vec4 finalColor;

        void main() {
            finalColor = vec4(fragmentColor, 1.0);
        }
    )GLSL";
    if (!shader_utils.registerShader(ShaderUtils::ShaderType::Fragment, fragmentSource)) {
        glfwTerminate();
        return -1;
    }

    if (!shader_utils.registerProgram()) {
        glfwTerminate();
        return -1;
    }

    // Triangle vertices
    MathUtils::vertex vertices[3] = {
        { 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f}, // Top
        { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f}, // Right
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f}, // Bottom left
    };
    const unsigned int vertexCount = sizeof(vertices) / sizeof(*vertices);
    const unsigned int drawBufferSize = vertexCount * sizeof(MathUtils::vertex);


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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, MathUtils::VERTEX_ELEMENTS_NB * sizeof(float), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Specify color attribute -> 3 as offset
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, MathUtils::VERTEX_ELEMENTS_NB * sizeof(float), (GLvoid *)(3 * sizeof(float)));
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
