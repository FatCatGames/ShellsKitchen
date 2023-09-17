#include "../../../Catbox/Shaders/Particle/ParticleShaderStructs.hlsli"
#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"


Texture2D albedoTexture : register(t0);

ParticlePixelOutput main(ParticleGeometryToPixel input)
{
    ParticlePixelOutput result;
    float4 textureColor = albedoTexture.Sample(defaultSampler, input.UV);

    if (textureColor.a < 0.01f)
        discard;
	//result.id = OB_id;
    result.Color.rgba = textureColor.rgba * input.Color.rgba;
    //result.Color.rgba = 1;
    return result;
}