#pragma Compute
struct Particle
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float2 Lifetime;
    float Size;
    float Speed;
};

static const uint Add = 1;
static const uint Subtract = -1;

static const uint DEAD_COUNT_OFFSET = 0;
static const uint ALIVE_PRE_SIM_COUNT_OFFSET = DEAD_COUNT_OFFSET + 4;
static const uint ALIVE_POST_SIM_COUNT_OFFSET = DEAD_COUNT_OFFSET + 8;
static const uint TEST_COUNT_OFFSET = DEAD_COUNT_OFFSET + 12;

RWStructuredBuffer<Particle> ParticleBuffer : register(b2);
RWByteAddressBuffer CounterBuffer : register(b3);
RWStructuredBuffer<uint> AliveBufferPreSim : register(b4);
RWStructuredBuffer<uint> AliveBufferPostSim : register(b5);
RWStructuredBuffer<uint> DeadBuffer : register(b6);

cbuffer EmitterData : register(b7)
{
    float4 Position;
    float4 StartColor;
    float4 EndColor;
    float4 Velocity;
    float4 Rnd;
    float2 Lifetime;
    float2 Size;
    float2 Speed;
    uint EmitCount;
    uint EmitterIndex;
}

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
    particle.Lifetime.x -= dt;
    
    if (particle.Lifetime.x <= 0.0f)
    {
        // Increment the dead count
        uint deadCount;
        CounterBuffer.InterlockedAdd(DEAD_COUNT_OFFSET, Add, deadCount);
        
        uint testCount;
        CounterBuffer.InterlockedAdd(TEST_COUNT_OFFSET, Add, testCount);
        
        // Assign the particle index to the dead buffer
        DeadBuffer[deadCount] = particleIndex;
        return;
    }
    
    // Sim the particle and assign it back to the buffer
    particle.Color = lerp(StartColor, EndColor, (particle.Lifetime.y - particle.Lifetime.x) / particle.Lifetime.y);
    particle.Position += particle.Velocity * particle.Speed * dt;
    ParticleBuffer[particleIndex] = particle;
    
    // Increment the post-sim alive count and push the particle index
    uint alivePostSimCount;
    CounterBuffer.InterlockedAdd(ALIVE_POST_SIM_COUNT_OFFSET, Add, alivePostSimCount);
    AliveBufferPostSim[alivePostSimCount] = particleIndex;
}