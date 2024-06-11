#version 430 core

// ----------------------------------------------------------------------------
//
// Work group
//
// ----------------------------------------------------------------------------

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

// ----------------------------------------------------------------------------
//
// Image uniform
//
// ----------------------------------------------------------------------------

layout(rgba32f, binding = 0) uniform image2D imgOutput;

// ----------------------------------------------------------------------------
//
// Uniforms
//
// ----------------------------------------------------------------------------

struct Vertex {
    vec3 pos;
    vec3 color;
    vec3 normal;
    vec2 uv;
    vec3 tan;
    vec3 bitan;
};

layout(std430, binding = 0) buffer VertexBuffer {
    Vertex vertices[];
};

layout(std430, binding = 1) buffer IndexBuffer {
    uint indices[];
};

layout (location = 0) uniform float t;
layout (location = 1) uniform int numIndices;

// ----------------------------------------------------------------------------
//
// Functions
//
// ----------------------------------------------------------------------------

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Triangle {
    vec3 v1;
    vec3 v2;
    vec3 v3;
};

struct Sphere {
    vec3 origin;
    float radius;
};

struct HitInfo {
    vec3 intersection;
    vec3 normal;
    float dist;
    bool hit;
};

// Möller–Trumbore ray-triangle intersection algorithm
HitInfo intersectionTriangle(Ray ray, Triangle triangle) {

    const float epsilon = 0.0000001;

    HitInfo hitInfo;
    hitInfo.intersection = vec3(0.0);
    hitInfo.hit = false;

    vec3 edge1 = triangle.v2 - triangle.v1;
    vec3 edge2 = triangle.v3 - triangle.v1;
    vec3 ray_cross_e2 = cross(ray.direction, edge2);

    float det = dot(edge1, ray_cross_e2);
    if (det > -epsilon && det < epsilon) return hitInfo;

    float inv_det = 1.0 / det;
    vec3 s = ray.origin - triangle.v1;

    float u = inv_det * dot(s, ray_cross_e2);
    if (u < 0 || u > 1) return hitInfo;

    vec3 s_cross_e1 = cross(s, edge1);
    float v = inv_det * dot(ray.direction, s_cross_e1);
    if (v < 0 || u + v > 1) return hitInfo;

    float t = inv_det * dot(edge2, s_cross_e1);
    if (t > epsilon) {
        hitInfo.intersection = vec3(ray.origin + ray.direction * t);
        hitInfo.hit = true;
    }

    return hitInfo;
}

HitInfo intersectionSphere(Ray ray, Sphere sphere) {
    
    HitInfo hitInfo;
    
    vec3 oc = ray.origin - sphere.origin;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - (sphere.radius * sphere.radius);
    
    float nabla = b * b - 4.0 * a * c;
    
    if (nabla > 0.0) {
        
        float lambda1 = (-b - sqrt(nabla)) / (2.0 * a);
        float lambda2 = (-b + sqrt(nabla)) / (2.0 * a);
        float lambda = min(lambda1, lambda2);
        
        if (lambda > 0.0) {
            hitInfo.intersection = ray.origin + lambda * ray.direction;
            hitInfo.normal = normalize(hitInfo.intersection - sphere.origin);
            hitInfo.dist = lambda;
            hitInfo.hit = true;
            return hitInfo;
        }
    }
    
    hitInfo.hit = false;
    return hitInfo;
}

void main() {

	vec2 imageSize = vec2(1000, 1000);
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 xy = 2.0 * pixelCoord / imageSize - 1.0;

    Ray ray;
    ray.origin = vec3(xy.x, xy.y, 0.0);
    ray.direction = vec3(0.0, 0.0, -1.0);

	vec3 color = vec3(0.0);

    for(int i = 0; i < numIndices; i += 3) {

        Triangle triangle;
        triangle.v1 = vertices[indices[i]].pos;
        triangle.v2 = vertices[indices[i + 1]].pos;
        triangle.v3 = vertices[indices[i + 2]].pos;

        HitInfo hitInfo = intersectionTriangle(ray, triangle);
        if(hitInfo.hit) color = vec3(1.0, 0.0, 0.0);
    }

    Sphere sphere;
    sphere.origin = vec3(0.0, 0.0, -2.0);
    sphere.radius = 0.25;

    HitInfo hitInfo = intersectionSphere(ray, sphere);
    if(hitInfo.hit) color = vec3(1.0);

    imageStore(imgOutput, pixelCoord, vec4(color, 1.0));
}