#include "../Struct/ShaderStructs.hlsli"
#include "ParticleShaderStructs.hlsli"
#include "../ShaderFunctions.hlsli"

[maxvertexcount(4)]

void main(
point ParticleVertexData input[1],
	inout TriangleStream<ParticleGeometryToPixel> output
)

{
    const float2 offsets[4] =
    {
        { -1, 1  },
        {  1, 1  },
        { -1, -1 },
        {  1, -1 }
    };

    const float2 uvs[4] =
    {
        { 0.f, 0.f },
        { 1.f, 0.f },
        { 0.f, 1.f },
        { 1.f, 1.f }
    };

    float4x4 bringForth =
    {
        1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1.0005f
    };
    
    const ParticleVertexData inputParticle = input[0];
    for (unsigned int index = 0; index < 4; ++index)
    {
        ParticleGeometryToPixel result;
        result.Position = mul(FB_ToView, inputParticle.Position);
        result.Position.xy += offsets[index] * inputParticle.Scale.xy;
        result.Position = mul(FB_ToProjection, result.Position);
        result.Position = mul(bringForth, result.Position);
        result.Color = inputParticle.Color;
       
        result.Lifetime = inputParticle.Lifetime;
        
        result.UV = RotateUV(uvs[index], inputParticle.Rotation );
        if(inputParticle.Position.y == -4.5)
        {
            result.UV.y = result.UV.y * 0.75f - 0.2f;
            result.UV.x = result.UV.x;
            result.Color = result.Color;
            
        }
        
        result.ParticleID = inputParticle.ParticleID;
        output.Append(result);
    }
}