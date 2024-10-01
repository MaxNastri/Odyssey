#pragma Compute
struct Particle
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float Lifetime;
    float MaxLifetime;
    float Size;
    float Speed;
};

static const uint Add = 1;
static const uint Subtract = -1;

static const uint DEAD_COUNT_OFFSET = 0;
static const uint ALIVE_PRE_SIM_COUNT_OFFSET = DEAD_COUNT_OFFSET + 4;
static const uint ALIVE_POST_SIM_COUNT_OFFSET = ALIVE_PRE_SIM_COUNT_OFFSET + 4;

RWStructuredBuffer<Particle> ParticleBuffer : register(b2);
RWByteAddressBuffer CounterBuffer : register(b3);
RWStructuredBuffer<uint> AliveBufferPreSim : register(b4);
RWStructuredBuffer<uint> AliveBufferPostSim : register(b5);
RWStructuredBuffer<uint> DeadBuffer : register(b6);

// Per particle
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint alivePreSimCount = CounterBuffer.Load(ALIVE_PRE_SIM_COUNT_OFFSET);
    
    if (id.x >= alivePreSimCount)
        return;
    
    uint particleIndex = AliveBufferPreSim[id.x];
    float dt = (1.0f / 144.0f);
    Particle particle = ParticleBuffer[particleIndex];
    particle.Lifetime += dt;
    
    if (particle.Lifetime > particle.MaxLifetime)
    {
        // Increment the dead count
        uint deadCount;
        CounterBuffer.InterlockedAdd(DEAD_COUNT_OFFSET, Add, deadCount);
        
        // Assign the particle index to the dead buffer
        DeadBuffer[deadCount] = particleIndex;
        return;
    }
    
    // Sim the particle and assign it back to the buffer
    particle.Position += particle.Velocity * particle.Speed * dt;
    ParticleBuffer[particleIndex] = particle;
    
    // Increment the post-sim alive count and push the particle index
    uint alivePostSimCount;
    CounterBuffer.InterlockedAdd(ALIVE_POST_SIM_COUNT_OFFSET, Add, alivePostSimCount);
    AliveBufferPostSim[alivePostSimCount] = particleIndex;

}

#pragma Vertex
struct Particle
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float Lifetime;
    float MaxLifetime;
    float Size;
    float Speed;
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

StructuredBuffer<Particle> ParticleBufferVS : register(b2);
StructuredBuffer<uint> AliveBuffer : register(b4);

VertexOutput main(uint id : SV_VertexID)
{
    uint aliveIndex = id / 6;
    uint particleIndex = AliveBuffer[aliveIndex];
    Particle particle = ParticleBufferVS[particleIndex];
    
    VertexOutput output;
    
    float2 offsets[6] =
    {
        float2(-1, 1),
        float2(1, 1),
        float2(-1, -1),
        float2(-1, -1),
        float2(1, 1),
        float2(1, -1),
    };
    
    float3 camRight = View[0].xyz;
    float3 camUp = View[1].xyz;
    float2 offset = offsets[id % 6];
    
    output.Position = float4(particle.Position.xyz + (camRight * offset.x * (particle.Size * 0.5f)) + (camUp * offset.y * (particle.Size * 0.5f)), 1.0f);
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