#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <raytracingl/opengl/shader/shader.h>
#include <raytracingl/opengl/buffer/buffer.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <raytracingl/vendor/tiny_gltf.h>

using namespace rgl;
using namespace tinygltf;

// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// texture size
const unsigned int TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;

// timing 
float deltaTime = 0.0f, lastFrame = 0.0f;

GLuint createOutputTexture(int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

bool loadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

int main(int argc, char* argv[]) {

	// GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Basic", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(0);

	// GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Couldn't initialize GLEW" << std::endl;
		return 0;
	}

	// Query limitations
	int max_compute_work_group_count[3];
	int max_compute_work_group_size[3];
	int max_compute_work_group_invocations;

	for (int idx = 0; idx < 3; idx++) {
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
	}	
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

	std::cout << "OpenGL Limitations: " << std::endl;
	std::cout << "maximum number of work groups in X dimension " << max_compute_work_group_count[0] << std::endl;
	std::cout << "maximum number of work groups in Y dimension " << max_compute_work_group_count[1] << std::endl;
	std::cout << "maximum number of work groups in Z dimension " << max_compute_work_group_count[2] << std::endl;

	std::cout << "maximum size of a work group in X dimension " << max_compute_work_group_size[0] << std::endl;
	std::cout << "maximum size of a work group in Y dimension " << max_compute_work_group_size[1] << std::endl;
	std::cout << "maximum size of a work group in Z dimension " << max_compute_work_group_size[2] << std::endl;

	std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << std::endl;

	// Shaders
	Shader vertexShader = Shader::fromFile("glsl/vertex.glsl", Shader::ShaderType::Vertex);
	Shader fragmentShader = Shader::fromFile("glsl/fragment.glsl", Shader::ShaderType::Fragment);
	ShaderProgram::Ptr shaderProgram = ShaderProgram::New(vertexShader, fragmentShader);

	Shader computeShader = Shader::fromFile("glsl/compute.glsl", Shader::ShaderType::Compute);
	ShaderProgram::Ptr computeShaderProgram = ShaderProgram::New(computeShader);

	unsigned int texture = createOutputTexture(TEXTURE_WIDTH, TEXTURE_HEIGHT);

	shaderProgram->useProgram();
	shaderProgram->uniformInt("tex", texture - 1);

	// Screen quad
	std::vector<Vertex> quadVertices = {
		Vertex(glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(1.0), glm::vec3(0.0), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0), glm::vec3(0.0), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec3(1.0), glm::vec3(0.0), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec3(1.0), glm::vec3(0.0), glm::vec2(1.0f, 0.0f))
	};

	VertexArray::Ptr vertexArray = VertexArray::New();
	VertexBuffer::Ptr vertexBuffer = VertexBuffer::New(quadVertices);

	// Scene
	std::vector<Vertex> meshVertices = {
		Vertex(glm::vec3(-1.0, -1.0,  1.0), glm::vec3(0.0f, 0.0f, 1.0f)),
		Vertex(glm::vec3( 1.0, -1.0,  1.0), glm::vec3(1.0f, 0.0f, 1.0f)),
		Vertex(glm::vec3( 1.0,  1.0,  1.0), glm::vec3(0.0f, 1.0f, 1.0f)),
		Vertex(glm::vec3(-1.0,  1.0,  1.0), glm::vec3(0.0f, 1.0f, 0.5f)),
		Vertex(glm::vec3(-1.0, -1.0, -1.0), glm::vec3(0.0f, 0.0f, 1.0f)),
		Vertex(glm::vec3( 1.0, -1.0, -1.0), glm::vec3(1.0f, 0.0f, 1.0f)),
		Vertex(glm::vec3( 1.0,  1.0, -1.0), glm::vec3(0.0f, 1.0f, 1.0f)),
		Vertex(glm::vec3(-1.0,  1.0, -1.0), glm::vec3(0.0f, 1.0f, 0.5f))
	};

	std::vector<unsigned int> meshIndices = {
		0, 1, 2,  1, 5, 6,  7, 6, 5,
		2, 3, 0,  6, 2, 1,  5, 4, 7,
		4, 0, 3,  4, 5, 1,  3, 2, 6,
		3, 7, 4,  1, 0, 4,  6, 7, 3 
	};

	glm::mat4 modelMatrix(1.f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f));

	/*
	std::vector<Vertex> meshVertices;
	std::vector<unsigned int> meshIndices;

	loadModel("/home/morcillosanz/Documents/GitHub/glTF-Sample-Models/2.0/Cube/glTF/Cube.gltf", meshVertices, meshIndices);

	for(auto& vertex : meshVertices) {
		vertex.pos = vertex.pos * glm::vec3(0.5) + glm::vec3(0.0, 0.0, -2.0);
		std::cout << vertex << std::endl;
	}
	*/

	std::cout << "Num vertices: " << meshVertices.size() << std::endl;
	std::cout << "Num indices: " << meshIndices.size() << std::endl;

	ShaderStorageBuffer<Vertex>::Ptr ssboVertices = ShaderStorageBuffer<Vertex>::New(meshVertices, 0);
	ShaderStorageBuffer<unsigned int>::Ptr ssboIndices = ShaderStorageBuffer<unsigned int>::New(meshIndices, 1);

	computeShaderProgram->useProgram();
	computeShaderProgram->uniformInt("numVertices", meshVertices.size());
	computeShaderProgram->uniformInt("numIndices", meshIndices.size());
	computeShaderProgram->uniformMat4("modelMatrix", modelMatrix);

	// Main loop
	while (!glfwWindowShouldClose(window)) {

		// Set frame time.
		static int fCounter = 0;
		float currentFrame = glfwGetTime();

		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if(fCounter > 500) {
			std::cout << "FPS: " << 1 / deltaTime << std::endl;
			fCounter = 0;
		} else 
			fCounter++;

		// Compute Shader
		computeShaderProgram->useProgram();
		computeShaderProgram->uniformFloat("t", currentFrame);
		glDispatchCompute((unsigned int)TEXTURE_WIDTH/10, (unsigned int)TEXTURE_HEIGHT/10, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// render image to quad
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram->useProgram();

		vertexArray->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		vertexArray->unbind();

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteTextures(1, &texture);
	glfwTerminate();

	return EXIT_SUCCESS;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

bool loadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    // Cargar el modelo GLTF
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);

    if (!ret) {
        std::cerr << "Error al cargar el archivo GLTF: " << err << std::endl;
        return false;
    }

    // Iterar sobre todas las mallas del modelo
    for (size_t m = 0; m < model.meshes.size(); ++m) {
        const tinygltf::Mesh& mesh = model.meshes[m];

        // Iterar sobre las primitivas de cada malla
        for (size_t p = 0; p < mesh.primitives.size(); ++p) {
            const tinygltf::Primitive& primitive = mesh.primitives[p];

            // Obtener los accesores de los atributos
            const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
            const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
            const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];

            // Obtener los datos de los buffers para cada atributo
            const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
            const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
            const tinygltf::BufferView& colorBufferView = model.bufferViews[colorAccessor.bufferView];
            const tinygltf::BufferView& uvBufferView = model.bufferViews[uvAccessor.bufferView];

            const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];
            const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
            const tinygltf::Buffer& colorBuffer = model.buffers[colorBufferView.buffer];
            const tinygltf::Buffer& uvBuffer = model.buffers[uvBufferView.buffer];

            // Calcular los offsets dentro de los buffer views
            size_t posOffset = posAccessor.byteOffset + posBufferView.byteOffset;
            size_t normalOffset = normalAccessor.byteOffset + normalBufferView.byteOffset;
            size_t colorOffset = colorAccessor.byteOffset + colorBufferView.byteOffset;
            size_t uvOffset = uvAccessor.byteOffset + uvBufferView.byteOffset;

            // Leer los datos de los buffers
            const float* posData = reinterpret_cast<const float*>(&posBuffer.data[posOffset]);
            const float* normalData = reinterpret_cast<const float*>(&normalBuffer.data[normalOffset]);
            const float* colorData = reinterpret_cast<const float*>(&colorBuffer.data[colorOffset]);
            const float* uvData = reinterpret_cast<const float*>(&uvBuffer.data[uvOffset]);

            // Leer los índices
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
            size_t indexOffset = indexAccessor.byteOffset + indexBufferView.byteOffset;

            const uint32_t* indexData = reinterpret_cast<const uint32_t*>(&indexBuffer.data[indexOffset]);
            size_t indexCount = indexAccessor.count;

            // Procesar los datos de vértices e índices
            for (size_t v = 0; v < posAccessor.count; ++v) {
                Vertex vertex;
                vertex.pos = glm::vec3(posData[v * 3], posData[v * 3 + 1], posData[v * 3 + 2]);
                vertex.normal = glm::vec3(normalData[v * 3], normalData[v * 3 + 1], normalData[v * 3 + 2]);
                vertex.color = glm::vec3(colorData[v * 3], colorData[v * 3 + 1], colorData[v * 3 + 2]);
                vertex.uv = glm::vec2(uvData[v * 2], uvData[v * 2 + 1]);
                vertices.push_back(vertex);
            }

            for (size_t i = 0; i < indexCount; ++i) {
                indices.push_back(indexData[i]);
            }
        }
    }

    return true;
}