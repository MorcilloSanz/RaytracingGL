#version 430 core

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

// ----------------------------------------------------------------------------
//
// uniforms
//
// ----------------------------------------------------------------------------

layout(rgba32f, binding = 0) uniform image2D imgOutput;

layout(std430, binding = 0) buffer VertexBuffer {
    vec3 positions[3];
    vec3 colors[3];
	vec3 normals[3];
    vec2 uvs[3];
    vec3 tans[3];
    vec3 bitans[3];
};

layout(std430, binding = 1) buffer IndexBuffer {
    uint indices[3];
};

layout (location = 0) uniform float t;

// ----------------------------------------------------------------------------
//
// functions
//
// ----------------------------------------------------------------------------

struct Ray {
    vec3 origin;
    vec3 direction;
};

// Möller-Trumbore intersection algorithm
bool rayIntersectsTriangle(Ray ray, vec3 vertex0, vec3 vertex1, vec3 vertex2, out float t) {

	const float EPSILON = 0.0000001;

    vec3 edge1 = vertex1 - vertex0;
    vec3 edge2 = vertex2 - vertex0;
    
    vec3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);
    
    // If a is close to 0, the ray is parallel to the triangle
    if (a > -EPSILON && a < EPSILON)
        return false;

    float f = 1.0 / a;
    vec3 s = ray.origin - vertex0;
    float u = f * dot(s, h);
    
    // If u is outside the range [0, 1], there's no intersection
    if (u < 0.0 || u > 1.0)
        return false;

    vec3 q = cross(s, edge1);
    float v = f * dot(ray.direction, q);
    
    // If v is outside the range [0, 1], or u + v > 1, there's no intersection
    if (v < 0.0 || u + v > 1.0)
        return false;

    // Compute t to find out where the intersection point is on the ray
    t = f * dot(edge2, q);
    
    // If t is less than 0, the intersection is behind the ray origin
    if (t > EPSILON) 
    {
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection
        return false;
}

void main() {

	vec2 imageSize = vec2(1000, 1000);
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 xy = 2.0 * pixelCoord / imageSize - 1.0;

	Ray ray;
    ray.origin = vec3(xy.x, xy.y, 0.0);
    ray.direction = vec3(0.0, 0.0, -1.0);

	vec3 color = vec3(0.0);

	for(int i = 0; i < 3; i += 3) {

		vec3 v0 = positions[indices[i]];
		vec3 v1 = positions[indices[i + 1]];
		vec3 v2 = positions[indices[i + 2]];

		float t = 0.0;

		if(rayIntersectsTriangle(ray, v0, v1, v2, t)) {

			vec3 intersection = ray.origin + ray.direction * t;
			color = vec3(1.0);
			break;
		}
	}

	color = vec3(pixelCoord / imageSize, 1.0);

    imageStore(imgOutput, pixelCoord, vec4(color, 1.0));
}