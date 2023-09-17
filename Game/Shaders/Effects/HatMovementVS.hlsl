#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"




VertexToPixel main(VertexInput input)
{
    VertexToPixel result;

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

    float3 worldDomain = float3(OB_ToWorld._m11, OB_ToWorld._m22, OB_ToWorld._m33);
    
    ////////////////////////////////// Noise of two different scales are blended and animated per direction ////////////////////////////////////////////////////////////////////////////
    

    float windStrength = 1;
    float windNoiseFrequency = 1;
    float worldScaling = 0.2f;
    
    float hatStrength = min(OB_CustomFloat * 0.2f, 1); //Normalizing from 5 movespeed to 1
    
    float bounceHatStrength = easeInElastic(hatStrength);
    
    float hatStrenghtHeight = bounceHatStrength * (input.Position.y) ;

    
    
    float3 worldScaleLeaves = float3(worldDomain * worldScaling);
    float3 worldScaleTrunk = float3(worldDomain * worldScaling);
    
    float4 posInWorldSpace = mul(OB_ToWorld, input.Position) * 0.5f;
    
    float leavesNoise = SimplexNoise(float2((posInWorldSpace.x * worldScaleLeaves.x) + FB_TotalTime, (posInWorldSpace.z * worldScaleLeaves.z) + FB_TotalTime));
    float trunkNoise = SimplexNoise(float2(posInWorldSpace.xy * windNoiseFrequency));
    
    float time = sin(FB_TotalTime * 1.f);
    float time2 = sin(FB_TotalTime * 3.6f);
    
    float timeVariety = (time * time2);
    
    float leavesNoiseOverTime = (leavesNoise + timeVariety) * OB_CustomFloat;
    float trunkNoiseOverTime = (trunkNoise + time);
    
    float leavesNoiseVxMultiply = (leavesNoiseOverTime * 1);
    float trunkNoiseVxMultiply = (trunkNoiseOverTime * input.VxColor.r);
    
    float windX = leavesNoiseVxMultiply * 0.25f * (OB_CustomFloat * 10000);
    float windY = leavesNoiseVxMultiply * 0.25f;
    float windZ = 0;
    
    
    
    
        float combinedVxColor = ((1 - input.VxColor.r) + input.VxColor.g);
    
    float poleRotate = (bounceHatStrength) * 0.5f;
    
    float4 wind = float4(windX, windY, windZ, 1);
    
    
    
    /////////////////////////////////// Matrix-multiplying the noise to the skinning-matrix ////////////////////////////////////////////////////////////////////////////////////////////
    
    float4x4 windEffect =
    {
        1, 0, 0, 0,
		0, 1, 0, 0,
		0, poleRotate, 1, 0,
		0, 0, 0, 1
    };
    
    ////////////////////////////////////            Multiplying the Matricies              ////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    float4 vertexDisplacementLocalPosition = float4(input.Position.x, input.Position.y, (input.Position.z - hatStrenghtHeight), input.
    Position.w);
    float4 vertexWorldPosition = mul(OB_ToWorld, mul(vertexDisplacementLocalPosition, windEffect));
    
    if (OB_isInstanced)
    {
        vertexWorldPosition = mul(input.World, mul(input.Position, windEffect));
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