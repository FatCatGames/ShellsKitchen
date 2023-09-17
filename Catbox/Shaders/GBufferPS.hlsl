#include "Struct/ShaderStructs.hlsli"
#include "PBRFunctions.hlsli"
#include "ShaderFunctions.hlsli"


GBufferOutput main(VertexToPixel input)
{
    GBufferOutput result;
    float4 albedo;
    float4 normalMapSample;
    float2 normalMap;
    float ambientOcclusion;
    float4 material;    
    float3 pixelNormal;
        
    if (OB_isTiling)
    {       
        float2 scale = float2(MB_CustomFloat4.x, MB_CustomFloat4.y) + 1;
        float2 offset = float2(MB_CustomFloat4.z, MB_CustomFloat4.w);
        float2 uv = ScaleUV(input.AlbedoUV, scale);
        uv = OffsetUV(uv, offset);
        
        albedo = AlbedoTex.Sample(wrapSampler, uv).rgba;
        if (albedo.a <= 0.05f) discard;
        albedo.rgb = GammaToLinear(albedo.rgb);
        normalMapSample = NormalTex.Sample(wrapSampler, uv);
        normalMap = normalMapSample.ag;
        ambientOcclusion = normalMapSample.b;
        material = MaterialTex.Sample(wrapSampler, uv);
    
        pixelNormal = normalMap.xyy;
        pixelNormal = 2.0f * pixelNormal - 1;
        pixelNormal.z = sqrt(1 - saturate(pixelNormal.x + pixelNormal.x + pixelNormal.y + pixelNormal.y));
        pixelNormal = normalize(pixelNormal);
        
        float3x3 TBN = float3x3
        (
            normalize(input.Tangent),
            normalize(-1.0f * input.Binormal),
            normalize(input.Normal)
        );
        pixelNormal = normalize(mul(pixelNormal, TBN));
    }
    
    else if (OB_isTriPlanar)
    {
        float3 WorldSpaceUV = (input.WorldPosition.rgb * MB_CustomValue1);
        float3 VNWS = float3(-1, 1, 1) * sign(input.Normal);
        float3 AWP = float3(1, 1, -1) * WorldSpaceUV;
        float2 samplingX = float2(VNWS.r, 1) * AWP.gb;
        float2 samplingY = float2(VNWS.g, 1) * AWP.rb;
        float2 samplingZ = float2(VNWS.b, 1) * AWP.rg;
        
    //Sampling stuff for triplanar shait   	
        float4 albedo_GB = AlbedoTex.Sample(wrapSampler, samplingX);
        float4 albedo_RB = AlbedoTex.Sample(wrapSampler, samplingY);
        float4 albedo_RG = AlbedoTex.Sample(wrapSampler, samplingZ);
        
        albedo_GB.rgb = GammaToLinear(albedo_GB.rgb);
        albedo_RB.rgb = GammaToLinear(albedo_RB.rgb);
        albedo_RG.rgb = GammaToLinear(albedo_RG.rgb);
        
        
        float4 material_GB = MaterialTex.Sample(wrapSampler, samplingX);
        float4 material_RB = MaterialTex.Sample(wrapSampler, samplingY);
        float4 material_RG = MaterialTex.Sample(wrapSampler, samplingZ);
     
        float ambientOcclusion_GB = NormalTex.Sample(wrapSampler, samplingX).b;
        float ambientOcclusion_RB = NormalTex.Sample(wrapSampler, samplingY).b;
        float ambientOcclusion_RG = NormalTex.Sample(wrapSampler, samplingZ).b;

    //Sampling normals for triplanar shait    

        float3 VsNormalWS = input.Normal;
        
        float3 normalMap_X = deriveNormalZ(NormalTex.Sample(wrapSampler, samplingX).ag);
        float _R = normalMap_X.b * VsNormalWS.x;
        float _G = (normalMap_X.r * VNWS.r) + VsNormalWS.y;
        float _B = (normalMap_X.g * -1) + VsNormalWS.z;
        normalMap_X = float3(_R, _G, _B);
        
        float3 normalMap_Y = deriveNormalZ(NormalTex.Sample(wrapSampler, samplingY).ag);
        _R = (normalMap_Y.r * VNWS.g) + VsNormalWS.x;
        _G = normalMap_Y.b * VsNormalWS.y;
        _B = (normalMap_Y.g * -1) + VsNormalWS.z;
        normalMap_Y = float3(_R, _G, _B);
        
        float3 normalMap_Z = deriveNormalZ(NormalTex.Sample(wrapSampler, samplingZ).ag);
        _R = (normalMap_Z.r * VNWS.b) + VsNormalWS.x;
        _G = normalMap_Z.g + VsNormalWS.y;
        _B = normalMap_Z.b * VsNormalWS.z;
        normalMap_Z = float3(_R, _G, _B);
        
	    
    //Calculating lerp value for triplanar shait    
        float3 someValue = pow(abs(input.Normal), MB_CustomValue2);
        float theDot = dot(someValue, float3(1, 1, 1));
        float3 lerpValue = someValue / theDot;
	
        albedo = lerp(albedo_GB, albedo_RB, lerpValue.g);
        albedo = lerp(albedo, albedo_RG, lerpValue.b);

        material = lerp(material_GB, material_RB, lerpValue.g);
        material = lerp(material, material_RG, lerpValue.b);
         
        ambientOcclusion = lerp(ambientOcclusion_GB, ambientOcclusion_RB, lerpValue.g);
        ambientOcclusion = lerp(ambientOcclusion, ambientOcclusion_RG, lerpValue.b);
             
        pixelNormal = lerp(normalMap_X, normalMap_Y, lerpValue.g);
        pixelNormal = lerp(pixelNormal, normalMap_Z, lerpValue.b);

    }
    else //Normal shading
    {
        albedo = AlbedoTex.Sample(defaultSampler, input.AlbedoUV).rgba;
        if (albedo.a <= 0.05f)
            discard;
        albedo.rgb = GammaToLinear(albedo.rgb);
        normalMapSample = NormalTex.Sample(defaultSampler, input.AlbedoUV);
        normalMap = normalMapSample.ag;
        ambientOcclusion = normalMapSample.b;
        material = MaterialTex.Sample(defaultSampler, input.AlbedoUV);
    
        pixelNormal = normalMap.xyy;
        pixelNormal = 2.0f * pixelNormal - 1;
        pixelNormal.z = sqrt(1 - saturate(pixelNormal.x + pixelNormal.x + pixelNormal.y + pixelNormal.y));
        pixelNormal = normalize(pixelNormal);
        
        float3x3 TBN = float3x3
        (
            normalize(input.Tangent),
            normalize(-1.0f * input.Binormal),
            normalize(input.Normal)
        );
        pixelNormal = normalize(mul(pixelNormal, TBN));
    }
    
    
    const float emissive = material.b;
    const float emissiveStr = material.a;

    result.Albedo = albedo * MB_Color;
    result.Normal = float4(pixelNormal, 1.0f);
    result.EmmissiveColor.rgb = albedo.rgb * emissive * emissiveStr * MB_EmissiveStrength;
    result.Material = material;
    result.VertexNormal = float4(normalize(input.Normal), 1.0f);
    result.WorldPosition = input.WorldPosition;
    result.AmbientOcclusion = ambientOcclusion;
    result.id = OB_id;
    float time = FB_TotalTime;
    float deltatime = FB_DeltaTime;

    return result;
}