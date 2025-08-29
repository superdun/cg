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

     // 计算三角形边矢量
    float3 edge0 = gin[1].PosW - gin[0].PosW;
    float3 edge1 = gin[2].PosW - gin[0].PosW;
    // 三角形面法线（未归一化）
    float3 faceNormal = normalize(cross(edge0, edge1));
    float t = max(fmod (gTotalTime,10)-3.0f,0.0f) ;
    float displacementAmount = (-pow(2,-1*t)+1)*6; 
    GeoOut gout;
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        float3 pos = gin[i].PosW;
        // 沿面法线方向平移顶点
        float3 displacedPos = pos + displacementAmount * faceNormal;
        gout.PosW = displacedPos;
        gout.NormalW = gin[i].NormalW;
        gout.PrimID = primID;
        gout.PosH = mul(float4(displacedPos, 1.0f), gViewProj);
        gout.TexC = gin[i].TexC;
        triStream.Append(gout);
    }
    triStream.RestartStrip();

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


