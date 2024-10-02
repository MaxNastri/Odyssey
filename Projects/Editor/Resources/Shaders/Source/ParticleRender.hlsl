#pragma Vertex
struct Particle
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float Lifetime;
    float Size;
    float Speed;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 TexCoord0 : TEXCOORD0;
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

StructuredBuffer<Particle> ParticleBufferVS : register(b2);
StructuredBuffer<uint> AliveBuffer : register(b4);

static const float BillBoardSize = 0.125f;

VertexOutput main(uint id : SV_VertexID)
{
    uint aliveIndex = id / 6;
    uint particleIndex = AliveBuffer[aliveIndex];
    Particle particle = ParticleBufferVS[particleIndex];
    
    VertexOutput output;
    
    float2 offsets[6] =
    {
        float2(-BillBoardSize, BillBoardSize),
        float2(BillBoardSize, BillBoardSize),
        float2(-BillBoardSize, -BillBoardSize),
        float2(-BillBoardSize, -BillBoardSize),
        float2(BillBoardSize, BillBoardSize),
        float2(BillBoardSize, -BillBoardSize),
    };
    
    float2 uvs[6] =
    {
        float2(0.0f, 1.0f),
        float2(1.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f),
    };
    
    float3 camRight = View[0].xyz;
    float3 camUp = View[1].xyz;
    float2 offset = offsets[id % 6];
    float2 uv = uvs[id % 6];
    
    output.Position = float4(particle.Position.xyz + (camRight * offset.x * (particle.Size * 0.5f)) + (camUp * offset.y * (particle.Size * 0.5f)), 1.0f);
    output.Position = mul(ViewProjection, output.Position);
    output.TexCoord0 = uv;
    output.Color = particle.Color;
    return output;
}

#pragma Fragment
Texture2D diffuseTex2D : register(t3);
SamplerState diffuseSampler : register(s3);

struct FragmentInput
{
    float4 Position : SV_Position;
    float2 TexCoord0 : TEXCOORD0;
    float4 Color : COLOR0;
    float4 Velocity : VELOCITY0;
};

float4 main(FragmentInput input)
{
    return diffuseTex2D.Sample(diffuseSampler, input.TexCoord0) * input.Color;
}