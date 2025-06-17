#pragma once
#include "../Chapter4-Window/D3DUtil.h"
#include "../Chapter6/MathHelper.h"
#include "../Chapter6/UploadBuffer.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
struct ObjectConstants
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
};
struct PassConstants
{
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Indices [0, NUM_DIR_LIGHTS) are directional lights;
	// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
	// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
	// are spot lights for a maximum of MaxLights per object.
	Light Lights[MaxLights];
};

struct FrameResource
{
public:
	static std::unique_ptr<FrameResource> CreateWithMaterial(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
	static std::unique_ptr<FrameResource> CreateWaveWithMaterial(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT waveVertCount);
	static std::unique_ptr<FrameResource> CreateWithOutMaterial(ID3D12Device* device, UINT passCount, UINT objectCount);
	static std::unique_ptr<FrameResource> CreateWaveWithOutMaterial(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount);
	//FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	//FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
	//FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount);
	//FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	// We cannot reset the allocator until the GPU is done processing the commands.
	// So each frame needs their own allocator.
	ComPtr<ID3D12CommandAllocator> CmdListAlloc = nullptr;

	// We can only have one set of constant buffers in flight at a time.
	// So we will need to create a new set of constant buffers for each frame.
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
	// 基于更新频率对常量数据分组
	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;

	UINT64 Fence = 0;
	// We cannot update a dynamic vertex buffer until the GPU is done processing
	// the commands that reference it.  So each frame needs their own.
	std::unique_ptr<UploadBuffer<Vertex>> WavesVB = nullptr;
private:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
};