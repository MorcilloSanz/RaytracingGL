#pragma once

#include <iostream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "raytracingl/ptr.h"

namespace rgl
{

struct Vertex {

    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uvs;
    glm::vec3 normal;
    glm::vec3 tan, bitan;

    Vertex(const glm::vec3& _pos, const glm::vec3& _color, const glm::vec3& _normal, const glm::vec2& _uvs)
        : pos(_pos), color(_color), normal(_normal), uvs(_uvs), tan(1.f), bitan(1.f) {
    }

    Vertex(const glm::vec3& _pos, const glm::vec3& _color, const glm::vec3& _normal)
        : pos(_pos), color(_color), normal(_normal), uvs(1.0), tan(1.f), bitan(1.f) {
    }

    Vertex(const glm::vec3& _pos, const glm::vec3& _color)
        : pos(_pos), color(_color), normal(1.f), uvs(1.f), tan(1.f), bitan(1.f) {
    }

    Vertex(const glm::vec3& _pos)
        : pos(_pos), color(1.f), normal(1.f), uvs(1.f), tan(1.f), bitan(1.f) {
    }

    Vertex() = default;
    ~Vertex() = default;

    Vertex(const Vertex& vertex)
        : pos(vertex.pos), color(vertex.color), uvs(vertex.uvs), normal(vertex.normal),
        tan(vertex.tan), bitan(vertex.bitan) {
    }

    Vertex(Vertex&& vertex) noexcept
        : pos(std::move(vertex.pos)), color(std::move(vertex.color)), uvs(std::move(vertex.uvs)),
        normal(std::move(vertex.normal)), tan(std::move(vertex.tan)), bitan(std::move(vertex.bitan)) {
    }

    Vertex& operator=(const Vertex& vertex) {
        pos = vertex.pos;
        color = vertex.color;
        uvs = vertex.uvs;
        normal = vertex.normal;
        tan = vertex.tan;
        bitan = vertex.bitan;
        return *this;
    }

    Vertex& operator=(Vertex&& vertex) noexcept {
        pos = std::move(vertex.pos);
        color = std::move(vertex.color);
        uvs = std::move(vertex.uvs);
        normal = std::move(vertex.normal);
        tan = std::move(vertex.tan);
        bitan = std::move(vertex.bitan);
        return *this;
    }
};

}