#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D img_output;

struct Ray {
    vec3 origin;
    vec3 direction;
};

vec3 traceRay(Ray ray) {

    // Simple ray-sphere intersection test
    vec3 sphereCenter = vec3(0.0, 0.0, -5.0);
    float sphereRadius = 3.0;

    vec3 oc = ray.origin - sphereCenter;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0.0) {
        return vec3(1.0, 0.0, 0.0); // Red color for hit
    } else {
        return vec3(0.0, 0.0, 1.0); // Blue color for miss
    }
}

void main() {
    
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = (vec2(pixelCoord) / vec2(imageSize(img_output))) * 2.0 - 1.0;

    Ray ray;
    ray.origin = vec3(0.0, 0.0, 0.0);
    ray.direction = normalize(vec3(uv, -1.0));

    vec3 color = traceRay(ray);
    imageStore(img_output, pixelCoord, vec4(color, 1.0));
}