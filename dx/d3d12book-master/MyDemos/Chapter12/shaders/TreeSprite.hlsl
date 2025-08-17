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
[maxvertexcount(48)]
void GS(triangle  VertexOut gin[3],uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
    float3 center = float3(0, 0, 0);
    float radius = 8.0f; // This was hardcoded in SubdivideTriangleToSphere, so keeping it here.
    float distToCenter = length(gEyePosW - center);
    float subdivisionDistance1 = 25.0f; // 最近，最大细分
    float subdivisionDistance2 = 50.0f;
    float subdivisionDistance3 = 90.0f;
    int subdivLevel = 0;
    if (distToCenter < subdivisionDistance1)
        subdivLevel = 3; // 最深细分，比如 3 次递归
    else if (distToCenter < subdivisionDistance2)
        subdivLevel = 2; // 中等细分
    else if (distToCenter < subdivisionDistance3)
        subdivLevel = 1; // 低细分
    else
        subdivLevel = 0; // 不细分
    // Get initial vertices
    float3 v0_orig = ProjectToSphere(gin[0].PosW, center, radius);
    float3 v1_orig = ProjectToSphere(gin[1].PosW, center, radius);
    float3 v2_orig = ProjectToSphere(gin[2].PosW, center, radius);
    // Array to store triangles for current level of subdivision
    // A single level of subdivision produces 4 triangles from 1.
    // Max level 3 means 4^3 = 64 triangles.
    // Max number of vertices = 64 * 3 = 192.
    // This array will hold the vertices of the triangles to be processed in the current "level" of iteration.
    // Since we can't dynamically size arrays, we'll need to define a large enough array for the max level.
    // A simpler approach for limited levels is manual unrolling as below.
    if (subdivLevel == 0)
    {
        // No subdivision, output original triangle
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
    else if (subdivLevel == 1)
    {
        // Subdivide once (from 1 triangle to 4)
        float3 m0 = ProjectToSphere((v0_orig + v1_orig) * 0.5f, center, radius);
        float3 m1 = ProjectToSphere((v1_orig + v2_orig) * 0.5f, center, radius);
        float3 m2 = ProjectToSphere((v2_orig + v0_orig) * 0.5f, center, radius);
        // 4 new triangles
        OutputVertex(v0_orig, primID, triStream); OutputVertex(m0, primID, triStream); OutputVertex(m2, primID, triStream); triStream.RestartStrip();
        OutputVertex(m0, primID, triStream); OutputVertex(v1_orig, primID, triStream); OutputVertex(m1, primID, triStream); triStream.RestartStrip();
        OutputVertex(m2, primID, triStream); OutputVertex(m1, primID, triStream); OutputVertex(v2_orig, primID, triStream); triStream.RestartStrip();
        OutputVertex(m0, primID, triStream); OutputVertex(m1, primID, triStream); OutputVertex(m2, primID, triStream); triStream.RestartStrip();
    }
    else if (subdivLevel == 2)
    {
        // Subdivide twice (from 1 triangle to 16)
        // Level 1 triangles
        float3 l1_t0_v0=v0_orig, l1_t0_v1, l1_t0_v2;
        float3 l1_t1_v0, l1_t1_v1=v1_orig, l1_t1_v2;
        float3 l1_t2_v0, l1_t2_v1, l1_t2_v2=v2_orig;
        float3 l1_t3_v0, l1_t3_v1, l1_t3_v2;
        float3 m0_l1 = ProjectToSphere((v0_orig + v1_orig) * 0.5f, center, radius);
        float3 m1_l1 = ProjectToSphere((v1_orig + v2_orig) * 0.5f, center, radius);
        float3 m2_l1 = ProjectToSphere((v2_orig + v0_orig) * 0.5f, center, radius);
        l1_t0_v1 = m0_l1; l1_t0_v2 = m2_l1; // T0: (v0, m0, m2)
        l1_t1_v0 = m0_l1; l1_t1_v2 = m1_l1; // T1: (m0, v1, m1)
        l1_t2_v0 = m2_l1; l1_t2_v1 = m1_l1; // T2: (m2, m1, v2)
        l1_t3_v0 = m0_l1; l1_t3_v1 = m1_l1; l1_t3_v2 = m2_l1; // T3: (m0, m1, m2)
        // Subdivide each of the 4 triangles from Level 1
        // Using a helper macro or inline function for this would make it cleaner, but GS limitations...
        // We'll have to repeat the subdivision logic 4 times.
        // --- Subdivide T0 (v0_orig, m0_l1, m2_l1) ---
        float3 t0_m0 = ProjectToSphere((l1_t0_v0 + l1_t0_v1) * 0.5f, center, radius);
        float3 t0_m1 = ProjectToSphere((l1_t0_v1 + l1_t0_v2) * 0.5f, center, radius);
        float3 t0_m2 = ProjectToSphere((l1_t0_v2 + l1_t0_v0) * 0.5f, center, radius);
        OutputVertex(l1_t0_v0, primID, triStream); OutputVertex(t0_m0, primID, triStream); OutputVertex(t0_m2, primID, triStream); triStream.RestartStrip();
        OutputVertex(t0_m0, primID, triStream); OutputVertex(l1_t0_v1, primID, triStream); OutputVertex(t0_m1, primID, triStream); triStream.RestartStrip();
        OutputVertex(t0_m2, primID, triStream); OutputVertex(t0_m1, primID, triStream); OutputVertex(l1_t0_v2, primID, triStream); triStream.RestartStrip();
        OutputVertex(t0_m0, primID, triStream); OutputVertex(t0_m1, primID, triStream); OutputVertex(t0_m2, primID, triStream); triStream.RestartStrip();
        // --- Subdivide T1 (m0_l1, v1_orig, m1_l1) ---
        float3 t1_m0 = ProjectToSphere((l1_t1_v0 + l1_t1_v1) * 0.5f, center, radius);
        float3 t1_m1 = ProjectToSphere((l1_t1_v1 + l1_t1_v2) * 0.5f, center, radius);
        float3 t1_m2 = ProjectToSphere((l1_t1_v2 + l1_t1_v0) * 0.5f, center, radius);
        OutputVertex(l1_t1_v0, primID, triStream); OutputVertex(t1_m0, primID, triStream); OutputVertex(t1_m2, primID, triStream); triStream.RestartStrip();
        OutputVertex(t1_m0, primID, triStream); OutputVertex(l1_t1_v1, primID, triStream); OutputVertex(t1_m1, primID, triStream); triStream.RestartStrip();
        OutputVertex(t1_m2, primID, triStream); OutputVertex(t1_m1, primID, triStream); OutputVertex(l1_t1_v2, primID, triStream); triStream.RestartStrip();
        OutputVertex(t1_m0, primID, triStream); OutputVertex(t1_m1, primID, triStream); OutputVertex(t1_m2, primID, triStream); triStream.RestartStrip();
        // --- Subdivide T2 (m2_l1, m1_l1, v2_orig) ---
        float3 t2_m0 = ProjectToSphere((l1_t2_v0 + l1_t2_v1) * 0.5f, center, radius);
        float3 t2_m1 = ProjectToSphere((l1_t2_v1 + l1_t2_v2) * 0.5f, center, radius);
        float3 t2_m2 = ProjectToSphere((l1_t2_v2 + l1_t2_v0) * 0.5f, center, radius);
        OutputVertex(l1_t2_v0, primID, triStream); OutputVertex(t2_m0, primID, triStream); OutputVertex(t2_m2, primID, triStream); triStream.RestartStrip();
        OutputVertex(t2_m0, primID, triStream); OutputVertex(l1_t2_v1, primID, triStream); OutputVertex(t2_m1, primID, triStream); triStream.RestartStrip();
        OutputVertex(t2_m2, primID, triStream); OutputVertex(t2_m1, primID, triStream); OutputVertex(l1_t2_v2, primID, triStream); triStream.RestartStrip();
        OutputVertex(t2_m0, primID, triStream); OutputVertex(t2_m1, primID, triStream); OutputVertex(t2_m2, primID, triStream); triStream.RestartStrip();
        // --- Subdivide T3 (m0_l1, m1_l1, m2_l1) ---
        float3 t3_m0 = ProjectToSphere((l1_t3_v0 + l1_t3_v1) * 0.5f, center, radius);
        float3 t3_m1 = ProjectToSphere((l1_t3_v1 + l1_t3_v2) * 0.5f, center, radius);
        float3 t3_m2 = ProjectToSphere((l1_t3_v2 + l1_t3_v0) * 0.5f, center, radius);
        OutputVertex(l1_t3_v0, primID, triStream); OutputVertex(t3_m0, primID, triStream); OutputVertex(t3_m2, primID, triStream); triStream.RestartStrip();
        OutputVertex(t3_m0, primID, triStream); OutputVertex(l1_t3_v1, primID, triStream); OutputVertex(t3_m1, primID, triStream); triStream.RestartStrip();
        OutputVertex(t3_m2, primID, triStream); OutputVertex(t3_m1, primID, triStream); OutputVertex(l1_t3_v2, primID, triStream); triStream.RestartStrip();
        OutputVertex(t3_m0, primID, triStream); OutputVertex(t3_m1, primID, triStream); OutputVertex(t3_m2, primID, triStream); triStream.RestartStrip();
    }

}

float4 PS(GeoOut pin) : SV_Target
{
	float3 uvw = float3(pin.TexC, pin.PrimID%3);
    float4 diffuseAlbedo = float4(0.4f, 0.6f, 0.8f, 1.0f);;



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

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}


