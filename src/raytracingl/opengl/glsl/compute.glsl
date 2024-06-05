#version 430 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D outputImage;

/*
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

void main() {

    vec2 imageSize = vec2(640, 480);
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 xy = (vec2(pixelCoord) / vec2(imageSize)) * 2.0 - 1.0;

    Ray ray;
    ray.origin = vec3(0.0, 0.0, 0.0);
    ray.direction = normalize(vec3(xy, -1.0));

    vec3 color = vec3(vec2(gl_GlobalInvocationID.xy) / vec2(640, 480), 0);

    imageStore(outputImage, pixelCoord, vec4(color, 1.0));
}
*/

void main() {

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec2 imageSize = vec2(640, 480);
    if(texelCoord.x >= imageSize.x || texelCoord.y >= imageSize.y)
        return;
	
    vec4 value = vec4(texelCoord.x / imageSize.x, texelCoord.y / imageSize.y, 1.0, 1.0);

    imageStore(outputImage, texelCoord, value);
}