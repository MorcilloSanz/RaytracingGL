#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// texture size
const unsigned int TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;

// timing 
float deltaTime = 0.0f, lastFrame = 0.0f;

GLuint createOutputTexture(int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

	// GLTF Scene
	Model model;
	TinyGLTF loader;
	std::string err, warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, "/home/morcillosanz/Documents/GitHub/glTF-Sample-Models/2.0/WaterBottle/glTF/WaterBottle.gltf");

	Mesh mesh = model.meshes[0]; // Do this for each mesh

	for (const auto &primitive : mesh.primitives) {

		// Access indices
		if (primitive.indices > -1) {
			const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
			const tinygltf::BufferView &bufferView = model.bufferViews[indexAccessor.bufferView];
			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			
			const void *dataPtr = &buffer.data[bufferView.byteOffset + indexAccessor.byteOffset];
			const unsigned short *indices = static_cast<const unsigned short*>(dataPtr);

			for (size_t i = 0; i < indexAccessor.count; ++i) {
				std::cout << "Index[" << i << "]: " << indices[i] << std::endl;
			}
		}

		// Access vertices
		for (const auto &attrib : primitive.attributes) {

			const tinygltf::Accessor &accessor = model.accessors[attrib.second];
			const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			
			const void *dataPtr = &buffer.data[bufferView.byteOffset + accessor.byteOffset];

			if (attrib.first == "POSITION") {
				const float *positions = static_cast<const float*>(dataPtr);
				std::cout << "Positions:" << std::endl;
				for (size_t i = 0; i < accessor.count; ++i) {
					std::cout << "Vertex[" << i << "]: ("
								<< positions[3 * i + 0] << ", "
								<< positions[3 * i + 1] << ", "
								<< positions[3 * i + 2] << ")" << std::endl;
				}
			} else if (attrib.first == "NORMAL") {
				const float *normals = static_cast<const float*>(dataPtr);
				std::cout << "Normals:" << std::endl;
				for (size_t i = 0; i < accessor.count; ++i) {
					std::cout << "Normal[" << i << "]: ("
								<< normals[3 * i + 0] << ", "
								<< normals[3 * i + 1] << ", "
								<< normals[3 * i + 2] << ")" << std::endl;
				}
			} else if (attrib.first == "TEXCOORD_0") {
				const float *uvs = static_cast<const float*>(dataPtr);
				std::cout << "UVs:" << std::endl;
				for (size_t i = 0; i < accessor.count; ++i) {
					std::cout << "UV[" << i << "]: ("
								<< uvs[2 * i + 0] << ", "
								<< uvs[2 * i + 1] << ")" << std::endl;
				}
			} else if (attrib.first == "TANGENT") {
				const float *tangents = static_cast<const float*>(dataPtr);
				std::cout << "Tangents:" << std::endl;
				for (size_t i = 0; i < accessor.count; ++i) {
					std::cout << "Tangent[" << i << "]: ("
								<< tangents[4 * i + 0] << ", "
								<< tangents[4 * i + 1] << ", "
								<< tangents[4 * i + 2] << ", "
								<< tangents[4 * i + 3] << ")" << std::endl;
				}
			}
		}

		// Access textures
		if (primitive.material > -1) {
			const tinygltf::Material &material = model.materials[primitive.material];

			if (material.values.find("baseColorTexture") != material.values.end()) {
				const tinygltf::Texture &texture = model.textures[material.values.at("baseColorTexture").TextureIndex()];
				const tinygltf::Image &image = model.images[texture.source];

				std::cout << "Texture: " << image.uri << std::endl;
			}
		}
	}

	// Scene
	std::vector<Vertex> meshVertices = {
		Vertex(glm::vec3( 0.0, -0.5, -2.5),  glm::vec3(1.0f, 0.0f, 0.0f)),
		Vertex(glm::vec3( 0.5,  0.5, -2.5),  glm::vec3(0.0f, 1.0f, 0.0f)),
		Vertex(glm::vec3(-0.5,  0.5, -2.5),  glm::vec3(0.0f, 0.0f, 1.0f))
	};

	std::vector<unsigned int> meshIndices = {
		0, 1, 2
	};

	ShaderStorageBuffer<Vertex>::Ptr ssboVertices = ShaderStorageBuffer<Vertex>::New(meshVertices, 0);
	ShaderStorageBuffer<unsigned int>::Ptr ssboIndices = ShaderStorageBuffer<unsigned int>::New(meshIndices, 1);

	computeShaderProgram->useProgram();
	computeShaderProgram->uniformInt("numIndices", meshIndices.size());

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