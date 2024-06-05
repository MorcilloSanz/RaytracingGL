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

    int width = 640, height = 480;
    window = glfwCreateWindow(width, height, "Basic", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    // Init GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Couldn't initialize GLEW" << std::endl;
        return 0;
    }

    glfwSetKeyCallback(window, key_callback);

    // Screen Quad
    Shader vertexShader = Shader::fromFile("glsl/vertex.glsl", Shader::ShaderType::Vertex);
    Shader fragmentShader = Shader::fromFile("glsl/fragment.glsl", Shader::ShaderType::Fragment);
    ShaderProgram::Ptr shaderProgram = ShaderProgram::New(vertexShader, fragmentShader);

    std::vector<Vertex> quadVertices = {
        Vertex(glm::vec3( 1.f,  1.f, 0.0f), glm::vec4(1.0), glm::vec2(1.0f, 1.0f)), // top right
        Vertex(glm::vec3( 1.f, -1.f, 0.0f), glm::vec4(1.0), glm::vec2(1.0f, 0.0f)), // bottom right
        Vertex(glm::vec3(-1.f, -1.f, 0.0f), glm::vec4(1.0), glm::vec2(0.0f, 0.0f)), // bottom left
        Vertex(glm::vec3(-1.f,  1.f, 0.0f), glm::vec4(1.0), glm::vec2(0.0f, 1.0f))  // top left 
    };

    std::vector<unsigned int> quadIndices = { 0, 1, 3, 1, 2, 3 };

    VertexArray::Ptr vertexArray = VertexArray::New();
    VertexBuffer::Ptr vertexBuffer = VertexBuffer::New(quadVertices, quadIndices);

    // Scene
    int max_compute_work_group_count[3];
	int max_compute_work_group_size[3];
	int max_compute_work_group_invocations;

	for (int idx = 0; idx < 3; idx++) {
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
	}	
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

	std::cout << "OpenGL Limitations: " << std::endl;
	std::cout << "Maximum number of work groups in X dimension " << max_compute_work_group_count[0] << std::endl;
	std::cout << "Maximum number of work groups in Y dimension " << max_compute_work_group_count[1] << std::endl;
	std::cout << "Maximum number of work groups in Z dimension " << max_compute_work_group_count[2] << std::endl;

	std::cout << "Maximum size of a work group in X dimension " << max_compute_work_group_size[0] << std::endl;
	std::cout << "Maximum size of a work group in Y dimension " << max_compute_work_group_size[1] << std::endl;
	std::cout << "Maximum size of a work group in Z dimension " << max_compute_work_group_size[2] << std::endl;

	std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << std::endl;

    GLuint outputTexture = createOutputTexture(width, height);

    Shader computeShader = Shader::fromFile("glsl/compute.glsl", Shader::ShaderType::Compute);
    ShaderProgram::Ptr computeShaderProgram = ShaderProgram::New(computeShader);

    std::vector<Vertex> meshVertices = {
        Vertex(glm::vec3(-0.5, -0.5, -0.5),  glm::vec4(0.0f, 0.0f, 1.0f, 1.f)),
        Vertex(glm::vec3( 0.5, -0.5, -0.5),  glm::vec4(1.0f, 0.0f, 1.0f, 1.f)),
        Vertex(glm::vec3(-0.5,  0.5, -0.5),  glm::vec4(0.0f, 1.0f, 1.0f, 1.f)),
        Vertex(glm::vec3( 0.5,  0.5, -0.5),  glm::vec4(0.0f, 1.0f, 0.5f, 1.f)),
        Vertex(glm::vec3(-0.5, -0.5,  0.5),  glm::vec4(0.0f, 0.0f, 1.0f, 1.f)),
        Vertex(glm::vec3( 0.5, -0.5,  0.5),  glm::vec4(1.0f, 0.0f, 1.0f, 1.f)),
        Vertex(glm::vec3(-0.5,  0.5,  0.5),  glm::vec4(0.0f, 1.0f, 1.0f, 1.f)),
        Vertex(glm::vec3( 0.5,  0.5,  0.5),  glm::vec4(0.0f, 1.0f, 0.5f, 1.f))
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
        glDispatchCompute((GLuint)ceil(width / 16.0), (GLuint)ceil(height / 16.0), 1);
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
        glDrawElements(GL_TRIANGLES, quadIndices.size(), GL_UNSIGNED_INT, 0);
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    return texture;
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}
 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}