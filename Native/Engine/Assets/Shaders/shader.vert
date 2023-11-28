#version 450

layout(set = 0, binding = 0) uniform UBOScene {
    mat4 view;
    mat4 proj;
} uboCamera;

layout(set = 1, binding = 0) uniform UBOModel {
    mat4 model;
} uboModel;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = uboCamera.proj * uboCamera.view * uboModel.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}