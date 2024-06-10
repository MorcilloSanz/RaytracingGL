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

// ----------------------------------------------------------------------------
//
// Functions
//
// ----------------------------------------------------------------------------

struct Ray {
    vec3 origin;
    vec3 direction;
};


void main() {

	vec2 imageSize = vec2(1000, 1000);
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 xy = 2.0 * pixelCoord / imageSize - 1.0;

	Ray ray;
    ray.origin = vec3(xy.x, xy.y, 0.0);
    ray.direction = vec3(0.0, 0.0, -1.0);

	vec3 color = vec3(0.0);

	color = vec3(pixelCoord / imageSize, 1.0);
    //color = vertices[1].color;

    imageStore(imgOutput, pixelCoord, vec4(color, 1.0));
}