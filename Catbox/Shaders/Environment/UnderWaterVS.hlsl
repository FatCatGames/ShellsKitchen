#include "../Struct/ShaderStructs.hlsli"
#include "../ShaderFunctions.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel result;

    float4 noiseX = float4(SimplexNoise(float2((input.Position.x * 0.15 + (FB_TotalTime * 0.09)), input.Position.z * 0.15 - (FB_TotalTime * 0.19))), 1, 1, 0);
    float4 noiseY = float4(SimplexNoise(float2((input.Position.x * 0.25 - (FB_TotalTime * 0.1)), input.Position.z * 0.25 + (FB_TotalTime * 0.16))), 1, 1, 0);
    float4 noiseZ = float4(1, 1, SimplexNoise(float2((input.Position.z * 0.15 - (FB_TotalTime * 0.1)), input.Position.x * 0.15 - (FB_TotalTime * 0.16))), 0);
    float4 noiseW = float4(1, 1, SimplexNoise(float2((input.Position.z * 0.25 + (FB_TotalTime * 0.1)), input.Position.x * 0.25 + (FB_TotalTime * 0.16))), 0);
    
    float4 noise2 = lerp(noiseX, noiseY, 0.5);
    float4 noise3 = lerp(noiseZ, noiseW, 0.5);
    
    float4 noise = lerp(noise2, noise3, 0.5) * 0.5f;
    float4 remappedNoise;
    remappedNoise.x = Remap(0, 1, -1, 1, noise.x);
    remappedNoise.y = Remap(0, 1, -1, 1, noise.y);
    remappedNoise.z = Remap(0, 1, -1, 1, noise.z);
    
    float4x4 skinningMatrix =
    {
        1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
    };

    if (!OB_isStatic)
    {
        skinningMatrix = 0;
        skinningMatrix += input.BoneWeights.x * SB_BoneData[input.BoneIDs.x];
        skinningMatrix += input.BoneWeights.y * SB_BoneData[input.BoneIDs.y];
        skinningMatrix += input.BoneWeights.z * SB_BoneData[input.BoneIDs.z];
        skinningMatrix += input.BoneWeights.w * SB_BoneData[input.BoneIDs.w];
    }

    
    
    float4 vertexWorldPosition = mul(OB_ToWorld, mul(input.Position, skinningMatrix));
    
    if (vertexWorldPosition.y < -5)
        vertexWorldPosition = mul(OB_ToWorld, mul(input.Position, skinningMatrix)) + float4((remappedNoise.x+0.5f), 0, remappedNoise.z, 0);
    else
        vertexWorldPosition = mul(OB_ToWorld, mul(input.Position, skinningMatrix));
    
	
    //float4 vertexWorldPosition = mul(OB_ToWorld, mul(input.Position, skinningMatrix)) + (float4(noise.xyz, 0));

    if (OB_isInstanced)
    {
        vertexWorldPosition = mul(input.World, mul(input.Position, skinningMatrix));
    }

    const float4 vertexViewPosition = mul(FB_ToView, vertexWorldPosition);
    const float4 vertexProjectionPosition = mul(FB_ToProjection, vertexViewPosition);

    const float3x3 worldNormalRotation = (float3x3) OB_ToWorld;
    const float3x3 skinNormalRotation = (float3x3) skinningMatrix;
	
    result.ProjectedPosition = vertexProjectionPosition;
    result.WorldPosition = vertexWorldPosition;

    result.VxColor = input.VxColor;
    result.NormalTex = input.NormalTex;
    result.Roughness = input.Roughness;
    result.AO = input.AO;
    result.AlbedoUV = input.AlbedoUV;
    result.NormalUV = input.NormalUV;
    result.RoughnessUV = input.RoughnessUV;
    result.AOUV = input.AOUV;
	
    result.Tangent = mul(worldNormalRotation, mul(input.Tangent, skinNormalRotation));
    result.Binormal = mul(worldNormalRotation, mul(input.Binormal, skinNormalRotation));
    result.Normal = normalize(mul(worldNormalRotation, mul(input.Normal, skinNormalRotation)));

    return result;
}