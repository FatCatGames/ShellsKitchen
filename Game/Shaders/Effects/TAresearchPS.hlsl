#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/PBRFunctions.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../Catbox/Shaders/Color/ColorConversion.hlsli"

Texture2D albedoTexture : register(t0);
Texture2D Noise : register(t11);


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    
    float time = FB_TotalTime;
    
    float timeR = FB_TotalTime;
    float timeG = FB_TotalTime * 0.33f;
    float timeB = FB_TotalTime * 0.17f;
    
    float fracTime = frac(time * 0.5f);
    float fracTimeHalf = frac(time*0.5f);
    
    float fracTimeSlower = frac(time*0.15f);
    
    float3 albedo = albedoTexture.Sample(wrapSampler, input.AlbedoUV).rgb;
    
    float2 UVpanningText = float2(input.AlbedoUV.x, input.AlbedoUV.y + fracTime);
    float2 UVpanningLines = float2(input.AlbedoUV.x + fracTime, input.AlbedoUV.y);
    float2 UVpanningSwipe = float2(input.AlbedoUV.x, input.AlbedoUV.y - fracTimeHalf);
    
    float2 UVpanningScanLines = float2(input.AlbedoUV.x + fracTimeSlower, input.AlbedoUV.y);
    
    //float4 albedo = albedoTexture.Sample(defaultSampler, panningText).rgba;

    float panningText = albedoTexture.Sample(wrapSampler, UVpanningText).r;
    float panningLines = albedoTexture.Sample(wrapSampler, UVpanningLines).g;
    float panningSwipe = albedoTexture.Sample(wrapSampler, UVpanningSwipe).a;
    
    float2 worldPosTexture = float2(input.WorldPosition.y+fracTimeSlower, input.WorldPosition.x);
    
    float scanLines = (albedoTexture.Sample(wrapSampler, worldPosTexture).g * 0.25f) * input.VxColor.r;
    
    //float3 Color = float3(1, 0, 0);
    //float3 RGB2HSV = rgb2hsv(float3(sin(Color.r) * MB_CustomFloat4.x * fracTime, Color.g, Color.b));
    //float3 RGB2HSVcycle = float3(RGB2HSV.r, RGB2HSV.g, RGB2HSV.b);
    
    float3 colorShift = float3(abs(sin(timeR)), abs(sin(timeG)), abs(sin(timeB)));
    
    result.Color.rgb = ((panningText * panningLines) * 2) * panningSwipe * colorShift * 20 * input.VxColor.r + scanLines;
    
    result.Color.a = 1;
    return result;
}