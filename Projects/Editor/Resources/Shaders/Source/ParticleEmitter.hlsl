#pragma Compute
struct Particle
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float Lifetime;
    float Size;
    float Speed;
};

struct ParticleCounts
{
    uint AliveCount;
    uint DeadCount;
};

static const uint ALIVE_COUNT_OFFSET = 0;
static const uint DEAD_COUNT_OFFSET = ALIVE_COUNT_OFFSET + 4;

RWStructuredBuffer<uint> AliveBuffer : register(b0);
RWStructuredBuffer<uint> DeadBuffer : register(b1);
RWStructuredBuffer<Particle> ParticleBuffer : register(b2);
RWByteAddressBuffer CounterBuffer : register(b3);
cbuffer EmitterData : register(b4)
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float Lifetime;
    float Size;
    float Speed;
    uint EmitCount;
    uint EmitterIndex;
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    // Pop the dead count
    uint deadCount;
    CounterBuffer.InterlockedAdd(DEAD_COUNT_OFFSET, -1, deadCount);
    
    // Make sure the dead count is valid
    if (deadCount < 1)
        return;

    Particle p;
    p.Position = Position;
    p.Color = Color;
    p.Velocity = Velocity;
    p.Lifetime = Lifetime;
    p.Size = Size;
    p.Speed = Speed;
    
    // Convert deadCount into an index (-1) and get a dead particle index
    uint particleIndex = DeadBuffer[deadCount - 1];
    
    ParticleBuffer[particleIndex] = p;
    
    // Add the index to the alive list (push):
    uint aliveCount;
    CounterBuffer.InterlockedAdd(ALIVE_COUNT_OFFSET, 1, aliveCount);
    AliveBuffer[aliveCount] = particleIndex;
}