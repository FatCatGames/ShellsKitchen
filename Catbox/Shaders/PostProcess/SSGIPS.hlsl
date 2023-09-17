#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"
#include "../ShaderFunctions.hlsli"
//#define VignetteRatio   1.00  //[0.15 to 6.00]  Sets a width to height ratio. 1.00 (1/1) is perfectly round, while 1.60 (16/10) is 60 % wider than it's high.
//#define VignetteRadius  1.00  //[-1.00 to 3.00] lower values = stronger radial effect from center
//#define VignetteAmount -1.00  //[-2.00 to 1.00] Strength of black. -2.00 = Max Black, 1.00 = Max White.
//#define VignetteSlope      8  //[2 to 16] How far away from the center the change should start to really grow strong (odd numbers cause a larger fps drop than even numbers)
//#define VignetteCenter float2(0.500, 0.500)  //[0.000 to 1.000, 0.000 to 1.000] Center of effect for VignetteType 1. 2 and 3 do not obey this setting.

Texture2D screenTexture : register(t0);
Texture2D depthTexture : register(t1);

#define MAX_STEPS 100
#define MAX_DIST 100.0
#define SURF_DIST 0.01

//float pulse = 30.2;
//float intensity = 6.2 * input.myVxColor;
//float speed = 0.8;
	
//float4 vertexWorldPosition;
//float4 localPos = input.myPosition;
//    localPos *= 0.001;
//float dist = length(float3(localPos.xyz));

//float pulseDist = dist * pulse;
	
//float motion = SmoothStart(pow(sin(Time * speed + pulseDist), 2));
//float3 posOffset = input.myNormal * motion;
//    posOffset.x *= 1.6;
//    posOffset *=
//intensity;

cbuffer FogData : register(b10)
{
    float4 FogColor;
    float FogDensity;
    float FogCutoff;

    float yDepthDensity = 1;
    float yDepthCutoff = -2;
    float yDepthSmoothing = -5;
    float3 padding;
    float4 yDepthColor;
};
float RandNumber(int pos) //Grundfunktionen fr randomisering
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

float3 GetPerpendicularVector(float3 normal)
{
    float3 biTangent;

    float3 c1 = cross(normal, float3(0.0, 0.0, 1.0));
    float3 c2 = cross(normal, float3(0.0, 1.0, 0.0));

    if (length(c1) > length(c2))
    {
        biTangent = c1;
    }
    else
    {
        biTangent = c2;
    }

    return biTangent;
}

// Get a cosine-weighted random vector centered around a specified normal direction.
float3 GetCosHemisphereSample(float rand1, float rand2, float3 hitNorm)
{
	// Get 2 random numbers to select our sample with
    float2 randVal = float2(rand1, rand2);

	// Cosine weighted hemisphere sample from RNG
    float3 bitangent = GetPerpendicularVector(hitNorm);
    float3 tangent = cross(bitangent, hitNorm);
    float r = sqrt(randVal.x);
    float phi = 2.0f * 3.14159265f * randVal.y;

	// Get our cosine-weighted hemisphere lobe sample direction
    return tangent * (r * cos(phi).x) + bitangent * (r * sin(phi)) + hitNorm.xyz * sqrt(max(0.0, 1.0f - randVal.x));
}
float GetDist(float3 p)
{
                         //from left, up, away, radius
    float rightLeft = (1 - sin(FB_TotalTime)) - 0.5;
    float forwardBackward = (1 - cos(FB_TotalTime)) + 7;

    float4 sphere = float4(rightLeft, 1.2, forwardBackward, 1);

    float sphereDist = length(p - sphere.xyz) - sphere.w;
    float planeDist = p.y;

    float d = min(sphereDist, planeDist);
    return d;
}



float2 RayMarch(float3 rayOrigin, float3 rayDirection, float depth)
{
    float stepSize = 1.0f;
    float3 origin = rayOrigin;
    float3 direction = rayDirection;
    float3 rayPos = origin + rayDirection * stepSize;
    
    float4 screenSpace = mul(mul(float4(1,1,1, 1), FB_ToView), FB_ToProjection);
    // Perform perspective division
    float3 ndcSpacePosition = screenSpace.xyz / screenSpace.w;

    // Map to screen space coordinates
    float2 screenSpacePosition;
    screenSpacePosition.x = (ndcSpacePosition.x + 1.0) * 0.5;
    screenSpacePosition.y = (1.0 - ndcSpacePosition.y) * 0.5;

    
    screenSpacePosition.x *= (FB_Resolution.x / FB_Resolution.y);
    // Convert to pixel coordinates
    float2 pixelCoordinates;
    pixelCoordinates.x = round(screenSpacePosition.x * FB_Resolution.x);
    pixelCoordinates.y = round(screenSpacePosition.y * FB_Resolution.y);
    
    return pixelCoordinates.xy;
    for (int i = 1; i < 100; i++)
    {
        rayPos = origin + (rayDirection * stepSize * i);
        float4 screenSpace = mul(mul(float4(rayPos.xyz, 1), FB_ToView), FB_ToProjection);
        float2 uv = screenSpace.xy / screenSpace.w;
        
        float3 comparePos = WorldPosTex.Sample(defaultSampler, uv);
        
        float distance = sqrt(pow(rayPos.x - comparePos.x, 2) + pow(rayPos.y - comparePos.y, 2) + pow(rayPos.z - comparePos.z, 2));
        if (distance <= 0.1f)
        {
            pixelCoordinates = uv;
            break;
        }
    }
    return pixelCoordinates;
}
//PostProcessPixelOutput main(PostProcessVertexToPixel input)
//{
    
//    //CORRECT NORMAL
//    PostProcessPixelOutput result;
//    float3 worldNormal = VertexNormalTex.Sample(defaultSampler, input.UV).rgb;
//    const float signedLength = (worldNormal.r + worldNormal.g + worldNormal.b) / 3;
//    if (signedLength < 0)
//    {
//        worldNormal = float3(1 - abs(worldNormal));
//    }
//    result.Color.rgb = worldNormal;
    
//    float4 worldPos = WorldPosTex.Sample(defaultSampler, input.UV);
//    //if(worldPos.x == 0 && worldPos.y == 0 && worldPos.z == 0)
//    //{
//    //    discard;
//    //}
//    result.Color = worldPos;
    
    
//    //float noise = RandNumber((worldPos.x * 100 * worldPos.y * 100 * worldPos.z) * /*frac(*/FB_TotalTime/*)*/);
    
//    //float4 stochasticNormals = float4(GetCosHemisphereSample(noise, noise, float3(worldNormal.xyz)), 1);
//    //return result;
    
    
//    float depth = depthTexture.Sample(defaultSampler, input.UV).r;
    
//    float4 screenPos = float4(input.UV, depth, 1);
   
//    //float3 viewPos = FB_CamPosition;
    
//    float3 dir = worldNormal;
    
//    //dir = mul(FB_ToProjection, dir);
    
//    //worldPos = normalize(mul(FB_ToView, worldPos));
//    //worldPos = normalize(mul(FB_ToProjection, worldPos));
    
    
//   // float2 uv = RayMarch(worldPos.xyz, worldNormal.rgb, depth);
    
//    //float2 uv = RayMarch(worldPos.xyz, dir, depth);
    
//    float4 viewPos = mul(float4(worldPos.xyz, 1), FB_ToView);
//    float4 projPos = mul(viewPos, FB_ToProjection);
    
//    float3 NDC = projPos.xyz / projPos.w;
    
//    float2 uv;
//    uv.x = Remap(-1, 1, 0, 1, NDC.x);
//    uv.y = Remap(-1, 1, 0, 1, NDC.y);

//    float4 sampledColor = screenTexture.Sample(defaultSampler, uv);
//    result.Color = sampledColor;
//    result.Color.a = 1.0f;
//    return result;
    
    
//    //if (uv.x != input.UV.x && uv.y != input.UV.y)
//    //{
//    //    result.Color.rgba = float4(1, 1, 1, 1);
//    //    return result;
//    //}
   
//    //result.Color.rgba = screenTexture.Sample(defaultSampler, uv.xy);
//    //return result;
    
    
//    ////float stepSize = (1.0f / )
//    ////result.Color = float4(rayMatch.xyz, 1);
    
//    //float4 normalCol = AlbedoTex.Sample(defaultSampler, worldNormal.xy);
    
//    ////result.Color = normalCol;
//    //return result;
//}
float3 ViewToWorld(float3 viewPos)
{
    float4 clipSpace = float4(viewPos.xy, 1.0, 1.0);
    float4 projection = mul(clipSpace, FB_ToProjection);
    float3 worldPos = projection.xyz / projection.w;
    return worldPos;
}

float3 CalculateSSGI(float2 texCoord, float3 normal, float3 viewDir)
{
    float3 rayDir = normalize(ViewToWorld(float3(texCoord, 1.0)) - FB_CamPosition.xyz);
    float3 hitPos = FB_CamPosition.xyz;
    float3 indirectIllumination = float3(0.0, 0.0, 0.0);
    float weightSum = 0.0;

    for (float t = 0.0; t < 10.0f; t += 0.1f)
    {
        hitPos += rayDir * 0.1f;
        float2 hitTexCoord = hitPos.xy / hitPos.z * 0.5 + 0.5;
        float3 albedo = screenTexture.Sample(defaultSampler, hitTexCoord.xy).xyz;
        float3 hitNormal = normalize(VertexNormalTex.Sample(defaultSampler, hitTexCoord.xy).xyz * 2.0 - 1.0);

        float3 indirectDir = normalize(hitPos - FB_CamPosition.xyz);
        float weight = dot(normal, indirectDir);
        weight = max(0.0, weight);
        weight = pow(weight, 1.0f);
        weightSum += weight;

        indirectIllumination += albedo * weight;
    }

    if (weightSum > 0.0)
    {
        indirectIllumination /= weightSum;
    }

    return indirectIllumination * 1.0f;
}

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
    PostProcessPixelOutput result;
    float4 albedo = screenTexture.Sample(defaultSampler, input.UV);
    float3 normal = VertexNormalTex.Sample(defaultSampler, input.UV).xyz * 2.0 - 1.0;
    float3 viewDir = normalize(FB_CamPosition.xyz - ViewToWorld(float3(input.UV, 1.0)));

    float3 ssgi = CalculateSSGI(input.UV, normal, viewDir);

    float4 finalColor = float4(albedo.xyz * (1.0 + ssgi), albedo.w);
    result.Color = finalColor;
    return result;
}