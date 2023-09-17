#include "../Struct/ShaderStructs.hlsli"
#include "../PBRFunctions.hlsli"
#include "../ShaderFunctions.hlsli"

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
    
    float tiling1 = 0.055f;
    float tiling2 = 0.145f;
    
    float UVtiling = 1;
    
    float4 noiseX = float4(SimplexNoise(float2((input.WorldPosition.z * tiling1 + (FB_DeltaTime * 0.09)), input.WorldPosition.x * tiling1 - (FB_TotalTime * 0.19))),
                    SimplexNoise(float2((input.WorldPosition.x * tiling1 + (FB_DeltaTime * 0.09)), input.WorldPosition.z * tiling1 - (FB_TotalTime * 0.19))),
                    SimplexNoise(float2((input.WorldPosition.x * tiling1 + (FB_DeltaTime * 0.09)), input.WorldPosition.z * tiling1 - (FB_TotalTime * 0.19))), 0);
    
    float4 noiseY = float4(SimplexNoise(float2((input.WorldPosition.z * tiling1 + (FB_DeltaTime * 0.09)), input.WorldPosition.x * tiling1 - (FB_TotalTime * 0.19))),
                    SimplexNoise(float2((input.WorldPosition.x * tiling2 - (FB_TotalTime * 0.1)), input.WorldPosition.z * tiling2 + (FB_TotalTime * 0.16))),
                    SimplexNoise(float2((input.WorldPosition.x * tiling1 + (FB_DeltaTime * 0.09)), input.WorldPosition.x * tiling1 - (FB_TotalTime * 0.19))), 0);
    
    float4 noiseZ = float4(SimplexNoise(float2((input.WorldPosition.z * tiling1 - (FB_TotalTime * 0.1)), input.WorldPosition.x * tiling1 - (FB_TotalTime * 0.16))),
                    SimplexNoise(float2((input.WorldPosition.z * tiling1 - (FB_TotalTime * 0.1)), input.WorldPosition.x * tiling1 - (FB_TotalTime * 0.16))),
                    SimplexNoise(float2((input.WorldPosition.z * tiling1 - (FB_TotalTime * 0.1)), input.WorldPosition.x * tiling1 - (FB_TotalTime * 0.16))), 0);
    
    float4 noiseW = float4(SimplexNoise(float2((input.WorldPosition.z * tiling2 + (FB_TotalTime * 0.1)), input.WorldPosition.x * tiling2 + (FB_TotalTime * 0.16))),
                    SimplexNoise(float2((input.WorldPosition.z * tiling2 + (FB_TotalTime * 0.1)), input.WorldPosition.x * tiling2 + (FB_TotalTime * 0.16))),
                    SimplexNoise(float2((input.WorldPosition.x * tiling2 + (FB_TotalTime * 0.1)), input.WorldPosition.z * tiling2 + (FB_TotalTime * 0.16))), 0);
    
    float4 noise2 = abs(noiseX * noiseY);
    float4 noise3 = abs(noiseZ * noiseW);
    
    //float4 noise2 = lerp(noiseX, noiseY, 0.5);
    //float4 noise3 = lerp(noiseZ, noiseW, 0.5);
    
    float2 panningUV = float2((input.WorldPosition.x * UVtiling) + FB_TotalTime * 0.01f, (input.WorldPosition.y * UVtiling) + FB_TotalTime * 0.01f);
    float2 panningUV2 = float2((input.WorldPosition.x * UVtiling) - FB_TotalTime * 0.01f, (input.WorldPosition.y * UVtiling) + FB_TotalTime * 0.01f);
    float2 panningUV3 = float2((input.WorldPosition.x * UVtiling) - FB_TotalTime * 0.01f, (input.WorldPosition.y * UVtiling) - FB_TotalTime * 0.01f);
    
    //float4 noise = lerp(noise2, noise3, 0.5) * 0.5f;
    float4 noise = (noise2 + noise3) / 5;

    float4 albedo = albedoTexture.Sample(defaultSampler, input.AlbedoUV).rgba;
    float4 albedo2 = albedoTexture.Sample(defaultSampler, input.AlbedoUV).rgba;
    float4 albedo3 = albedoTexture.Sample(defaultSampler, input.AlbedoUV).rgba;
  
    albedo.rgb = GammaToLinear(albedo.rgb);
    albedo = albedo.rgba * MB_Color;
    if (albedo.a < 0.05f)
        discard;

    float3 causticsNoise = CowSticks(input.WorldPosition.xyz * 0.0008f, FB_TotalTime * 1.5f, input.Normal.xyz);
    //float3 causticsNoise = pow(abs(lerp(1, 0.99f, noise.rgb)), 2000);
    //float3 causticsNoise = lerp(1.f, 20, noise.r);
    
    float3 causticsNoiseLerp = lerp(0, 0.75f, pow(causticsNoise, 200));
    
    //float3 remapCausticNoise = Remap(0, 1, 0.1f, 0.9f, causticsNoise.r);
  
    //if (input.WorldPosition.y < (noise.y * sin(0.2f * FB_TotalTime)+1.5f) + sin(0.1f * (FB_TotalTime * 5)) - 1.5f && input.WorldPosition.y > 0)
    if ( input.WorldPosition.y < (noise.y * sin(FB_TotalTime) + 0.2f) * sin(0.2f * (FB_TotalTime * 5)) + 0.75f && input.WorldPosition.y > 0)
        albedo.rgb = 1;
    else if (input.WorldPosition.y < 0)
        albedo.rgb = albedo.rgb + abs(causticsNoise) * float3(0.9f, 0.9f, 1) * 0.155f;
    
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

    const float emissive = pbr.b;
    const float emissiveStr = pbr.a;
	//const float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.).rgb;

    const float3 toEye = normalize(FB_CamPosition.xyz - input.WorldPosition.xyz);
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
            result.Color.rgb = (1, 1, 1, 1);
			//result.Color = albedo;
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

    float shadedArea = 1;

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
			
            const float shadowBias = 0.0001f;
            float shadow = 0.0f;
            const float viewDepth = (lightViewToProj.z / lightViewToProj.w) - shadowBias;
				
            float4 coords;
            coords.xy = projectedTexCoord;
            coords.z = viewDepth - shadowBias;
            coords.w = 1.0f;
				
            shadow = pcfShadow(coords, shadowMap, 2048.0f * 2, shadowSampler);
            shadedArea = albedo.rgb - abs(causticsNoise) * float3(0.9f, 0.9f, 1) * 0.155f;
				
            lightAmount *= shadow;
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
        ambientLighting *= lights[i].LB_Intensity * lights[i].LB_Color * lights[i].LB_Color;


        lightResult += ambientLighting * ssaoValue;
    }

  
    //result.Color.rgb = lightResult;
    

    
    float3 emissiveResult = albedo.rgb * emissive * emissiveStr * MB_EmissiveStrength;
    lightResult += emissiveResult;
    
    result.Color.rgb = lightResult * shadedArea;
    result.Color.a = 1;
    result.WorldPosition = input.WorldPosition;
    result.LightResult = lightResult;

    result.id = OB_id;
    
    return result;
}