#include "buffer.h"

namespace rgl
{

///////////////////
//  IndexBuffer  //
///////////////////

IndexBuffer::IndexBuffer(const std::vector<unsigned int>& _indices)
    : Buffer(), indices(_indices) {
    initBuffer();
}

IndexBuffer::~IndexBuffer() {
    unbind();
    glDeleteBuffers(1, &id);
}

IndexBuffer::IndexBuffer(const IndexBuffer& indexBuffer) 
    : indices(indexBuffer.indices) {
    id = indexBuffer.id;
}

IndexBuffer::IndexBuffer(IndexBuffer&& indexBuffer) noexcept
    : indices(std::move(indexBuffer.indices)) {
    id = indexBuffer.id;
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& indexBuffer) {
    id = indexBuffer.id;
    indices = indexBuffer.indices;
    return *this;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& indexBuffer) noexcept {
    id = indexBuffer.id;
    indices = std::move(indexBuffer.indices);
    return *this;
}

void IndexBuffer::initBuffer() {
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void IndexBuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void IndexBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

////////////////////
//  VertexBuffer  //
////////////////////

VertexBuffer::VertexBuffer(const std::vector<Vertex>& _vertices)
    : Buffer(), vertices(_vertices) {
    initBuffer();
}

VertexBuffer::VertexBuffer(const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& indices)
    : Buffer(), vertices(_vertices) {
    initBuffer();
    indexBuffer = IndexBuffer::New(indices);
}

VertexBuffer::~VertexBuffer() {
    unbind();
    glDeleteBuffers(1, &id);
}

VertexBuffer::VertexBuffer(const VertexBuffer& vertexBuffer)
    : vertices(vertexBuffer.vertices), indexBuffer(vertexBuffer.indexBuffer) {
    id = vertexBuffer.id;
}

VertexBuffer::VertexBuffer(VertexBuffer&& vertexBuffer) noexcept 
    : vertices(std::move(vertexBuffer.vertices)), indexBuffer(vertexBuffer.indexBuffer) {
    id = vertexBuffer.id;
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vertexBuffer) {
    id = vertexBuffer.id;
    vertices = vertexBuffer.vertices;
    indexBuffer = vertexBuffer.indexBuffer;
    return *this;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vertexBuffer) noexcept {
    id = vertexBuffer.id;
    vertices = std::move(vertexBuffer.vertices);
    indexBuffer = vertexBuffer.indexBuffer;
    return *this;
}

void VertexBuffer::vertexAttributes() {
    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, MAX_VERTEX_ATTRIBUTES * sizeof(float), (void*)0);

    // color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, MAX_VERTEX_ATTRIBUTES * sizeof(float), (void*)(3 * sizeof(float)));

    // normal attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, MAX_VERTEX_ATTRIBUTES * sizeof(float), (void*)(7 * sizeof(float)));

    // texture coordinates attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, MAX_VERTEX_ATTRIBUTES * sizeof(float), (void*)(10 * sizeof(float)));

    // tangent attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, MAX_VERTEX_ATTRIBUTES * sizeof(float), (void*)(12 * sizeof(float)));
    
    // bitangent attribute
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, MAX_VERTEX_ATTRIBUTES * sizeof(float), (void*)(15 * sizeof(float)));
}

float* VertexBuffer::parseVertices() {

    unsigned int index = 0;
    float* glVertices = new float[vertices.size() * MAX_VERTEX_ATTRIBUTES];

    for(auto& vertex : vertices) {
        glVertices[index] = vertex.pos.x;       glVertices[index + 1] = vertex.pos.y;   glVertices[index + 2] = vertex.pos.z;
        glVertices[index + 3] = vertex.color.r; glVertices[index + 4] = vertex.color.g; glVertices[index + 5] = vertex.color.b; glVertices[index + 6] = vertex.color.a;
        glVertices[index + 7] = vertex.normal.x;glVertices[index + 8] = vertex.normal.y;glVertices[index + 9] = vertex.normal.z;
        glVertices[index + 10] = vertex.uvs.x;  glVertices[index + 11] = vertex.uvs.y; 
        glVertices[index + 12] = vertex.tan.x;  glVertices[index + 13] = vertex.tan.y;  glVertices[index + 14] = vertex.tan.z;
        glVertices[index + 15] = vertex.bitan.x;glVertices[index + 16] = vertex.bitan.y;glVertices[index + 17] = vertex.bitan.z;
        index += MAX_VERTEX_ATTRIBUTES;
    }

    return glVertices;
}

void VertexBuffer::initBuffer() {

    float* glVertices = parseVertices();

    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * MAX_VERTEX_ATTRIBUTES, glVertices, GL_STATIC_DRAW);

    delete[] glVertices;

    vertexAttributes();
}

void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

///////////////////
//  VertexArray  //
///////////////////

VertexArray::VertexArray() : Buffer() {
    initBuffer();
}

VertexArray::~VertexArray() {
    unbind();
    glDeleteVertexArrays(1, &id);
}

VertexArray::VertexArray(const VertexArray& vertexArray) {
    id = vertexArray.id;
}

VertexArray::VertexArray(VertexArray&& vertexArray) noexcept {
    id = vertexArray.id;
}

VertexArray& VertexArray::operator=(const VertexArray& vertexArray) {
    id = vertexArray.id;
    return *this;
}

VertexArray& VertexArray::operator=(VertexArray&& vertexArray) noexcept {
    id = vertexArray.id;
    return *this;
}

void VertexArray::initBuffer() {
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);
}

void VertexArray::bind() {
    glBindVertexArray(id);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}

///////////////////////////
//  ShaderStorageBuffer  //
//////////////////////////

template <typename T>
ShaderStorageBuffer<T>::ShaderStorageBuffer(const std::vector<T>& _data, unsigned int _bindingPoint) 
    : data(_data), bindingPoint(_bindingPoint) {
    initBuffer();
}

template <typename T>
ShaderStorageBuffer<T>::~ShaderStorageBuffer() {
    glDeleteBuffers(1, &id);
}

template <typename T>
ShaderStorageBuffer<T>::ShaderStorageBuffer(const ShaderStorageBuffer& shaderStorageBuffer) 
    : data(shaderStorageBuffer.data), bindingPoint(shaderStorageBuffer.bindingPoint) {
    id = shaderStorageBuffer.id;
}

template <typename T>
ShaderStorageBuffer<T>::ShaderStorageBuffer(ShaderStorageBuffer&& shaderStorageBuffer) noexcept 
    : data(std::move(shaderStorageBuffer.data)), bindingPoint(shaderStorageBuffer.bindingPoint) {
    id = shaderStorageBuffer.id;
}

template <typename T>
ShaderStorageBuffer<T>& ShaderStorageBuffer<T>::operator=(const ShaderStorageBuffer& shaderStorageBuffer) {
    id = shaderStorageBuffer.id;
    data = shaderStorageBuffer.data;
    bindingPoint = shaderStorageBuffer.bindingPoint;
    return *this;
}

template <typename T>
ShaderStorageBuffer<T>& ShaderStorageBuffer<T>::operator=(ShaderStorageBuffer&& shaderStorageBuffer) noexcept {
    id = shaderStorageBuffer.id;
    data = std::move(shaderStorageBuffer.data);
    bindingPoint = shaderStorageBuffer.bindingPoint;
    return *this;
}

template <typename T>
void ShaderStorageBuffer<T>::initBuffer() {
    glGenBuffers(1, &id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template <typename T>
void ShaderStorageBuffer<T>::bind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
};

template <typename T>
void ShaderStorageBuffer<T>::unbind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template class ShaderStorageBuffer<Vertex>;
template class ShaderStorageBuffer<unsigned int>;

}