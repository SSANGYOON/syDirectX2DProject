#include "global.hlsli"

struct ComputeShared
{
    int addCount;
    float3 padding;
};

struct Particle
{
    float3 position;
    float remainLife;
    float2 velocity;
    float2 size;
    uint alive;
    float3 initialPos;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);


[numthreads(32, 32, 1)]
void CS_MAIN(uint3 DispatchID : SV_DispatchThreadID, uint GroupIndex : SV_GroupIndex)
{
    g_shared[0].addCount = addCount;
    GroupMemoryBarrierWithGroupSync();
    float pi = 3.1415928;
    if (GroupIndex >= maxParticles)
        return;

    if (g_particle[GroupIndex].alive == 0)
    {
        while (true)
        {
            uint remaining = g_shared[0].addCount;
            if (remaining <= 0)
                break;

            float x = DispatchID.x / 32.f;
            float y = DispatchID.y / 32.f;

            float r1 = Rand(float2(x, y));
            float r2 = Rand(float2(y, x));
            float r3 = Rand(float2(x * time, y));
            float r4 = Rand(float2(x, y * time));

            float r5 = Rand(float2(y * time, x));
            float r6 = Rand(float2(y, x * time));
            // [0.5~1] -> [0~1]
            float4 noise1 =
            {
                r1 - 0.5f,
                r2 - 0.5f,
                r3 - 0.5f,
                r4 - 0.5f,
            };

            float2 noise2 =
            {
                r5 - 0.5f,
                r6 - 0.5f,
            };

            float2 pos2 = (float2)0.f;

            if (PositionPolar > 0) {
                float r = LocalPosition.x + noise1.z * PositionVariation.x;

                float theta = pi / 180.f * (LocalPosition.y + noise1.w * PositionVariation.y);
                pos2 = r * float2(cos(theta), sin(theta));
                g_particle[GroupIndex].position = mul(float4(pos2, 0.f, 1.f), worldTrans).xyz;
            }
            else {
                pos2 = LocalPosition + PositionVariation * noise1.zw;
                g_particle[GroupIndex].position = mul(float4(pos2, 0.f, 1.f), worldTrans).xyz;
            }

            if (textureAttach > 0)
            {
                float2 texSize;

                tex_0.GetDimensions(texSize.x, texSize.y);

                pos2.y = -pos2.y;

                float2 uv = (targetTexturePos + pos2) / texSize;

                float4 texValue = tex_0.SampleLevel(pointSampler, uv, 0);

                if (texValue.w == 0 || uv.x > 1 || uv.y > 1 || uv.x < 0 || uv.y < 0) {
                    break;
                }
            }

            uint expected = remaining;
            uint desired = remaining - 1;
            uint originalValue;

            if (desired < 0)
                break;

            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);

            if (originalValue == expected)
            {
                g_particle[GroupIndex].alive = 1;
                break;
            }
        }

        
        if (g_particle[GroupIndex].alive == 1)
        {
            float x = DispatchID.x / 32.f;
            float y = DispatchID.y / 32.f;

            float r1 = Rand(float2(x, y));
            float r2 = Rand(float2(y, x));
            float r3 = Rand(float2(x * time, y));
            float r4 = Rand(float2(x, y * time));

            float r5 = Rand(float2(y * time, x));
            float r6 = Rand(float2(y, x * time));
            // [0.5~1] -> [0~1]
            float4 noise1 =
            {
                r1 - 0.5f,
                r2 - 0.5f,
                r3 - 0.5f,
                r4 - 0.5f,
            };

            float2 noise2 =
            {
                r5 - 0.5f,
                r6 - 0.5f,
            };

            g_particle[GroupIndex].velocity = mul(float4(Velocity + VelocityVariation * noise1.xy , 0.f, 0.f), worldTrans).xy;
            g_particle[GroupIndex].size = SizeBegin + SizeVariation * noise2;
            g_particle[GroupIndex].remainLife = LifeTime;
            g_particle[GroupIndex].initialPos = mul(float4(0.f, 0.f, 0.f, 1.f), worldTrans).xyz;
        }
    }
    else
    {
        float slice = DeltaTime / g_particle[GroupIndex].remainLife;
        g_particle[GroupIndex].velocity += (VelocityEnd - Velocity) * slice;
        g_particle[GroupIndex].size = lerp(g_particle[GroupIndex].size, SizeEnd, slice);

        float2 vel = g_particle[GroupIndex].velocity;

        if (VelocityPolar > 0) {
            g_particle[GroupIndex].initialPos = mul(float4(0.f, 0.f, 0.f, 1.f), worldTrans).xyz;
            float2 posDiff = g_particle[GroupIndex].position.xy - g_particle[GroupIndex].initialPos.xy;

            float2 normal = normalize(posDiff + float2(0.0001f, 0.f));
            float2 normalPerp = float2(normal.y, -normal.x);

            float cos;
            float sin;

            vel = normal * vel.x + normalPerp * vel.y;
        }

        g_particle[GroupIndex].position.xy += vel * DeltaTime;

        g_particle[GroupIndex].remainLife -= DeltaTime;
        if (g_particle[GroupIndex].remainLife <= 0)
            g_particle[GroupIndex].alive = 0;
    }
}