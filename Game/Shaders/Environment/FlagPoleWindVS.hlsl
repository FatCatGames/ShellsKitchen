#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/Noise/Noise.hlsli"

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
    float worldScaling = 0.05f;
    
    float3 worldScaleFlag = float3(worldDomain * worldScaling);
    float3 worldScalePole = float3(worldDomain * worldScaling);

    float flagNoise = SimplexNoise(float2((input.Position.x * worldScaleFlag.x) + FB_TotalTime, (input.Position.y * worldScaleFlag.y) + FB_TotalTime));
    float poleNoise = SimplexNoise(float2(worldScalePole.xy * windNoiseFrequency));
    
    float time = sin(FB_TotalTime * 1.f);
    float time2 = sin(FB_TotalTime * 3.6f);
    
    float timeVariety = (time * time2);
    
    float flagNoiseOverTime = (flagNoise + timeVariety);
    float poleNoiseOverTime = (poleNoise + time);
    
    float flagNoiseVxMultiply = (flagNoiseOverTime * (input.VxColor.g))*2;
    float poleNoiseVxMultiply = (poleNoiseOverTime * input.VxColor.r);
    
    float windX = flagNoiseVxMultiply;
    float windY = 1;
    float windZ = flagNoiseVxMultiply;
    
    float poleRotate = cos(timeVariety * input.VxColor.r + input.VxColor.g) * 0.025f;
    float poleRotateSin = sin(timeVariety * input.VxColor.r + input.VxColor.g) * 0.025f;
    
    float4 wind = float4(windX, windY, windZ , 1);
    
    
    
    
    /////////////////////////////////// Matrix-multiplying the noise to the skinning-matrix ////////////////////////////////////////////////////////////////////////////////////////////
    /*
        cosAngle, -sinAngle, 0,
        sinAngle, cosAngle, 0,
        0, 0, 1
    */
    
    float4x4 windEffect =
    {
        poleRotate + 1 , -poleRotateSin, 0, 0,
		poleRotateSin, poleRotate + 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
    };
    
    ////////////////////////////////////            Multiplying the Matricies              ////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    float4 vertexDisplacementLocalPosition = float4((input.Position.x - poleRotate), input.Position.y, input.Position.z - (wind.z), input.Position.w);
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