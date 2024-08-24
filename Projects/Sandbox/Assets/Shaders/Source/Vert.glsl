#version 450

layout(set = 0, binding = 0) uniform UBOScene {
    mat4 view;
    mat4 proj;
} uboCamera;

layout(set = 0, binding = 1) uniform UBOModel {
    mat4 model;
} uboModel;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec2 inUv0;
layout(location = 5) in vec2 inUv1;
layout(location = 6) in vec2 inUv2;
layout(location = 7) in vec2 inUv3;
layout(location = 8) in vec2 inUv4;
layout(location = 9) in vec2 inUv5;
layout(location = 10) in vec2 inUv6;
layout(location = 11) in vec2 inUv7;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV0;

void main() {
    gl_Position = uboCamera.proj * uboCamera.view * uboModel.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragNormal = inNormal;
    fragUV0 = inUv0;
}