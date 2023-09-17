#include "Struct/ShaderStructs.hlsli"
#include "PBRFunctions.hlsli"
#include "ShaderFunctions.hlsli"

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D pbrTexture : register(t2);
Texture2D ambientOcclusionTexture : register(t5);
Texture2D ScreenUVTexture : register(t6);
Texture2D shadowMap : register(t7);
Texture2D SSAOTexture : register(t8);
StructuredBuffer<LightData> lights : register(t9);
TextureCube environmentTexture : register(t10);

PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    
    float albedoTime = frac(FB_TotalTime * 0.2f);
    //albedoTime *= albedoTime;
    float normalTime = frac(FB_TotalTime * 0.173f);
    //normalTime *= normalTime;

    float mask = albedoTexture.Sample(wrapSampler, input.AlbedoUV + albedoTime).r;
    //float4 albedo = mask * MB_Color;
    //if (albedo.a < 0.05f)
    //    discard;


    float3 normalMask = albedoTexture.Sample(wrapSampler, input.AlbedoUV + normalTime).rgb;
    
    mask = lerp(normalMask.r, mask, pow(sin(FB_TotalTime), 2));

    float4 albedo = lerp(float4(normalMask, 0), MB_Color, mask);
    //albedo.rgb += normalMap * 0.3f;

    result.id = OB_id;
    result.Color = albedo * MB_EmissiveStrength;
    //result.Color = albedoTexture.Sample(defaultSampler, input.AlbedoUV);
    result.WorldPosition = input.WorldPosition;

    return result;
}