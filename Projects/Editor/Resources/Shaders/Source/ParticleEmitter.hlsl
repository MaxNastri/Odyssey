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
static const uint ALIVE_POST_SIM_COUNT_OFFSET = ALIVE_PRE_SIM_COUNT_OFFSET + 4;

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

// Random number generator based on: https://github.com/diharaw/helios/blob/master/src/engine/shader/random.glsl
//struct RNG
//{
//    uint2 s; // state

//	// xoroshiro64* random number generator.
//	// http://prng.di.unimi.it/xoroshiro64star.c
//    uint rotl(uint x, uint k)
//    {
//        return (x << k) | (x >> (32 - k));
//    }
//	// Xoroshiro64* RNG
//    uint next()
//    {
//        uint result = s.x * 0x9e3779bb;

//        s.y ^= s.x;
//        s.x = rotl(s.x, 26) ^ s.y ^ (s.y << 9);
//        s.y = rotl(s.y, 13);

//        return result;
//    }
//	// Thomas Wang 32-bit hash.
//	// http://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
//    uint hash(uint seed)
//    {
//        seed = (seed ^ 61) ^ (seed >> 16);
//        seed *= 9;
//        seed = seed ^ (seed >> 4);
//        seed *= 0x27d4eb2d;
//        seed = seed ^ (seed >> 15);
//        return seed;
//    }

//    void init(uint2 id, uint frameIndex)
//    {
//        uint s0 = (id.x << 16) | id.y;
//        uint s1 = frameIndex;
//        s.x = hash(s0);
//        s.y = hash(s1);
//        next();
//    }
//    float next_float()
//    {
//        uint u = 0x3f800000 | (next() >> 9);
//        return asfloat(u) - 1.0;
//    }
//    uint next_uint(uint nmax)
//    {
//        float f = next_float();
//        return uint(floor(f * nmax));
//    }
//    float2 next_float2()
//    {
//        return float2(next_float(), next_float());
//    }
//    float3 next_float3()
//    {
//        return float3(next_float(), next_float(), next_float());
//    }
//};

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
    
    // Get the index of the revived particle and assign it to the particle buffer
    uint particleIndex = DeadBuffer[deadCount - 1];
    Particle particle = ParticleBuffer[particleIndex];
    
    float lifetime = lerp(Lifetime.x, Lifetime.y, Rnd.w);
    // Construct a particle based on the emitter's params
    particle.Position = Position + float4(Rnd.x, Rnd.y, Rnd.z, 0.0f);
    particle.Color = StartColor;
    particle.Velocity = Velocity;
    particle.Lifetime = float2(lifetime, lifetime);
    particle.Size = lerp(Size.x, Size.y, Rnd.w);
    particle.Speed = lerp(Speed.x, Speed.y, Rnd.w);
    
    ParticleBuffer[particleIndex] = particle;
    
    // Add the index to the pre-sim alive list (push):
    uint aliveCount;
    CounterBuffer.InterlockedAdd(ALIVE_PRE_SIM_COUNT_OFFSET, Add, aliveCount);
    AliveBufferPreSim[aliveCount] = particleIndex;
}