#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel result;
    
    float fracTime = frac(FB_TotalTime * 0.2f);

    float4x4 skinningMatrix =
    {
        1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1 , 0,
		0, 0, 0, 1
    };
    
    //Cloud one -1.362114429473877f min
    if (!OB_isStatic)
    {
        skinningMatrix = 0;
        skinningMatrix += input.BoneWeights.x * SB_BoneData[input.BoneIDs.x];
        skinningMatrix += input.BoneWeights.y * SB_BoneData[input.BoneIDs.y];
        skinningMatrix += input.BoneWeights.z * SB_BoneData[input.BoneIDs.z];
        skinningMatrix += input.BoneWeights.w * SB_BoneData[input.BoneIDs.w];
    }
    //float2 worldXY = float2(OB_ToWorldx, OB_ToWorld[1,0]);
    float coolTime = FB_TotalTime * 2;
    float angX = sin(coolTime*0.001f);
    float angY = cos(coolTime*0.001f);
    
    //float4 stupid = mul(OB_ToWorld, input.Position);
    float slowTime = FB_TotalTime * 0.02f;
    float time = SimplexNoise(float2(OB_CustomFloat + slowTime, OB_CustomFloat + slowTime));
    //float4(input.VxColor.r, input.VxColor.g, input.VxColor.b, 0);
    //float normX = -Remap(0.f, 1.f, -1.362114429473877f, 1.0071916580200195f, input.VxColor.r);
    //float normY = Remap(0.f, 1.f, -1.362114429473877f, 1.0071916580200195f, input.VxColor.g);
    //float normZ = Remap(0.f, 1.f, -1.362114429473877f, 1.0071916580200195f, input.VxColor.b);
    
    //float normX = -Remap(0.f, 1.f, -1.362114429473877f, 1.0071916580200195f, input.VxColor.r);
    //float normY =  Remap(0.f, 1.f,  -1.362114429473877f, 1.0071916580200195f, input.VxColor.g);
    //float normZ =  Remap(0.f, 1.f,  -1.362114429473877f, 1.0071916580200195f, input.VxColor.b);
    
    

    float normX = -Remap(0.f, 1.f,   -MB_CustomValue1, MB_CustomValue2, input.VxColor.r);
    float normY =  Remap(0.f, 1.f,   -MB_CustomValue1, MB_CustomValue2, input.VxColor.g);
    float normZ =  Remap(0.f, 1.f,   -MB_CustomValue1, MB_CustomValue2, input.VxColor.b);
    
    
    float4 vxPos = float4(normX, normY, normZ, 1);
    
    float4 vertexWorldPosition = mul(OB_ToWorld, lerp(input.Position, vxPos, pow(sin(slowTime), 2)));
    
    if (OB_isInstanced)
    {
        vertexWorldPosition = mul(input.World, input.Position);
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