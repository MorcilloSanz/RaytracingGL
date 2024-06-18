#pragma once

#include <iostream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "raytracingl/ptr.h"

namespace rgl
{

struct alignas(16) Vertex {

    // DO NOT MODIFY THE ORDER. OTHERWISE, THERE WILL BE A MEMORY ALIGNMENT ISSUE
    // AND IT WILL NOT MATCH THE VERTEX STRUCTURE OF THE COMPUTE SHADER
    glm::vec3 pos;
    float pad1;  // padding to align vec3 to vec4

    glm::vec3 color;
    float pad2;  // padding to align vec3 to vec4

    glm::vec3 normal;
    float pad3;  // padding to align vec3 to vec4

    glm::vec2 uv;
    glm::vec2 pad4;  // padding to align vec3 to vec4

    glm::vec3 tan;
    float pad5;  // padding to align vec3 to vec4

    glm::vec3 bitan;
    float pad6;  // padding to align vec3 to vec4


    Vertex(const glm::vec3& _pos, const glm::vec3& _color, const glm::vec3& _normal, const glm::vec2& _uv)
        : pos(_pos), color(_color), normal(_normal), uv(_uv), tan(1.f), bitan(1.f) {
    }

    Vertex(const glm::vec3& _pos, const glm::vec3& _color, const glm::vec3& _normal)
        : pos(_pos), color(_color), normal(_normal), uv(1.0), tan(1.f), bitan(1.f) {
    }

    Vertex(const glm::vec3& _pos, const glm::vec3& _color)
        : pos(_pos), color(_color), normal(1.f), uv(1.f), tan(1.f), bitan(1.f) {
    }

    Vertex(const glm::vec3& _pos)
        : pos(_pos), color(1.f), normal(1.f), uv(1.f), tan(1.f), bitan(1.f) {
    }

    Vertex() = default;
    ~Vertex() = default;

    Vertex(const Vertex& vertex)
        : pos(vertex.pos), color(vertex.color), uv(vertex.uv), normal(vertex.normal),
        tan(vertex.tan), bitan(vertex.bitan) {
    }

    Vertex(Vertex&& vertex) noexcept
        : pos(std::move(vertex.pos)), color(std::move(vertex.color)), uv(std::move(vertex.uv)),
        normal(std::move(vertex.normal)), tan(std::move(vertex.tan)), bitan(std::move(vertex.bitan)) {
    }

    Vertex& operator=(const Vertex& vertex) {
        pos = vertex.pos;
        color = vertex.color;
        uv = vertex.uv;
        normal = vertex.normal;
        tan = vertex.tan;
        bitan = vertex.bitan;
        return *this;
    }

    Vertex& operator=(Vertex&& vertex) noexcept {
        pos = std::move(vertex.pos);
        color = std::move(vertex.color);
        uv = std::move(vertex.uv);
        normal = std::move(vertex.normal);
        tan = std::move(vertex.tan);
        bitan = std::move(vertex.bitan);
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
        os << "Vertex = [";
        os << "(" << vertex.pos.x << "," << vertex.pos.y << "," << vertex.pos.z << "),";
        os << "(" << vertex.color.r << "," << vertex.color.g << "," << vertex.color.b << "),";
        os << "(" << vertex.uv.x << "," << vertex.uv.y << "),";
        os << "(" << vertex.normal.x << "," << vertex.normal.y << "," << vertex.normal.z << ")]";
        return os;
    }
};

}