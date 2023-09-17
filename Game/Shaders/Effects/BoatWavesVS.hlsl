#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/Noise/Noise.hlsli"

cbuffer BoatParameters : register(b10)
{
    float Speed;
};

VertexToPixel main(VertexInput input)
{
    VertexToPixel result;

    float UVX = input.AlbedoUV.x;
    
    //float boatSpeed = 0.25f;
    //float boatSpeed = 0.55f;
    float boatSpeed = OB_CustomFloat;
   
    float boatSpeedNoise = boatSpeed * 3;
    
    float vertexColor = input.VxColor.r;
    
    float4 noise = float4(SimplexNoise(float2((input.Position.x * 5.025 + (FB_TotalTime * -boatSpeedNoise)), input.Position.z * 5.015 - (FB_TotalTime * -boatSpeedNoise))), 1, 1, 0) * 0.25f ;
    float4 boatWaveDisplacement = float4(1, sin(FB_TotalTime), 1, 1) * vertexColor;
    
    //+ noise.x
    float4x4 skinningMatrix =
    {
        1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
    };
    
    float4x4 waveMatrix =
    {
        60 * (input.VxColor.g * 0.15f) * (boatSpeed * 0.1f + 0.5f), 0, 0, 0,
		0, 25 * (boatSpeed), 0, 0,
		0, 0, 10 * (boatSpeed * 1.5f + 1.f), 0,
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

    float4 vertexWorldPosition = mul(OB_ToWorld, mul(input.Position, mul(skinningMatrix, waveMatrix)));

    if (OB_isInstanced)
    {
        vertexWorldPosition = mul(input.World, mul(input.Position, mul(skinningMatrix, waveMatrix)));
    }
    
    float waveForm = (sin((-input.VxColor.g * 2.5f)) + (input.VxColor.r * 4 + (3 * boatSpeed + 1)) * (1 - input.VxColor.g)) * boatSpeed;

    float4 displacedVertexWorldPosition = float4(vertexWorldPosition.x, vertexWorldPosition.y - vertexColor, vertexWorldPosition.z, vertexWorldPosition.w);
    
    const float4 vertexViewPosition = mul(FB_ToView, displacedVertexWorldPosition);
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