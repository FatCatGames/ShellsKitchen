#include "Struct/ShaderStructs.hlsli"
#include "ShaderFunctions.hlsli"
//#include "Functions/Ease.hlsli"

Texture2D albedoTexture : register(t12);
Texture2D depthTexture : register(t11);
Texture2D worldPosition : register(t4);

//ProjectedPosition = localspace in here
float RandNumber(int pos) //Grundfunktionen f?r randomisering
{
    
    unsigned int seed = 2147483647;
    
    unsigned int BIT_NOISE1 = 0xB5297A4D;
    unsigned int BIT_NOISE2 = 0x68E31DA4;
    unsigned int BIT_NOISE3 = 0x1B56C4E9;
    
    unsigned int mangled = (unsigned int) pos;
    mangled *= BIT_NOISE1;
    mangled += seed;
    mangled ^= (mangled >> 8);
    mangled += BIT_NOISE2;
    mangled ^= (mangled << 8);
    mangled *= BIT_NOISE3;
    mangled ^= (mangled >> 8);
    return (float) mangled / 0xffffffff;
	//STOLEN FROM SQUIRREL EISERLOH
}

PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    
    //float2 screenTex = float2(0.5,0.5); //input.WorldPosition.xy / FB_Resolution;

    //uint width;
    //uint height;
    
    //depthTexture.GetDimensions(width, height);
    
    //float newDepth = depthTexture.Sample(defaultSampler, input.AlbedoUV).r;
    //newDepth = 1 - saturate(((newDepth - 0.98f) * 50.f));
    //float4 col = saturate(((input.VxColor.r - 0.98f) * 50.f));
    
    //result.id = OB_id;

    //float4 cool = float4(newDepth, newDepth, newDepth, 1);
    
    //float2 proj = input.ProjectedPosition.xy / input.ProjectedPosition.w;
    
    //float2 texCoord = float2((1.f + proj.x) / 2.f + (0.5f / FB_Resolution.x), (1.f - proj.y) / 2.f + (0.5f / FB_Resolution.y));
    //float depthSample = depthTexture.Sample(defaultSampler, texCoord).r;
    //depthSample = 1 - saturate(((depthSample - 0.98f) * 50.f));
    //result.Color = float4(depthSample, depthSample, depthSample, 1);
    //return result;
    
    float3 worldPos = input.WorldPosition.xyz - float3(OB_ToWorld[3][0], OB_ToWorld[3][1], OB_ToWorld[3][2]);
    
    float calcTime = FB_TotalTime * MB_CustomValue1;
    float noiseTime = calcTime + SimplexNoise(float2(calcTime, calcTime));
    float noiseFun = SimplexNoise(input.AlbedoUV * calcTime);
    
    //float heightNoise = worldPos.y + worldPos.x * SimplexNoise(input.Normal.xy) *
    //max(min(BellCurve(frac(FB_TotalTime * 0.1f)), 0.2f), 0.5f) * MB_CustomValue2;
    
    float heightNoise = input.AlbedoUV.y + input.AlbedoUV.x * SimplexNoise(input.Normal.xy) *
    max(min(BellCurve(frac(FB_TotalTime * 0.1f)), 0.2f), 0.5f) * MB_CustomValue2;
    
    
    float lineA = frac(heightNoise * MB_CustomValue1) * MB_CustomValue1;
    float lineC = frac((heightNoise * MB_CustomValue1) * 0.5f) * MB_CustomValue1;
    float lineB = lineA + noiseFun * MB_CustomValue3;
    float lineD = lineC + noiseFun * MB_CustomValue3;

    if (lineB < noiseTime % MB_CustomValue1 * 1.1f && lineB > noiseTime % MB_CustomValue1)
    {
        discard;
    }
    if (lineD < noiseTime % MB_CustomValue1 * 1.1f && lineD > noiseTime % MB_CustomValue1)
    {
        discard;
    }

    
        float4 albedo = AlbedoTex.Sample(wrapSampler, float2(input.AlbedoUV.x + noiseFun * 0.02f, input.AlbedoUV.y));
    albedo *= MB_Color; // float4(0.2f, 0.4, 0.8f, 1);
    
    const float3 toEye = normalize(FB_CamPosition.xyz - input.WorldPosition.xyz);
    float dotProduct = abs(sin(dot(toEye, input.Normal)));
    //float dotProductofLight = abs(dot(rotlightDirection, input.Normal));
    //float mixdotProduct = dotProduct * dotProductofLight;
    //albedo.a *= lerp(0, 1, dotProduct);
    
    //albedo.a -= 1 - (distance(worldPos, input.WorldPosition.xyz) * 0.01f);
    albedo.rgb *= 10;
    result.Color = albedo;
    float randPixel = RandNumber(input.WorldPosition.x * input.WorldPosition.y * calcTime);
    if (randPixel < 0.35f)
    {
        result.Color = float4(randPixel, randPixel, randPixel, 0);
    }
    
    //result.Color = float4(1, 1, 1, 1);
    result.id = OB_id;
    return result;
}