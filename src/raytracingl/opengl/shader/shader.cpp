#include "shader.h"

#include <fstream>

namespace rgl {

Shader::Shader(const std::string& _code, const ShaderType& _shaderType) 
    : code(_code), shaderType(_shaderType) {
    compileShader();
}

Shader::Shader() : code(""), filePath(""), shaderID(0), shaderType(ShaderType::None) {}

Shader::Shader(const Shader& shader)
    : code(shader.code), filePath(shader.filePath), shaderID(shader.shaderID),
    shaderType(shader.shaderType) {
}

Shader::Shader(Shader&& shader) noexcept
    : code(std::move(shader.code)), filePath(std::move(shader.filePath)),
    shaderID(shader.shaderID), shaderType(std::move(shader.shaderType)) {
}

Shader& Shader::operator=(const Shader& shader) {
    code = shader.code;
    filePath = shader.filePath;
    shaderID = shader.shaderID;
    shaderType = shader.shaderType;
    return *this;
}

Shader& Shader::operator=(Shader&& shader) noexcept{
    code = std::move(shader.code);
    filePath = std::move(shader.filePath);
    shaderID = std::move(shader.shaderID);
    shaderType = std::move(shader.shaderType);
    return *this;
}

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    std::string line = "", code = "";
    if(file.is_open()) {
        while ( getline(file, line) ) code += line + "\n";
        file.close();
    }
    return code;
}

void Shader::compileShader() {

    std::string debugShader = "";
    switch (shaderType) {
    case ShaderType::Vertex:
        shaderID = glCreateShader(GL_VERTEX_SHADER);
        debugShader = "Vertex";
        break;
    case ShaderType::Fragment:
        shaderID = glCreateShader(GL_FRAGMENT_SHADER);
        debugShader = "Fragment";
        break;
    case ShaderType::Compute:
        shaderID = glCreateShader(GL_COMPUTE_SHADER);
        debugShader = "Compute";
        break;
    default:
        debugShader = "Not implemented yet";
        break;
    }

    const char* chrCode = code.c_str();
    glShaderSource(shaderID, 1, &chrCode, NULL);
    glCompileShader(shaderID);

    int success;
    char infoLog[512];

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cout << debugShader << " shader compilation error: " << infoLog << std::endl;
    }
}



ShaderProgram::ShaderProgram(const Shader& _vertexShader, const Shader& _fragmentShader) 
    : vertexShader(_vertexShader), fragmentShader(_fragmentShader), shaderProgramID(0) {
    link();
}

ShaderProgram::ShaderProgram(const Shader& _computeShader) 
    : computeShader(_computeShader), shaderProgramID(0) {
    link(true);
}

ShaderProgram::ShaderProgram() : shaderProgramID(0) {}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(shaderProgramID);
}

ShaderProgram::ShaderProgram(const ShaderProgram& shaderProgram) 
    : vertexShader(shaderProgram.vertexShader), fragmentShader(shaderProgram.fragmentShader),
    shaderProgramID(shaderProgram.shaderProgramID) {
}

ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram) noexcept 
: vertexShader(std::move(shaderProgram.vertexShader)), fragmentShader(std::move(shaderProgram.fragmentShader)),
    shaderProgramID(shaderProgram.shaderProgramID) {
}

ShaderProgram& ShaderProgram::operator=(const ShaderProgram& shaderProgram) {
    vertexShader = shaderProgram.vertexShader;
    fragmentShader = shaderProgram.fragmentShader;
    shaderProgramID = shaderProgram.shaderProgramID;
    return *this;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram) noexcept {
    vertexShader = std::move(shaderProgram.vertexShader);
    fragmentShader = std::move(shaderProgram.fragmentShader);
    shaderProgramID = std::move(shaderProgram.shaderProgramID);
    return *this;
}

void ShaderProgram::link(bool hasComputeShader) {

    shaderProgramID = glCreateProgram();
    
    if(hasComputeShader) 
        glAttachShader(shaderProgramID, computeShader.getShaderID());
    else {
        glAttachShader(shaderProgramID, vertexShader.getShaderID());
        glAttachShader(shaderProgramID, fragmentShader.getShaderID());
    }
    
    glLinkProgram(shaderProgramID);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
        std::cout << "Couldn't link shaders\n" << infoLog << std::endl;
    }

    vertexShader.deleteShader();
    computeShader.deleteShader();
    fragmentShader.deleteShader();
}

void ShaderProgram::uniformInt(const std::string& uniform, int value) {
    int location = glGetUniformLocation(shaderProgramID, uniform.c_str());
    glUniform1i(location, value); 
}

void ShaderProgram::uniformFloat(const std::string& uniform, float value) {
    int location = glGetUniformLocation(shaderProgramID, uniform.c_str());
    glUniform1f(location, value); 
}

void ShaderProgram::uniformVec3(const std::string& uniform, const glm::vec3& vec) {
    int location = glGetUniformLocation(shaderProgramID, uniform.c_str());
    glUniform3fv(location, 1, &vec[0]); 
}

void ShaderProgram::uniformMat4(const std::string& uniform, const glm::mat4& mat) {
    int location = glGetUniformLocation(shaderProgramID, uniform.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

}