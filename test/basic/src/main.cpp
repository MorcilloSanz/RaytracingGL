#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include <stdlib.h>
#include <stdio.h>

#include <raytracingl/opengl/shader/shader.h>
#include <raytracingl/opengl/buffer/buffer.h>

using namespace rgl;

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLuint createOutputTexture(int width, int height);

int main(void) {

    // Window 
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Basic", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Couldn't initialize GLEW" << std::endl;
        return 0;
    }

    glfwSetKeyCallback(window, key_callback);

    // Engine
    Shader vertexShader = Shader::fromFile("glsl/vertex.glsl", Shader::ShaderType::Vertex);
    Shader fragmentShader = Shader::fromFile("glsl/fragment.glsl", Shader::ShaderType::Fragment);
    ShaderProgram::Ptr shaderProgram = ShaderProgram::New(vertexShader, fragmentShader);

    int width = 640, height = 480;
    GLuint outputTexture = createOutputTexture(width, height);

    // Screen Quad
    std::vector<Vertex> quadVertices = {
        Vertex(glm::vec3(-1.0f,  1.0f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( 1.0f, -1.0f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(1.0f, 0.0f)),

        Vertex(glm::vec3(-1.0f,  1.0f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3( 1.0f, -1.0f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3( 1.0f,  1.0f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(1.0f, 1.0f))
    };

    VertexArray::Ptr vertexArray = VertexArray::New();
    VertexBuffer::Ptr vertexBuffer = VertexBuffer::New(quadVertices);

    // Scene
    Shader computeShader = Shader::fromFile("glsl/compute.glsl", Shader::ShaderType::Compute);
    ShaderProgram::Ptr computeShaderProgram = ShaderProgram::New(computeShader);

    std::vector<Vertex> meshVertices = {
        Vertex(glm::vec3(-1.0, -1.0, -1.0),  glm::vec4(0.0f, 0.0f, 1.0f, 1.f)),
        Vertex(glm::vec3( 1.0, -1.0, -1.0),  glm::vec4(1.0f, 0.0f, 1.0f, 1.f)),
        Vertex(glm::vec3(-1.0,  1.0, -1.0),  glm::vec4(0.0f, 1.0f, 1.0f, 1.f)),
        Vertex(glm::vec3( 1.0,  1.0, -1.0),  glm::vec4(0.0f, 1.0f, 0.5f, 1.f)),
        Vertex(glm::vec3(-1.0, -1.0,  1.0),  glm::vec4(0.0f, 0.0f, 1.0f, 1.f)),
        Vertex(glm::vec3( 1.0, -1.0,  1.0),  glm::vec4(1.0f, 0.0f, 1.0f, 1.f)),
        Vertex(glm::vec3(-1.0,  1.0,  1.0),  glm::vec4(0.0f, 1.0f, 1.0f, 1.f)),
        Vertex(glm::vec3( 1.0,  1.0,  1.0),  glm::vec4(0.0f, 1.0f, 0.5f, 1.f))
    };

    std::vector<unsigned int> meshIndices = {
        0, 1, 2, 2, 1, 3,
        4, 0, 6, 6, 0, 2,
        7, 5, 6, 6, 5, 4,
        3, 1, 7, 7, 1, 5,
        4, 5, 0, 0, 5, 1,
        6, 7, 2, 2, 7, 3
    };

    ShaderStorageBuffer<Vertex>::Ptr ssboVertices = ShaderStorageBuffer<Vertex>::New(meshVertices, 0);
    ShaderStorageBuffer<unsigned int>::Ptr ssboIndices = ShaderStorageBuffer<unsigned int>::New(meshIndices, 1);
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {

        // Compute program
        computeShaderProgram->useProgram();
        glDispatchCompute((GLuint)width / 16, (GLuint)height / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Draw
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw output texture 
        shaderProgram->useProgram();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, outputTexture);
        shaderProgram->uniformInt("screenTexture", outputTexture - 1);
        vertexArray->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Update frames
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

GLuint createOutputTexture(int width, int height) {

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    return texture;
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}
 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}