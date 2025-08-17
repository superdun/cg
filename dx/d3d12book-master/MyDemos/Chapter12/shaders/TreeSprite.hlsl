//***************************************************************************************
// TreeSprite.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightingUtil.hlsl"

Texture2DArray gTreeMapArray : register(t0);


SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
	float4x4 gTexTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;

	float4 gFogColor;
	float gFogStart;
	float gFogRange;
	float2 cbPerObjectPad2;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
};

cbuffer cbMaterial : register(b2)
{
	float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
	float4x4 gMatTransform;
};

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float2 TexC    : TEXCOORD;
};
struct GeoOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC    : TEXCOORD;
    uint   PrimID  : SV_PrimitiveID;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);

	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, gMatTransform).xy;

    return vout;
}
float3 ProjectToSphere(float3 pos, float3 center, float radius)
{
    float3 dir = pos - center;
    dir = normalize(dir);
    return center + dir * radius;
}
void OutputVertex(float3 pos, uint primID, inout TriangleStream<GeoOut> triStream)
{
    GeoOut gout;
    float3 center = float3(0,0,0);
    gout.PosW = pos;
    gout.NormalW = normalize(pos - center);
    gout.PrimID = primID;
    gout.PosH = mul(float4(pos, 1.0f), gViewProj);
    gout.TexC = float2(0, 0); // or compute UV coords properly if needed
    triStream.Append(gout);
}
void SubdivideTriangleToSphere(
    float3 p0,
    float3 p1,
    float3 p2,
    uint primID,
    inout TriangleStream<GeoOut> triStream,
    int depth,
    int endDepth
    )
{
    float3 center = float3(0,0,0);
    float radius = 8.0f;
    float3 v0 = ProjectToSphere(p0, center, radius);
    float3 v1 = ProjectToSphere(p1, center, radius);
    float3 v2 = ProjectToSphere(p2, center, radius);
    float3 m0 = ProjectToSphere((v0 + v1) * 0.5f, center, radius);
    float3 m1 = ProjectToSphere((v1 + v2) * 0.5f, center, radius);
    float3 m2 = ProjectToSphere((v2 + v0) * 0.5f, center, radius);
    if(depth==endDepth)
    {
        OutputVertex(v0, primID, triStream);
        OutputVertex(m0, primID, triStream);
        OutputVertex(m2, primID, triStream);
        triStream.RestartStrip();
        OutputVertex(m0, primID, triStream);
        OutputVertex(v1, primID, triStream);
        OutputVertex(m1, primID, triStream);
        triStream.RestartStrip();
        OutputVertex(m2, primID, triStream);
        OutputVertex(m1, primID, triStream);
        OutputVertex(v2, primID, triStream);
        triStream.RestartStrip();
        OutputVertex(m0, primID, triStream);
        OutputVertex(m1, primID, triStream);
        OutputVertex(m2, primID, triStream);
        triStream.RestartStrip();
        return;
    }
    else{
                // 还未达到细分深度，继续递归
        SubdivideTriangleToSphere(v0, m0, m2, primID, triStream, depth + 1, endDepth);
        SubdivideTriangleToSphere(m0, v1, m1, primID, triStream, depth + 1, endDepth);
        SubdivideTriangleToSphere(m2, m1, v2, primID, triStream, depth + 1, endDepth);
        SubdivideTriangleToSphere(m0, m1, m2, primID, triStream, depth + 1, endDepth);
    }


}
 // We expand each point into a quad (4 vertices), so the maximum number of vertices
 // we output per geometry shader invocation is 4.
[maxvertexcount(12)]
void GS(triangle  VertexOut gin[3],uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{

    float3 center = float3(0, 0, 0);
    float distToCenter = length(gEyePosW - center);
    float subdivisionDistance1 = 25.0f; // 最近，最大细分
    float subdivisionDistance2 = 40.0f;
    float subdivisionDistance3 = 55.0f;
    int subdivLevel = 0;
    if (distToCenter < subdivisionDistance1)
        subdivLevel = 3; // 最深细分，比如 3 次递归
    else if (distToCenter < subdivisionDistance2)
        subdivLevel = 2; // 中等细分
    else if (distToCenter < subdivisionDistance3)
        subdivLevel = 1; // 低细分
    else
        subdivLevel = 0; // 不细分
    if (subdivLevel > 0)
    {
        SubdivideTriangleToSphere(gin[0].PosW, gin[1].PosW, gin[2].PosW, primID, triStream, 0, subdivLevel);
    }
    else
    {
        // 视距较远，直接输出原始三角形（不细分）
        GeoOut gout;
        [unroll]
        for (int i = 0; i < 3; ++i)
        {
            gout.PosW = gin[i].PosW;
            gout.NormalW = gin[i].NormalW;
            gout.PrimID = primID;
            gout.PosH = gin[i].PosH;
            gout.TexC = gin[i].TexC;
            triStream.Append(gout);
        }
        triStream.RestartStrip();
    }
}

float4 PS(GeoOut pin) : SV_Target
{
	float3 uvw = float3(pin.TexC, pin.PrimID%3);
    float4 diffuseAlbedo = float4(0.4f, 0.6f, 0.8f, 1.0f);;

#ifdef ALPHA_TEST
	// Discard pixel if texture alpha < 0.1.  We do this test as soon
	// as possible in the shader so that we can potentially exit the
	// shader early, thereby skipping the rest of the shader code.
	clip(diffuseAlbedo.a - 0.1f);
#endif

    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye.
	float3 toEyeW = gEyePosW - pin.PosW;
	float distToEye = length(toEyeW);
	toEyeW /= distToEye; // normalize

    // Light terms.
    float4 ambient = gAmbientLight*diffuseAlbedo;

    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}


