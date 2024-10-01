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

cbuffer EmitterData : register(b7)
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float3 Rnd;
    float Lifetime;
    float Size;
    float Speed;
    uint EmitCount;
    uint EmitterIndex;
}

float random(float uv)
{
    return frac(sin(uv) * 43758.5453123);
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    // Don't emit more than we should
    if (id.x >= EmitCount)
        return;
    
    // Revive a dead particle
    int deadCount;
    CounterBuffer.InterlockedAdd(DEAD_COUNT_OFFSET, Subtract, deadCount);
    
    // Make sure its a valid particle
    if (deadCount < 1)
        return;

    // Construct a particle based on the emitter's params
    Particle particle;
    particle.Position = float4(Rnd.x, Rnd.y, Rnd.z, 0.0f);
    particle.Color = Color;
    particle.Velocity = Velocity;
    particle.Lifetime = Lifetime;
    particle.Size = Size;
    particle.Speed = Speed;
    
    // Get the index of the revived particle and assign it to the particle buffer
    uint particleIndex = DeadBuffer[deadCount - 1];
    ParticleBuffer[particleIndex] = particle;
    
    // Add the index to the pre-sim alive list (push):
    uint aliveCount;
    CounterBuffer.InterlockedAdd(ALIVE_PRE_SIM_COUNT_OFFSET, Add, aliveCount);
    AliveBufferPreSim[aliveCount] = particleIndex;
}