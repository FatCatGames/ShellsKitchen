#include "../Struct/ShaderStructs.hlsli"
#include "../Noise/Noise.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel result;

    float3 waveDomain = float3(OB_ToWorld._m30, OB_ToWorld._m31, OB_ToWorld._m32);
    
    //float time = FB_TotalTime*FB_DeltaTime;
    float4(1, 1, 1, 1);
    float4 noiseX = float4(SimplexNoise(float2((input.Position.x * 0.15 + (FB_TotalTime *  0.09)), input.Position.z * 0.15 - (FB_TotalTime * 0.19))), 1, 1, 0);
    float4 noiseY = float4(SimplexNoise(float2((input.Position.x * 0.25 - (FB_TotalTime * 0.1)), input.Position.z * 0.25 + (FB_TotalTime * 0.16))), 1, 1, 0);
    float4 noiseZ = float4(1,1,  SimplexNoise(float2((input.Position.z * 0.15 - (FB_TotalTime * 0.1)), input.Position.x * 0.15 - (FB_TotalTime * 0.16))), 0);
    float4 noiseW = float4(1,1, SimplexNoise(float2((input.Position.z * 0.25 + (FB_TotalTime * 0.1)), input.Position.x * 0.25 + (FB_TotalTime * 0.16))), 0);
    
    float4 noise2 = lerp(noiseX, noiseY, 0.5);
    float4 noise3 = lerp(noiseZ, noiseW, 0.5);
    
    float4 noise = lerp(noise2, noise3, 0.5) * 0.5f;
    
    float amplitude = 0.05f;
    
    float waveYdisplace = (noise.y * waveDomain.y);
    
    float4x4 skinningMatrix =
    {
        1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
    };
    
    float4x4 waveMatrix =
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

    
    float4 vertexWorldPosition = mul(OB_ToWorld, mul(input.Position, skinningMatrix)) + (float4(noise.x*3, 0.5, noise.z*3, 0) );

    if (OB_isInstanced)
    {
        vertexWorldPosition = mul(input.World, mul(input.Position, mul(skinningMatrix, 0)));
    }

    const float4 vertexViewPosition = mul(FB_ToView, vertexWorldPosition) ;
    const float4 vertexProjectionPosition = mul(FB_ToProjection, vertexViewPosition);

    const float3x3 worldNormalRotation = (float3x3) OB_ToWorld;
    const float3x3 skinNormalRotation = (float3x3) skinningMatrix;
    
    result.ProjectedPosition = vertexProjectionPosition;
    result.WorldPosition = vertexWorldPosition ;

    result.VxColor = input.VxColor * noise.xyzw;
    result.NormalTex = input.NormalTex * noise.xyz;
    result.Roughness = input.Roughness * noise.xyz;
    result.AO = input.AO * noise.xyz;
    result.AlbedoUV = input.AlbedoUV;
    result.NormalUV = input.NormalUV;
    result.RoughnessUV = input.RoughnessUV;
    result.AOUV = input.AOUV * noise.xz;

    result.Tangent = mul(worldNormalRotation, mul(input.Tangent, skinNormalRotation)) ;
    result.Binormal = mul(worldNormalRotation, mul(input.Binormal, skinNormalRotation)) ;
    result.Normal = normalize(mul(worldNormalRotation, mul(input.Normal, skinNormalRotation))) ;

    return result;
}