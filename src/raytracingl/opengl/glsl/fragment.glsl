#version 430 core

in vec4 Color;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main() {
   FragColor = Color * texture(screenTexture, TexCoords);
}