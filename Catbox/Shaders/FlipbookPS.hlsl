#include "Struct/ShaderStructs.hlsli"
#include "ShaderFunctions.hlsli"

cbuffer FlipbookBuffer : register(b10)
{
    int Rows = 8;
    int Columns = 8;
    int Framerate = 24;
    bool Loop = true;
}

Texture2D albedoTexture : register(t0);


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    
    float time = StartDelay(MB_CustomValue1, OB_AliveTime);
    if (MB_CustomValue2 > 0)
        time = StopDelay(MB_CustomValue2, time);

    float2 activeFrame = FlipBook(input.AlbedoUV, Rows, Columns, Framerate, time, Loop);

    float4 textureColor = albedoTexture.Sample(defaultSampler, activeFrame);
    textureColor *= MB_EmissiveStrength;
    if (textureColor.a < 0.05f) discard;
    result.Color = textureColor;
    result.Color.a *= textureColor.a;
    result.Color.a = saturate(result.Color.a);
    //result.id = 0;
    return result;
} 