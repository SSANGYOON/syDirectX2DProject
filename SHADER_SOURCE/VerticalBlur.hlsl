#include "global.hlsli"

Texture2D inputImage : register(t0);
RWTexture2D<float4> outputImage: register(u0);

groupshared float4 gCache[264];

const static float weights[5] = { 0.2270270270, 0.1945945946, 0.1216216216,
                                  0.0540540541, 0.0162162162 };

[numthreads(1, 256, 1)]
void CS_MAIN(int3 groupindex :SV_GroupThreadID, int3 globalindex : SV_DispatchThreadID)
{
    float width, height;


    float2 index = min(globalindex.xy, ImgSize);
    float2 coord = index / ImgSize;

    gCache[groupindex.y + 4] = inputImage.SampleLevel(linearSampler, coord, 0);

    if (groupindex.y < 4)
    {
        int y = max(globalindex.y - 4, 0);
        coord.y = (y / ImgSize.y);
        gCache[groupindex.y] = inputImage.SampleLevel(linearSampler, coord, 0);
    }
    if (groupindex.y >= 252)
    {
        int y = min(globalindex.y + 4, ImgSize.y);
        coord.y = (y / ImgSize.y);
        gCache[groupindex.y + 8] = inputImage.SampleLevel(linearSampler, coord, 0);
    }

    GroupMemoryBarrierWithGroupSync();

    float4 blurColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    [unroll]
    for (int i = -4; i <= 4; ++i)
    {
        int k = groupindex.y + 4 + i;
        blurColor += gCache[k] * weights[abs(i)];
    }

    if (globalindex.x < ImgSize.x && globalindex.y < ImgSize.y)
        outputImage[globalindex.xy] = blurColor;

}