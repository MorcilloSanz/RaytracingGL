#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D outputImage;

layout(std430, binding = 0) buffer VertexBuffer {
    vec3 positions[8];
    vec4 colors[8];
    vec2 uvs[8];
    vec3 normals[8];
    vec3 tans[8];
    vec3 bitans[8];
};

layout(std430, binding = 1) buffer IndexBuffer {
    uint indices[36];
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

bool rayTriangleIntersect(vec3 orig, vec3 dir, vec3 vert0, vec3 vert1, vec3 vert2, out float t, out float u, out float v) {

    vec3 edge1 = vert1 - vert0;
    vec3 edge2 = vert2 - vert0;
    vec3 pvec = cross(dir, edge2);
    float det = dot(edge1, pvec);

    if (det < 1e-6) return false;

    vec3 tvec = orig - vert0;
    u = dot(tvec, pvec);
    if (u < 0.0 || u > det) return false;

    vec3 qvec = cross(tvec, edge1);
    v = dot(dir, qvec);
    if (v < 0.0 || u + v > det) return false;

    t = dot(edge2, qvec);
    float invDet = 1.0 / det;
    t *= invDet;
    u *= invDet;
    v *= invDet;

    return true;
}

vec3 traceRay(Ray ray) {

    vec3 hitColor = vec3(0.0);

    for (int i = 0; i < indices.length(); i += 3) {

        vec3 v0 = positions[indices[i]];
        vec3 v1 = positions[indices[i + 1]];
        vec3 v2 = positions[indices[i + 2]];

        float t, u, v;

        if (rayTriangleIntersect(ray.origin, ray.direction, v0, v1, v2, t, u, v)) {
            hitColor = vec3(1.0, 0.0, 0.0); // Color para hit
            break; // Termina la iteración si hay una intersección
        }
    }

    return hitColor;
}

void main() {
    
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 xy = (vec2(pixelCoord) / vec2(imageSize(outputImage))) * 2.0 - 1.0;

    Ray ray;
    ray.origin = vec3(0.0, 0.0, 0.0);
    ray.direction = normalize(vec3(xy, -1.0));

    vec3 color = traceRay(ray);
    imageStore(outputImage, pixelCoord, vec4(color, 1.0));
}