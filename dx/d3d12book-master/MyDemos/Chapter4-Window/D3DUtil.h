#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <string>
#include <comdef.h>
#include <iostream>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <d3dx12.h>
#include <array>
using Microsoft::WRL::ComPtr;

class D3DUtil {
public:
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);
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



