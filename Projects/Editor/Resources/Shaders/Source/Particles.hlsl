#pragma Compute
struct Particle
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float Lifetime;
    float Size;
    float SizeOverLifetime;
};

RWStructuredBuffer<Particle> ParticleBuffer : register(u5);

[numthreads(256, 1, 1)]
void main(uint3 GlobalInvocationID : SV_DispatchThreadID, uint3 LocalInvocationID : SV_GroupThreadID)
{
    uint index = GlobalInvocationID.x;
    float dt = (1.0f / 144.0f);
    ParticleBuffer[index].Lifetime += dt;
    ParticleBuffer[index].Position += ParticleBuffer[index].Velocity * dt;
}
#pragma Vertex
struct VertexInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : COLOR0;
};

cbuffer SceneData : register(b0)
{
    float4x4 View;
    float4x4 ViewProjection;
    float4 ViewPos;
}

cbuffer ModelData : register(b1)
{
    float4x4 Model;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    float4 position = float4(input.Position, 1.0f);
    output.Position = mul(Model, position);
    output.Position = mul(ViewProjection, output.Position);
    output.Color = input.Color;
    return output;
}

#pragma Fragment
struct FragmentInput
{
    float4 Position : SV_Position;
    float4 Color : COLOR0;
    float4 Velocity : VELOCITY0;
};

float4 main(FragmentInput input)
{
    return input.Color;
}