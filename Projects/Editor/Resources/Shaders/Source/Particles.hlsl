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

RWStructuredBuffer<Particle> ParticleBuffer : register(b5);

[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float dt = (1.0f / 144.0f);
    ParticleBuffer[id.x].Lifetime += dt;
    ParticleBuffer[id.x].Position += ParticleBuffer[id.x].Velocity * dt;
}
#pragma Vertex
struct Particle
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float Lifetime;
    float Size;
    float SizeOverLifetime;
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

StructuredBuffer<Particle> ParticleBufferVS : register(b5);

VertexOutput main(uint id : SV_VertexID)
{
    Particle particle = ParticleBufferVS[id / 6];
    
    VertexOutput output;
    
    float2 offsets[6] =
    {
        float2(-1, 1), // 0
        float2(1, 1), // 1
        float2(-1, -1), // 2
        float2(-1, -1), // 3
        float2(1, 1), // 4
        float2(1, -1), // 5
    };
    
    float3 camRight = View[0].xyz;
    float3 camUp = View[1].xyz;
    float2 offset = offsets[id % 6];
    
    output.Position = float4(particle.Position.xyz + (camRight * offset.x * (0.5f * 0.5f)) + (camUp * offset.y * (0.5f * 0.5f)), 1.0f);
    output.Position = mul(ViewProjection, output.Position);
    output.Color = particle.Color;
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