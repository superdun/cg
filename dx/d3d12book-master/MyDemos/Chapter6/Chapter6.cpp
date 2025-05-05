// Chapter4-Window.cpp : 定义应用程序的入口点。
//

#include "Chapter6.h"
#include <DirectXColors.h>


using namespace DirectX;
struct Vertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

struct ObjectConstants
{
    XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

class BoxApp : public D3DApp
{
public:
    BoxApp(HINSTANCE hInstance);
    BoxApp(const BoxApp& rhs) = delete;
    BoxApp& operator=(const BoxApp& rhs) = delete;
    ~BoxApp();

    virtual bool Initialize()override;

private:
    virtual void OnResize()override;
    virtual void Update(const GameTimer& gt)override;
    virtual void Draw(const GameTimer& gt)override;

    virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
    virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
    virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

    void BuildDescriptorHeaps();
    void BuildConstantBuffers();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildBoxGeometry();
    void BuildPSO();

private:

    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

    ComPtr<ID3DBlob> mvsByteCode = nullptr;
    ComPtr<ID3DBlob> mpsByteCode = nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    ComPtr<ID3D12PipelineState> mPSO = nullptr;

    XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
    XMFLOAT4X4 mView = MathHelper::Identity4x4();
    XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    float mTheta = 1.5f * XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        BoxApp theApp(hInstance);
        if (!theApp.Initialize())
            return 0;

        return theApp.Run();
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}

BoxApp::BoxApp(HINSTANCE hInstance) : D3DApp(hInstance)
{
}

BoxApp::~BoxApp()
{
}

bool BoxApp::Initialize()
{
    if (!D3DApp::Initialize())
    {
        return false;
    }
    //重置命令列表的作用是准备它记录新的渲染命令或计算命令
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
    BuildDescriptorHeaps();
    BuildConstantBuffers();
    BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();
    ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	// Wait until initialization is complete.
	FlushCommandQueue();
    return true;
}
void BoxApp::OnResize()
{
    D3DApp::OnResize();
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}



void BoxApp::OnMouseDown(WPARAM btnState, int x, int y) {
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	SetCapture(mhMainWnd);
}
void BoxApp::OnMouseUp(WPARAM btnState, int x, int y) {
	ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y) {
    if ((btnState & MK_LBUTTON)!=0)
    {
        float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));
		mTheta += dx;
		mPhi += dy;
		mPhi = MathHelper::Clamp(mPhi, 0.1f, XM_PI - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);
		mRadius += dx - dy;

		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
    }
    mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BoxApp::Update(const GameTimer& gt) {
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float y = mRadius * cosf(mPhi);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);


    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    //摄像机矩阵
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

    //尤其在需要传递矩阵到 CPU 和 GPU 之间时，使用 XMFLOAT4X4 结构更方便，因为它是一个简单的可直接以浮点数组表示的结构。
    XMStoreFloat4x4(&mView, view);


    //世界矩阵
    DirectX::XMFLOAT4X4 I2(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 3.0f, 0.0f, 1.0f);
    XMMATRIX world1 = XMLoadFloat4x4(&mWorld);
    XMMATRIX world2 = XMLoadFloat4x4(&I2);
    //投影矩阵
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX worldViewProj1 = world1 * view * proj;
    XMMATRIX worldViewProj2 = world2 * view * proj;

    //更新常量缓冲区，在cpu gpu之间传递数据
    ObjectConstants objConstants1;
    XMStoreFloat4x4(&objConstants1.WorldViewProj, XMMatrixTranspose(worldViewProj1));
    ObjectConstants objConstants2;
    XMStoreFloat4x4(&objConstants2.WorldViewProj, XMMatrixTranspose(worldViewProj2));
    mObjectCB->CopyData(0, objConstants1);
    mObjectCB->CopyData(1, objConstants2);

}

void BoxApp::Draw(const GameTimer& gt)
{
    //命令列表分配器准备好重新使用之前分配的内存，可以用来存储新命令
    // 重置命令列表并使用特定的 PSO 作为初始状态


    //你问的“描述符（Descriptor）、根签名（Root Signature）、描述符表（Descriptor Table）、常量缓冲区（Constant Buffer）”之间的关系，是理解 DirectX 12 资源绑定的核心。下面我用简明的语言帮你梳理它们之间的联系和作用。

    //    -- -

    //    ## 1. * *常量缓冲区（Constant Buffer） * *

    //    -常量缓冲区是 GPU 读取的特殊缓冲区，通常用来存储着色器需要的常量数据（如变换矩阵、材质参数等）。
    //    - 举例来说，你可能有两个不同物体，每个物体自己的变换矩阵放在两个不同的常量缓冲区里。

    //    -- -

    //    ## 2. * *描述符（Descriptor） * *

    //    -资源（常量缓冲、纹理、采样器等）不可直接绑定给 GPU，必须通过“描述符”间接访问。
    //    - 描述符是GPU能够理解的“资源视图”，指明资源的类型和所在位置。
    //    - 举例，`CBV`（Constant Buffer View）就是一个描述符，告诉 GPU“这里有一个常量缓冲区，它的起始地址是……大少多大……”。
    //    - 描述符存储在描述符堆（Descriptor Heap）里。

    //    -- -

    //    ## 3. * *描述符堆（Descriptor Heap） * *

    //    -描述符的缓冲区，是GPU可访问的数组。
    //    - 你创建多个CBV描述符放入堆中，描述符堆里的每个描述符都对应一个资源视图。

    //    -- -

    //    ## 4. * *根签名（Root Signature） * *

    //    -根签名定义了着色器能访问资源和绑定方式的协议，类似接口规范。
    //    - 它定义了若干个根参数（Root Parameters），根参数可以是：

    //    - **根常量（Root Constants） * *：直接在命令缓冲里写入的值，直接绑定供着色器访问。
    //    - **根常量缓冲区视图（Root CBV） * *：直接绑定 GPU 虚拟地址，不靠描述符堆。
    //    - **描述符表（Descriptor Table） * *：根参数为描述符的指针（句柄），指向描述符堆的一段连续描述符，间接访问资源。

    //    - 这个设计是为了灵活管理性能和复杂度。

    //    -- -

    //    ## 5. * *描述符表（Descriptor Table） * *

    //    -根参数类型之一，是一段描述符的数组（在描述符堆里）。
    //    - 描述符表告诉GPU：“根参数实际是描述符堆中某块连续描述符的地址”。
    //    - 着色器代码里通过绑定的寄存器（如`b0`, `t0`等）访问这些资源。

    //    -- -

    //    ## 6. * *关系总结示意 * *

    //    | 组件 | 作用 | 关系                            |
    //    |---------------- - | ---------------------------- - | ------------------------------ - |
    //    | 常量缓冲区(CBV) | 存储具体的常量数据 | 需要被 GPU 使用                 |
    //    | 描述符(CBV视图) | 资源的“视图”，间接链接到常量缓冲区 | 放入描述符堆中，用来绑定资源     |
    //    | 描述符堆 | 存放描述符的数组 | 描述符表指向描述符堆中的位置     |
    //    | 根签名 | 约定资源绑定方式 | 包含根参数，根参数可能是描述符表  |
    //    | 描述符表 | 根参数的一种，指向描述符堆的描述符区 | 通过`SetGraphicsRootDescriptorTable`绑定 |

    //    -- -

    //    ## 7. * *绑定过程简单流程 * *

    //    1. * *创建常量缓冲区数据 * *，上传 CPU 内存中的变换矩阵等。
    //    2. * *创建 CBV 描述符 * *，描述符指向常量缓冲区的 GPU 地址，放入描述符堆。
    //    3. * *根签名定义一个描述符表（或单根常量缓冲区视图） * *，让着色器知道在哪里找资源。
    //    4. * *绘制命令里调用 `SetGraphicsRootDescriptorTable` 绑定指定描述符堆的描述符句柄（GPU指针）到根参数上。 * *
    //    5. * *GPU 在着色器里通过绑定的寄存器访问资源。 * *

    //    -- -

    //    ## 8. * *示例：用描述符表绑定两个不同常量缓冲区绘制两个物体 * *

    //    -你的描述符堆里有两个 CBV 描述符（0号，1号），分别指向两个常量缓冲区数据。
    //    - 根签名里定义的描述符表范围大小是2（两个连续描述符）。
    //    - 绘制物体A时绑定描述符堆起始句柄（0号描述符）。
    //    - 绘制物体B时绑定偏移一个描述符大小的句柄（1号描述符）。
    //    - 两个物体用不同的变换矩阵等常量。

    //    -- -

    //    # 总结

    //    | 名词 | 本质 | 作用                                              |
    //    |---------------- - | -------------------------------- | ------------------------------------------------ - |
    //    | 常量缓冲区(CB) | GPU内存中的数据块 | 存储着色器要用的常量数据                                  |
    //    | 描述符(Descriptor) | “资源视图”，描述资源如何被访问 | 告诉GPU资源的类型和位置，比如CBV，SRV等                     |
    //    | 描述符堆(Heap) | 存放一批描述符的GPU内存缓冲区 | 提供描述符批量管理                                       |
    //    | 根签名(Root Signature) | 定义资源绑定方式的配置协议，可包含根参数 | 着色器的资源访问入口，配置描述符表、根CBV、根常量等                      |
    //    | 描述符表(Descriptor Table) | 根签名中的一种根参数，指向描述符堆中一段连续描述符 | 通过一次绑定让着色器访问一组资源，提高灵活性和效率 |

    //    -- -

    //    如果你需要，我可以帮你写一份示意代码，演示用根签名、描述符表和常量缓冲区怎么协同使用。需要告诉我！


    ThrowIfFailed(mDirectCmdListAlloc->Reset());
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(),mPSO.Get()));

    mCommandList->RSSetViewports(1,&mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

    mCommandList->ResourceBarrier(
        1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
            CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET)
    );

	// Clear the back buffer and depth buffer.
    mCommandList->ClearRenderTargetView(
        CurrentBackBufferView(),
        reinterpret_cast<const float*>(&Colors::LightSteelBlue),
        0,
        nullptr
    );
	mCommandList->ClearDepthStencilView(
		DepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0,
		0, nullptr
	);
    mCommandList->OMSetRenderTargets(1,&CurrentBackBufferView(),true,&DepthStencilView());
    //常量缓冲区
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

    // Set the vertex buffer view in the command list.
    mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
    // Set the index buffer view in the command list.
    mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());

    // Set the primitive topology.
    mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //将描述符表绑定到根签名的对应参数：
    UINT descriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
    mCommandList->SetGraphicsRootDescriptorTable(0, cbvHandle);

    mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["box"].IndexCount,1, mBoxGeo->DrawArgs["box"].StartIndexLocation, mBoxGeo->DrawArgs["box"].BaseVertexLocation,0);
   
    cbvHandle.Offset(1, descriptorSize);
    mCommandList->SetGraphicsRootDescriptorTable(0, cbvHandle);
    mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["Pymrid"].IndexCount, 1, mBoxGeo->DrawArgs["Pymrid"].StartIndexLocation, mBoxGeo->DrawArgs["Pymrid"].BaseVertexLocation, 0);

    mCommandList->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
           CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
            )
    );

    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    FlushCommandQueue();
}
void BoxApp::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 2;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&mCbvHeap)));
}
void BoxApp::BuildConstantBuffers()
{
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 2, true);

    UINT objCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
    for (int i = 0; i < 2; ++i)
    {
        D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
		cbAddress += i * objCBByteSize;
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        cbvDesc.BufferLocation = cbAddress;
        cbvDesc.SizeInBytes = objCBByteSize;

        CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
        handle.Offset(i, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)); // descriptorSize 通常通过设备查询得到

        md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
    }
}

void BoxApp::BuildBoxGeometry()
{
    // 描述符->上传堆缓冲区->默认堆缓冲区->视图->slot->pipeline
    std::array<Vertex, 13> vertices =
    {
        Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
        Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
        Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
        Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
        Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
        Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
        Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) }),

        Vertex({ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(Colors::White) }),
        Vertex({ XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT4(Colors::Red) }),    
        Vertex({ XMFLOAT3(1.0f, 0.0f, +1.0f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(+1.0f, 0.0f, -1.0f), XMFLOAT4(Colors::Blue) }),   
        Vertex({ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT4(Colors::Yellow) }),
         

    };
    std::array<std::uint16_t, 54> indices =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7,

        // 底面 (两个三角形)
        1, 3,2,
        1, 4, 3,

        // 侧面
        0, 4, 1,  // 左侧面
        0, 3, 4,  // 前侧面
        0, 2, 3,  // 右侧面
        0, 1, 2   // 后侧面
    };


    const UINT64 vbByteSize = vertices.size() * sizeof(Vertex);
    const UINT64 ibByteSize = indices.size() * sizeof(std::uint16_t);

    mBoxGeo = std::make_unique < MeshGeometry>();
    mBoxGeo->Name = "boxGeo";
    ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU));
    CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU));
    CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);


    //uploader也是输出
    mBoxGeo->VertexBufferGPU = D3DUtil::CreateDefaultBuffer(md3dDevice.Get(),
        mCommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);

    mBoxGeo->IndexBufferGPU = D3DUtil::CreateDefaultBuffer(md3dDevice.Get(),
        mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);
    mBoxGeo->VertexByteStride = sizeof(Vertex);
    mBoxGeo->VertexBufferByteSize = vbByteSize;
    mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
    mBoxGeo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh1;
    submesh1.IndexCount = 36;
    submesh1.StartIndexLocation = 0;
    submesh1.BaseVertexLocation = 0;

    SubmeshGeometry submesh2;
    submesh2.IndexCount = 18;
    submesh2.StartIndexLocation = 36;
    submesh2.BaseVertexLocation = 8;

    mBoxGeo->DrawArgs["box"] = submesh1;
    mBoxGeo->DrawArgs["Pymrid"] = submesh2;
}
void BoxApp::BuildRootSignature()
{
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];
    CD3DX12_DESCRIPTOR_RANGE cvbTable;
    cvbTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    slotRootParameter[0].InitAsDescriptorTable(1, &cvbTable);
    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig, &errorBlob);

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);
    ThrowIfFailed(md3dDevice->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature))
    );
}
void BoxApp::BuildShadersAndInputLayout()
{
    HRESULT hr = S_OK;

    mvsByteCode = D3DUtil::CompileShader(L"C:\\repos\\cg\\dx\\d3d12book-master\\MyDemos\\Chapter6\\shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
    mpsByteCode = D3DUtil::CompileShader(L"C:\\repos\\cg\\dx\\d3d12book-master\\MyDemos\\Chapter6\\shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

    mInputLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
}

void BoxApp::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
    psoDesc.pRootSignature = mRootSignature.Get();
	psoDesc.VS = { reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()), mvsByteCode->GetBufferSize() };
    psoDesc.PS = { reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()), mpsByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); 
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
    psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	psoDesc.DSVFormat = mDepthStencilFormat;
    md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO));
}
