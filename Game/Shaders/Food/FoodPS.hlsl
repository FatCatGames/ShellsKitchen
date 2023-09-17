
#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/PBRFunctions.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../Catbox/Shaders/Color/ColorConversion.hlsli"

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D pbrTexture : register(t2);
Texture2D ambientOcclusionTexture : register(t5);
Texture2D ScreenUVTexture : register(t6);
Texture2D shadowMap : register(t7);
Texture2D SSAOTexture : register(t8);
StructuredBuffer<LightData> lights : register(t9);
TextureCube environmentTexture : register(t10);

PixelOutput main(VertexToPixel input)
{
    PixelOutput result;

    float4 albedo = albedoTexture.Sample(defaultSampler, input.AlbedoUV).rgba;
    
    const float3 toEye = normalize(FB_CamPosition.xyz - input.WorldPosition.xyz);
    float dotProduct = dot(toEye, input.Normal);
 

    
    float3 albedoHSV = rgb2hsv(albedo.rgb);
    float3 albedoStrongerValue = float3(albedoHSV.x, albedoHSV.y, albedoHSV.z * 1.5f);
    float3 albedoStrongerValueRGB = hsv2rgb(albedoStrongerValue);
    

    albedo.rgb = lerp(albedoStrongerValueRGB, albedo.rgb, (dotProduct * MB_CustomFloat4.x));
    
    //float3 SSSemissive = lerp(0, albedo.rgb, sin(dotProduct));
    
    
    //albedo.rgb = lerp(albedo.rgb, albedoStrongerValueRGB, dotProduct*0.1f);
    
    albedo.rgb = GammaToLinear(albedo.rgb);
    albedo = albedo.rgba * MB_Color;
    if (albedo.a < 0.05f)
        discard;

	//tangent space matrix
    const float3x3 TBN = float3x3(
		normalize(input.Tangent),
		normalize(input.Binormal),
		normalize(input.Normal));

	//R = 0
	//G = normal Y
	//B = AO
	//A = normal X
    float3 normalMap = normalTexture.Sample(defaultSampler, input.AlbedoUV).agb;
    const float ao = normalMap.b;
	
    float3 pixelNormal = normalMap;
    pixelNormal = 2.0f * pixelNormal - 1;
    pixelNormal.z = sqrt(normalize(dot(pixelNormal.xy, pixelNormal.xy)));
    pixelNormal = normalize(pixelNormal);
    pixelNormal = normalize(mul(pixelNormal, TBN));

    float4 pbr = pbrTexture.Sample(defaultSampler, input.AlbedoUV);
    const float metalness = pbr.r;
    const float roughness = pbr.g;

    const float emissive = pbr.b * 0;
    const float emissiveStr = pbr.a * 0;
	//const float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.).rgb;

    const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

    float2 screenUV = ScreenUVTexture.Sample(defaultSampler, input.AlbedoUV).rg;
    const float ssaoValue = SSAOTexture.Sample(defaultSampler, screenUV).r;

    result.id = OB_id;

    if (FB_DebugMode != 0)
    {
        result.Color.a = 1;
		//albedo
        if (FB_DebugMode == 1)
        {
            result.Color = albedo;
        }
		//vx Color
        else if (FB_DebugMode == 2)
        {
            result.Color = input.VxColor * MB_Color;
        }
		//vertex normals
        else if (FB_DebugMode == 3)
        {
			//float3 debugNormal = pixelNormal;
            float3 debugNormal = input.Normal;
            const float signedLength = (debugNormal.r + debugNormal.g + debugNormal.b) / 3;
            if (signedLength < 0)
            {
                debugNormal = float3(1 - abs(debugNormal));
            }
            result.Color.rgb = debugNormal;
        }
		//normal map
        else if (FB_DebugMode == 4)
        {
            const float3 normalMap = normalTexture.Sample(defaultSampler, input.AlbedoUV).agb;
            result.Color = float4(normalMap.r, normalMap.g, 1, 1);
        }
		//UVs
        else if (FB_DebugMode == 5)
        {
            result.Color = float4(input.AlbedoUV.r, input.AlbedoUV.g, 1, 1);
        }
		//AO
        else if (FB_DebugMode == 6)
        {
            result.Color.rgb = ao;
        }
		//Roughness
        else if (FB_DebugMode == 7)
        {
            result.Color.rgb = roughness;
        }
		//Metallic
        else if (FB_DebugMode == 8)
        {
            result.Color.rgb = metalness;
        }
		//Emissive
        else if (FB_DebugMode == 9)
        {
            result.Color.rgb = emissive * emissiveStr;
        }
        return result;
    }

    float3 lightResult;
	//Calculate directional lights
    for (int i = 0; i < SLB_PointStartIndex; i++)
    {
        float3 lightAmount = EvaluateDirectionalLight(
			diffuseColor,
			specularColor,
			pixelNormal,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity,
			-lights[i].LB_Forward,
			toEye
		);

        if (lights[i].LB_CastShadows)
        {
            const float4x4 lightView = lights[i].LB_View;
            const float4x4 lightProj = lights[i].LB_Projection;
            const float4 worldToLightView = mul(lightView, input.WorldPosition);
            const float4 lightViewToProj = mul(lightProj, worldToLightView);

            float2 projectedTexCoord;
            projectedTexCoord.x = lightViewToProj.x / lightViewToProj.w / 2.0f + 0.5f;
            projectedTexCoord.y = -lightViewToProj.y / lightViewToProj.w / 2.0f + 0.5f;

            if (saturate(projectedTexCoord.x) == projectedTexCoord.x && saturate(projectedTexCoord.y) == projectedTexCoord.y)
            {
                const float shadowBias = 0.0005f;
                const float shadow = 0.0f;
                const float viewDepth = (lightViewToProj.z / lightViewToProj.w) - shadowBias;
                const float lightDepth = shadowMap.Sample(pointClampSampler, projectedTexCoord).r;

                if (lightDepth < viewDepth)
                {
                    lightAmount *= shadow;
                }
            }
        }

        lightResult += lightAmount * ssaoValue;
    }

	//Calculate point lights
    for (i = SLB_PointStartIndex; i < SLB_SpotStartIndex; i++)
    {
		//PBR TGA VER

        lightResult += EvaluatePointLight(
			diffuseColor,
			specularColor,
			pixelNormal,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity * 100,
			lights[i].LB_MaxRange,
			lights[i].LB_Pos,
			toEye,
			input.WorldPosition.xyz
		);
    }

	//Calculate spot lights
    for (int i = SLB_SpotStartIndex; i < SLB_IBLStartIndex; i++)
    {
        lightResult += EvaluateSpotLight(
			diffuseColor.rgb,
			specularColor.rgb,
			pixelNormal.rgb,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity,
			lights[i].LB_MaxRange,
			lights[i].LB_Pos.xyz,
			lights[i].LB_Forward.xyz,
			lights[i].LB_OuterRadius,
			lights[i].LB_InnerRadius,
			toEye.xyz,
			input.WorldPosition.xyz
		);
    }


	//Calculate IBL light
    for (i = SLB_IBLStartIndex; i < SLB_LightsCount; i++)
    {
        float3 ambientLighting = EvaluateAmbience(
			environmentTexture,
			pixelNormal.rgb,
			input.Normal.rgb,
			toEye.xyz,
			roughness,
			ao,
			diffuseColor.rgb,
			specularColor.rgb
		);
        ambientLighting *= lights[i].LB_Intensity * lights[i].LB_Color ;

        lightResult += ambientLighting * ssaoValue;
    }
   
    // albedoStrongerValueRGB
    
    float3 viewDirection = normalize(input.ProjectedPosition.xyz);
    float3 lightDirection = normalize(toEye.xyz);
    float3 rotlightDirection = float3(lightDirection.x, lightDirection.y, lightDirection.z);
   
    float normalMask = normalize(input.Normal.x + input.Normal.y + input.Normal.z / 3);
    
    float3 emissiveResult = albedo.rgb * emissive * emissiveStr * MB_EmissiveStrength;
    lightResult += emissiveResult;
    
    float3 scatterColor = albedo.rgb * max(0, dot(input.Normal, -lightDirection));
    
    float3 cubeMapTexture = EvaluateAmbience(
			environmentTexture,
			pixelNormal.rgb,
			normalMap,
			toEye.xyz,
			roughness,
			ao,
			diffuseColor.rgb,
			specularColor.rgb
		);
    
    
    
    //result.Color.rgb = lerp(0, MB_CustomFloat4.z, dot(input.Normal, -lightDirection));
    result.Color.rgb = lightResult;
    
    
    result.Color.a = albedo.a;
    result.WorldPosition = input.WorldPosition;
    result.LightResult = lightResult;

    return result;
}
