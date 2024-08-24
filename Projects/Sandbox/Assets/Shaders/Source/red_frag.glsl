#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV0;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}