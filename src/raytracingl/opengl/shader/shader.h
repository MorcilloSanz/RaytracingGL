#pragma once

#include <iostream>
#include <vector>

#include <GL/glew.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "raytracingl/ptr.h"


namespace rgl
{

class Shader {
public:
    enum class ShaderType {
        None, Vertex, Fragment, Compute
    };
private:
    std::string code, filePath;
    ShaderType shaderType;
    unsigned int shaderID;
private:
    static std::string readFile(const std::string& path);
    void compileShader();
public:
    Shader(const std::string& _code, const ShaderType& _shaderType);
    Shader();
    ~Shader() = default;
    Shader(const Shader& shader);
    Shader(Shader&& shader) noexcept;
    Shader& operator=(const Shader& shader);
    Shader& operator=(Shader&& shader) noexcept;
public:
    static Shader fromFile(const std::string& filePath, const ShaderType& shaderType) {
        return Shader(readFile(filePath), shaderType);
    }

    static Shader fromCode(const std::string& code, const ShaderType& shaderType) {
        return Shader(code, shaderType);
    }

    void deleteShader() { glDeleteShader(shaderID); }
public:
    std::string& getCode() { return code; }
    unsigned int getShaderID() const { return shaderID; }
    ShaderType& getShaderType() { return shaderType; }
};


class ShaderProgram {
    GENERATE_SHARED_PTR(ShaderProgram)
private:
    unsigned int shaderProgramID;
    Shader vertexShader, fragmentShader, computeShader;
public:
    ShaderProgram(const Shader& _vertexShader, const Shader& _fragmentShader);
    ShaderProgram(const Shader& _computeShader);
    ShaderProgram();
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram& shaderProgram);
    ShaderProgram(ShaderProgram&& shaderProgram) noexcept;
    ShaderProgram& operator=(const ShaderProgram& shaderProgram);
    ShaderProgram& operator=(ShaderProgram&& shaderProgram) noexcept;
private:
    void link(bool hasComputeShader = false);
public:
    void uniformInt(const std::string& uniform, int value);
    void uniformFloat(const std::string& uniform, float value);
    void uniformVec3(const std::string& uniform, const glm::vec3& vec);
    void uniformMat4(const std::string& uniform, const glm::mat4& mat);
public:
    void useProgram() { glUseProgram(shaderProgramID); }
    unsigned int getShaderProgramID() const { return shaderProgramID; }
    
    Shader& getVertexShader() { return vertexShader; }
    Shader& getFragmentShader() { return fragmentShader; }
};

}