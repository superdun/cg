#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <string>
#include <comdef.h>
#include <iostream>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <d3dx12.h>
#include <array>
#include <unordered_map>
#include <memory>
using Microsoft::WRL::ComPtr;

class D3DUtil {
public:
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);
    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entry,
		const std::string& target);
    
	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
        //这段代码 (byteSize + 255) & ~255 是一种常见的技巧，用于将 byteSize 向上对齐到最近的 256 字节边界
		return (byteSize + 255) & ~255;
	}
};
class DxException {
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};


inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x){ if(x){ x->Release(); x = 0; } }
#endif

// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
// geometries are stored in one vertex and index buffer.  It provides the offsets
// and data needed to draw a subset of geometry stores in the vertex and index 
// buffers so that we can implement the technique described by Figure 6.3.
struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	// Bounding box of the geometry defined by this submesh. 
	// This is used in later chapters of the book.
	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	// Give it a name so we can look it up by name.
	std::string Name;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	//习题2
	Microsoft::WRL::ComPtr<ID3DBlob> VPosBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> VColorBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> VPosBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> VColorBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> VPosBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> VColorBufferUploader = nullptr;
	UINT VPosByteStride = 0;
	UINT VPosBufferByteSize = 0;
	UINT VColorByteStride = 0;
	UINT VColorBufferByteSize = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	// Data about the buffers.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}
	D3D12_VERTEX_BUFFER_VIEW VPosBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VPosBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VPosByteStride;
		vbv.SizeInBytes =VPosBufferByteSize;

		return vbv;
	}
	D3D12_VERTEX_BUFFER_VIEW VColorBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VColorBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VColorByteStride;
		vbv.SizeInBytes = VColorBufferByteSize;

		return vbv;
	}


	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	// We can free this memory after we finish upload to the GPU.
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;

		//习题2
		VPosBufferUploader = nullptr;
		VColorBufferUploader = nullptr;
	}
};




