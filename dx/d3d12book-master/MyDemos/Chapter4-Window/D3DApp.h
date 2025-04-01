#pragma once
#include "D3DUtil.h"
#include "GameTimer.h"
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
using Microsoft::WRL::ComPtr;





class D3DApp
{
public:
	int Run();
	virtual bool Initialize();
protected:
	D3DApp(HINSTANCE hInstance);
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	virtual void OnResize();
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;


	void CalculateFrameStats();
protected:
	static D3DApp* mApp;

	HINSTANCE mhAppInst = nullptr; // application instance handle
	HWND      mhMainWnd = nullptr; // main window handle


	ComPtr<IDXGIFactory4> mdxgiFactory;
	ComPtr<ID3D12Device3> md3dDevice;

	UINT64 mCurrentFence = 0;
	ComPtr<ID3D12Fence1> mFence;


	ComPtr<ID3D12CommandQueue> mCommandQueue;
	ComPtr<ID3D12CommandAllocator> mDirectCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList4> mCommmandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	ComPtr<IDXGISwapChain> mSwapChain;
	ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

	ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;
protected:
	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;
	UINT m4xMsaaQuality = 0;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 800;
	int mClientHeight = 600;

	std::wstring mMainWndCaption = L"d3d App";

	// Set true to use 4X MSAA (?.1.8).  The default is false.
	bool      m4xMsaaState = false;    // 4X MSAA enabled
	UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

private:
	bool mAppPaused = false;
	GameTimer mTimer;
};

