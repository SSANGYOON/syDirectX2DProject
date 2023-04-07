#include "global.hlsli"

struct Particle
{
    float3  worldPos;
    float   curTime;
    float3  worldDir;
    float   lifeTime;
    int     alive;
    float3  padding;
};

struct ComputeShared
{
    int addCount;
    float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

[numthreads(16, 16, 1)]
void CS_MAIN(int3 DispatchID : SV_DispatchThreadID, int GroupIndex : SV_GroupIndex)
{

    int maxCount = g_int_0;
    int addCount = g_int_1;
    int type = g_int_2;

    float deltaTime = g_vec2_0.x;
    float accTime = g_vec2_0.y;

    float2 particleMinGenRange = g_vec2_1;
    float2 particleMaxGenRange = g_vec2_2;
    float2 particleSpriteSize = g_vec2_3;

    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;
    float minSpeed = g_vec4_0.z;
    float maxSpeed = g_vec4_0.w;
    float4 targetOffset = g_vec4_1;

    g_shared[0].addCount = addCount;
    GroupMemoryBarrierWithGroupSync();

    if (g_particle[GroupIndex].alive == 0)
    {
        while (true)
        {
            int remaining = g_shared[0].addCount;
            if (remaining <= 0)
                break;

            int expected = remaining;
            int desired = remaining - 1;
            int originalValue;
            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);

            if (originalValue == expected)
            {
                g_particle[GroupIndex].alive = 1;
                break;
            }
        }

        float x = DispatchID.x;
        float y = DispatchID.y;
        if (g_particle[GroupIndex].alive == 1)
        {
            float r1 = Rand(float2(x, y));
            float r2 = Rand(float2(y, x));
            float r3 = Rand(float2(x * accTime, y));
            float r4 = Rand(float2(x, y * accTime));
            float r5 = Rand(float2(x * accTime, y * accTime));
            // [0.5~1] -> [0~1]
            float4 noise =
            {
                2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1,
                2 * r4 - 1
            };

            float3 dir = float3(normalize((noise.xy - 0.5f) * 2.f), 0.f);

            float2 particleGenPos =
                float2(particleMinGenRange.x + (particleMaxGenRange.x - particleMinGenRange.x) * noise.z,
                    particleMinGenRange.y + (particleMaxGenRange.y - particleMinGenRange.y) * noise.w);

            if (type == 0) {
                g_particle[GroupIndex].worldDir = dir;
                g_particle[GroupIndex].worldPos = dir * particleGenPos.x + targetOffset.xyz;
            }

            else {
                g_particle[GroupIndex].worldDir = float3(0, 1, 0);
                g_particle[GroupIndex].worldPos = float3(dir.xy * particleGenPos, 0) + targetOffset.xyz;;
            }

            g_particle[GroupIndex].lifeTime = (maxLifeTime - minLifeTime) * (2 * r5 - 1) + minLifeTime;
            g_particle[GroupIndex].curTime = 0;
        }
    }
    else
    {
        g_particle[GroupIndex].curTime += deltaTime;
        if (g_particle[GroupIndex].lifeTime < g_particle[GroupIndex].curTime)
        {
            g_particle[GroupIndex].alive = 0;
            return;
        }

        float ratio = g_particle[GroupIndex].curTime / g_particle[GroupIndex].lifeTime;
        float speed = (maxSpeed - minSpeed) * ratio + minSpeed;
        g_particle[GroupIndex].worldPos += g_particle[GroupIndex].worldDir * speed * deltaTime;
    }
}