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


 // We expand each point into a quad (4 vertices), so the maximum number of vertices
 // we output per geometry shader invocation is 4.
[maxvertexcount(9)]
void GS(line  VertexOut gin[2],uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
    GeoOut gout;
    // 线段端点位置
    float3 p0 = gin[0].PosW;
    float3 p1 = gin[1].PosW;
    // 线段方向
    float3 dir = normalize(p1 - p0);
    float d = distance(p1, p0)/2;
    // 假设up方向是Y方向
    float3 up = float3(0, 1, 0);
    // 计算垂直于线段方向的法线
    float3 normalDir = normalize(cross(dir, up));
    // 按钮大小相乘得到偏移距离，这里用平均大小的x分量为垂直方向偏移距离
    float size =2;
    // 构造三角形三个顶点的位置
    float3 v0 = p0;
    float3 v1 = p1;
    float3 v2 = (p0 + p1) * 0.5 + up * size;
    float3 v3 = v2+d * dir;
    float3 v4 = v2 -d * dir;

    // 计算对应的法线，假设法线都为垂直方向，或者用某策略赋值
    float3 normal = normalize(cross(v1 - v0, v2 - v0));

    // 输出第一个三角形
    gout.PosW = v0; gout.NormalW = normal; gout.PrimID = primID; gout.PosH = mul(float4(v0,1), gViewProj); gout.TexC = float2(0,0); triStream.Append(gout);
    gout.PosW = v1; gout.NormalW = normal; gout.PrimID = primID; gout.PosH = mul(float4(v1,1), gViewProj); gout.TexC = float2(1,0); triStream.Append(gout);
    gout.PosW = v2; gout.NormalW = normal; gout.PrimID = primID; gout.PosH = mul(float4(v2,1), gViewProj); gout.TexC = float2(0.5,1); triStream.Append(gout);
    triStream.RestartStrip();   // 结束第一个三角形

    gout.PosW = v0; gout.NormalW = normal; gout.PrimID = primID; gout.PosH = mul(float4(v0,1), gViewProj); gout.TexC = float2(0,0); triStream.Append(gout);
    gout.PosW = v4; gout.NormalW = normal; gout.PrimID = primID; gout.PosH = mul(float4(v4,1), gViewProj); gout.TexC = float2(1,0); triStream.Append(gout);
    gout.PosW = v2; gout.NormalW = normal; gout.PrimID = primID; gout.PosH = mul(float4(v2,1), gViewProj); gout.TexC = float2(0.5,1); triStream.Append(gout);
    triStream.RestartStrip();   // 结束第二个三角形

    gout.PosW = v1; gout.NormalW = normal; gout.PrimID = primID; gout.PosH = mul(float4(v1,1), gViewProj); gout.TexC = float2(0,0); triStream.Append(gout);
    gout.PosW = v3; gout.NormalW = normal; gout.PrimID = primID; gout.PosH = mul(float4(v3,1), gViewProj); gout.TexC = float2(1,0); triStream.Append(gout);
    gout.PosW = v2; gout.NormalW = normal; gout.PrimID = primID; gout.PosH = mul(float4(v2,1), gViewProj); gout.TexC = float2(0.5,1); triStream.Append(gout);
    triStream.RestartStrip();   // 结束第3个三角形
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


