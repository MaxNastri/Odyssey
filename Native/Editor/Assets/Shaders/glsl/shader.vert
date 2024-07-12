#version 450

layout(set = 0, binding = 0) uniform UBOScene {
    mat4 view;
    mat4 proj;
} uboCamera;

layout(set = 0, binding = 1) uniform UBOModel {
    mat4 model;
} uboModel;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUv0;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV0;

void main() {
    gl_Position = uboCamera.proj * uboCamera.view * uboModel.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragNormal = inNormal;
    fragUV0 = inUv0;
}