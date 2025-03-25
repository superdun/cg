// Chapter4.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <d3d12.h>
#include <d3dUtil.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;

typedef struct D3D12_FEATURE_DATA_MULTISAMPLE_QUANLITY_LEVELS {
	DXGI_FORMAT Format;
	UINT SampleCount;
	D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS Flags;
	UINT NumQualityLevels;
} D3D12_FEATURE_DATA_MULTISAMPLE_QUANLITY_LEVELS;;
DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
int main()
{	
	D3D12_FEATURE_DATA_MULTISAMPLE_QUANLITY_LEVELS msQulityLevels;
	msQulityLevels.Format = mBackBufferFormat;
	msQulityLevels.SampleCount = 4;
	msQulityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQulityLevels.NumQualityLevels = 0;
	HRESULT result = md3dDevice();
	ThrowIfFailed()
	cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
