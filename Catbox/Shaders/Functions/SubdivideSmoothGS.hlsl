#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/Noise/Noise.hlsli"

float3 CalculateTangent(SferGeometryInput v0, SferGeometryInput v1, SferGeometryInput v2)
{
    // Calculate the edges of the triangle
    float3 edge1 = v1.Position - v0.Position;
    float3 edge2 = v2.Position - v0.Position;

    // Calculate the UV deltas
    float2 deltaUV1 = v1.AlbedoUV - v0.AlbedoUV;
    float2 deltaUV2 = v2.AlbedoUV - v0.AlbedoUV;

    // Calculate the coefficient matrix
    float det = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
    float3x3 coefMatrix = float3x3(
        deltaUV2.y * edge1 - deltaUV1.y * edge2,
        deltaUV1.x * edge2 - deltaUV2.x * edge1,
        float3(det, -det, -det)
    );

    // Calculate the tangent and return it
    float3 tangent = normalize(mul(coefMatrix, v0.Normal));
    return tangent;
}


[maxvertexcount(3*4)]
void main(triangle SferGeometryInput i[3], inout TriangleStream<SferGeometryInput> Output)
{
     SferGeometryInput V1 = (SferGeometryInput) 0;
    ////////////////////////////////// Vertices overview /////////////////////////////////////
    
    /*
    i = Original Input Verticies
    V = New Verticies
    
            i1  
           /  \
          /    \
         V1 --- V2
        / \    / \
       /   \  /   \
     i0  -- V3 -- i2
    
    */
    
    float3 V1pos = (i[0].Position.xyz + i[1].Position.xyz) / 2;
    float3 V2pos = (i[1].Position.xyz + i[2].Position.xyz) / 2;
    float3 V3pos = (i[0].Position.xyz + i[2].Position.xyz) / 2;
    
    ////////////////////////////////////////// V1 ////////////////////////////////////////////
    
   

    /*
            i1  
           /  \
          /    \
       [ V1 ] - V2
        / \    / \
       /   \  /   \
     i0  -- V3 -- i2
    
    */
    
    V1.Normal = abs(normalize(cross(V2pos - V1pos, V3pos - V1pos)));
    V1.Tangent = CalculateTangent(i[0], i[1], i[2]);
    V1.Binormal = cross(V1.Tangent, V1.Normal);
    
    V1.WorldPosition = (i[0].WorldPosition + i[1].WorldPosition) / 2;
    V1.Position = (i[0].Position + i[1].Position) / 2;
    
    V1.NormalTex = lerp(i[0].NormalTex, i[1].NormalTex, 0.5);
    V1.Roughness = lerp(i[0].Roughness, i[1].Roughness, 0.5);
    V1.AO = lerp(i[0].AO, i[1].AO, 0.5);
    
    V1.UV = lerp(i[0].UV, i[1].UV, 0.5);
    V1.AlbedoUV = lerp(i[0].AlbedoUV, i[1].AlbedoUV, 0.5);
    V1.RoughnessUV = lerp(i[0].RoughnessUV, i[1].RoughnessUV, 0.5);
    V1.AOUV = lerp(i[0].AOUV, i[1].AOUV, 0.5);
 
    //////////////////////////////////////////////////////////////////////////////////
    
    SferGeometryInput V2 = (SferGeometryInput) 0;
    
    /*
            i1  
           /  \
          /    \
         V1 - [ V2 ]
        / \    / \
       /   \  /   \
     i0  -- V3 -- i2
    
    */
    
    V2.Normal = abs(normalize(cross(V3pos - V2pos, V1pos - V2pos)));
    V2.Tangent = CalculateTangent(i[1], i[2], i[0]);
    V2.Binormal = cross(V2.Tangent, V2.Normal);
    
    V2.WorldPosition = (i[1].WorldPosition + i[2].WorldPosition) / 2;
    V2.Position = (i[1].Position + i[2].Position) / 2;
 
    V2.NormalTex = lerp(i[1].NormalTex, i[2].NormalTex, 0.5);
    V2.Roughness = lerp(i[1].Roughness, i[2].Roughness, 0.5);
    V2.AO = lerp(i[1].AO, i[2].AO, 0.5);
    
    V2.UV = lerp(i[1].UV, i[2].UV, 0.5);
    V2.AlbedoUV = lerp(i[1].AlbedoUV, i[2].AlbedoUV, 0.5);
    V2.RoughnessUV = lerp(i[1].RoughnessUV, i[2].RoughnessUV, 0.5);
    V2.AOUV = lerp(i[1].AOUV, i[2].AOUV, 0.5);
    
    
    SferGeometryInput V3 = (SferGeometryInput) 0;
    
   /*
            i1  
           /  \
          /    \
         V1 -  V2
        / \    / \
       /   \  /   \
     i0 - [ V3 ] - i2
    
    */
    
    V3.Normal = abs(normalize(cross(V1pos - V3pos, V2pos - V3pos)));
    V3.Tangent = CalculateTangent(i[2], i[0], i[1]);
    V3.Binormal = cross( V3.Normal,V3.Tangent);

    V3.WorldPosition = (i[2].WorldPosition + i[0].WorldPosition) / 2;
    V3.Position = (i[2].Position + i[0].Position) / 2;
    
    V3.NormalTex = lerp(i[2].NormalTex, i[0].NormalTex, 0.5);
    V3.Roughness = lerp(i[2].Roughness, i[0].Roughness, 0.5);
    V3.AO = lerp(i[2].AO, i[0].AO, 0.5);
    
    V3.UV = lerp(i[2].UV, i[0].UV, 0.5);
    V3.AlbedoUV = lerp(i[2].AlbedoUV, i[0].AlbedoUV, 0.5);
    V3.RoughnessUV = lerp(i[2].RoughnessUV, i[0].RoughnessUV, 0.5);
    V3.AOUV = lerp(i[2].AOUV, i[0].AOUV, 0.5);
    
    V1.Normal = i[0].Normal;
    V2.Normal = i[1].Normal;
    V3.Normal = i[2].Normal;
    
    /*
            i1  
           /  \
          /    \
         V1 --- V2
        / \    / \
       / X \  /   \
      i0 -- V3 -- i2
    
    */
    
    Output.Append(i[0]);
    Output.Append(V1);
    Output.Append(V3);
    
    Output.RestartStrip();
    
    /*
            i1  
           /  \
          / XX \
         V1 --- V2
        / \    / \
       /   \  /   \
      i0 -- V3 -- i2
    
    */
    
    Output.Append(i[1]);
    Output.Append(V2);
    Output.Append(V1);

    Output.RestartStrip();
    
    
     /*
            i1  
           /  \
          /    \
         V1 --- V2
        / \    / \
       /   \  / X \
      i0 -- V3 -- i2
    
    */

    Output.Append(i[2]);
    Output.Append(V3);
    Output.Append(V2);

    Output.RestartStrip();
    

    /*
            i1  
           /  \
          /    \
         V1 --- V2
        / \ XX / \
       /   \  /   \
      i0 -- V3 -- i2
    
    */

    Output.Append(V1);
    Output.Append(V2);
    Output.Append(V3);
       
    Output.RestartStrip();
}

