#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/Particle/ParticleShaderStructs.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"

[maxvertexcount(4)]
void main(
	point ParticleVertexData input[1],
	inout TriangleStream<ParticleGeometryToPixel> output
)
{
    const float2 offsets[4] =
    {
        { -1, 1 },
        { 1, 1 },
        { -1, -1 },
        { 1, -1 }
    };

    const float2 uvs[4] =
    {
        { 0.f, 0.f },
        { 1.f, 0.f },
        { 0.f, 1.f },
        { 1.f, 1.f }
    };
    
    const float theta = 1.5708; // 90 degrees in radians
    float angle = 90.0 * 3.14 / 180.0;
    
    float4x4 rotationMatrix =
    {
    1, 0, 0, 0,
    0, 0, 1, 0, 
    0, 1, 0, 0,
    0, 0, 0, 1
    };

 
    const ParticleVertexData inputParticle = input[0];
    for (unsigned int index = 0; index < 4; ++index)
    {
        ParticleGeometryToPixel result;
        
        float4 rotatedParticle = mul(inputParticle.Position, rotationMatrix);
        
        //float4x4 transposedMatrix = transpose(rotationMatrix);
        
        float4 trying = rotatedParticle;
        trying.xy += offsets[index] * inputParticle.Scale.xy;
        //trying = mul(OB_ToWorld, trying);
        trying = mul(rotationMatrix, trying);
       
        result.Position = mul(FB_ToView, trying);
        result.Position = mul(FB_ToProjection, result.Position);
        
        
        
        result.Color = inputParticle.Color;
        result.Lifetime = inputParticle.Lifetime;
        result.UV = RotateUV(uvs[index], inputParticle.Rotation);
        result.ParticleID = inputParticle.ParticleID;
        
        output.Append(result);
    }
}