#pragma once

#include <iostream>
#include <vector>

#include <GL/glew.h>

#include "raytracingl/ptr.h"
#include "raytracingl/geometry/vertex.h"

#define MAX_VERTEX_ATTRIBUTES 18

namespace rgl
{

class Buffer {
    GENERATE_SHARED_PTR(Buffer)
protected:
    unsigned int id;
    virtual void initBuffer() = 0;
public:
    Buffer() : id(0) {}
    virtual ~Buffer() = default;
public:
    virtual void bind() = 0;
    virtual void unbind() = 0;
public:
    unsigned int getID() const { return id; }
};


class IndexBuffer : public Buffer {
    GENERATE_SHARED_PTR(IndexBuffer)
private:
    std::vector<unsigned int> indices;
public:
    IndexBuffer(const std::vector<unsigned int>& _indices);
    IndexBuffer() = default;
    ~IndexBuffer();
    IndexBuffer(const IndexBuffer& indexBuffer);
    IndexBuffer(IndexBuffer&& indexBuffer) noexcept;
    IndexBuffer& operator=(const IndexBuffer& indexBuffer);
    IndexBuffer& operator=(IndexBuffer&& indexBuffer) noexcept;
public:
    void initBuffer() override;
    void bind() override;
    void unbind() override;
public:
    std::vector<unsigned int> getIndices() const { return indices; }
};


class VertexBuffer : public Buffer {
    GENERATE_SHARED_PTR(VertexBuffer)
private:
    std::vector<Vertex> vertices;
    IndexBuffer::Ptr indexBuffer;
public:
    VertexBuffer(const std::vector<Vertex>& _vertices);
    VertexBuffer(const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& indices);
    VertexBuffer() = default;
    ~VertexBuffer();
    VertexBuffer(const VertexBuffer& vertexBuffer);
    VertexBuffer(VertexBuffer&& vertexBuffer) noexcept;
    VertexBuffer& operator=(const VertexBuffer& vertexBuffer);
    VertexBuffer& operator=(VertexBuffer&& vertexBuffer) noexcept;
private:
    void vertexAttributes();
    float* parseVertices();
public:
    void initBuffer() override;
    void bind() override;
    void unbind() override;
public:
    std::vector<Vertex> getVertices() const { return vertices; }
    IndexBuffer::Ptr getIndexBuffer() const { return indexBuffer; }
};


class VertexArray : public Buffer {
    GENERATE_SHARED_PTR(VertexArray)
public:
    VertexArray();
    ~VertexArray();
    VertexArray(const VertexArray& vertexArray);
    VertexArray(VertexArray&& vertexArray) noexcept;
    VertexArray& operator=(const VertexArray& vertexArray);
    VertexArray& operator=(VertexArray&& vertexArray) noexcept;
public:
    void initBuffer() override;
    void bind() override;
    void unbind() override;
};

}