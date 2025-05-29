#include "FrameResource.h"

//FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount)
//{
//	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
//	ObjectCB = std::make_unique <UploadBuffer<ObjectConstants>>(device, objectCount, true);
//	PassCB = std::make_unique <UploadBuffer<PassConstants>>(device, passCount, true);
//}

//FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
//{
//	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
//	ObjectCB = std::make_unique <UploadBuffer<ObjectConstants>>(device, objectCount, true);
//	PassCB = std::make_unique <UploadBuffer<PassConstants>>(device, passCount, true);
//	MaterialCB = std::make_unique <UploadBuffer<MaterialConstants>>(device, materialCount, true);
//
//}
//
//FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT waveVertCount)
//{
//	ThrowIfFailed(device->CreateCommandAllocator(
//		D3D12_COMMAND_LIST_TYPE_DIRECT,
//		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
//
//	PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
//	ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
//	MaterialCB = std::make_unique <UploadBuffer<MaterialConstants>>(device, materialCount, true);
//	WavesVB = std::make_unique<UploadBuffer<Vertex>>(device, waveVertCount, false);
//}


std::unique_ptr<FrameResource> FrameResource::CreateWithMaterial(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
	std::unique_ptr<FrameResource> resource(new FrameResource(device, passCount, objectCount));
	// 初始化 MaterialCB
	resource->MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
	return resource;
}

std::unique_ptr<FrameResource> FrameResource::CreateWaveWithMaterial(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT waveVertCount)
{
	std::unique_ptr<FrameResource> resource(new FrameResource(device, passCount, objectCount));
	// 初始化 MaterialCB
	resource->WavesVB = std::make_unique<UploadBuffer<Vertex>>(device, waveVertCount, false);
	resource->MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
	return resource;
}

std::unique_ptr<FrameResource> FrameResource::CreateWithOutMaterial(ID3D12Device* device, UINT passCount, UINT objectCount)
{
	std::unique_ptr<FrameResource> resource(new FrameResource(device, passCount, objectCount));
	// 初始化 MaterialCB
	return resource;
}

std::unique_ptr<FrameResource> FrameResource::CreateWaveWithOutMaterial(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount)
{
	std::unique_ptr<FrameResource> resource(new FrameResource(device, passCount, objectCount));
	// 初始化 MaterialCB
	resource->WavesVB = std::make_unique<UploadBuffer<Vertex>>(device, waveVertCount, false);
	return resource;
}
FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
	ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
	PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
}


FrameResource::~FrameResource()
{
}
