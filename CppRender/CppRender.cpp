// CppRender.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "CppRender.h"
#include <vector>
#include "Sphere.h"
#include "Light.h"
#include "Camera.h"
#include "RayTraceRender.h"
#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::seconds
#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CPPRENDER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPPRENDER));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPRENDER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CPPRENDER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

const std::vector<const Sphere*> sphereList = {
    new Sphere({0.0, -1, 3}, 1.0, {255, 0, 0}, 500, 0.2),
    new Sphere({2, 0.0, 4.0}, 1.0, {0, 0, 255}, 500, 0.3),
    new Sphere({-2.0, 0.0, 4.0}, 1, {0, 255, 0}, 10, 0.4),
    new Sphere({0, -5001, 0}, 5000, {255, 255, 0}, 1000, 0.5)
};

const std::vector<const Light*> lightList = {
     new Light(LightTypeEnum::AmbientLight,{0,0,0},{0,0,0},0.2),
     new Light(LightTypeEnum::PointLight,{2,1,0},{0,0,0},0.6),
     new Light(LightTypeEnum::DirectionalLight,{0,0,0},{1,4,4},0.2),

};


//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        int width = 800;
        int height = 800;
        double viewportWidth = 1;
        double viewportHeight = 1;
        double viewportDistance = 1;
        double viewportCanvasWidthRate = viewportWidth / width;
        double viewportCanvasHeightRate = viewportHeight / height;
        int minX = -width / 2;
        int maxX = width / 2;
        int minY = height / -2;
        int maxY = height / 2;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
       
        int loopCount = 0;
       
        
        while(loopCount<1)
        {
            Camera* camera = new Camera({ 0,0,0 }, { 0,0,0 });
            double  angelY = 0;
            switch (loopCount)
            {
            case 0:
               camera = new Camera({ 0,0,0 }, { 0,0,0 });
				 angelY = 0;
                break;
            case 1:
                camera = new Camera({ 5,0,2 }, { 0,0,0 });
                 angelY = Constants::PI / 4;
                break;
            case 2:
                 camera = new Camera({ 0,0,6 }, { 0,0,0 });
                 angelY = Constants::PI / 2;
                break;
            case 3:
                 camera = new Camera({ -1,0,1 }, { 0,0,0 });
                 angelY = 3* Constants::PI / 4;
                break;
            default:
                 camera = new Camera({ 0,0,0 }, { 0,0,0 });
                 angelY = 0;
                break;
            }
           
            RayTraceRender* render = new RayTraceRender(sphereList, lightList, camera);
            RECT rect;
            rect.left = 0;
            rect.top = 0;
            rect.right = rect.left + width;
            rect.bottom = rect.top + height;
            COLORREF colorRef = RGB(Constants::BACKGROUND_COLOR[0], Constants::BACKGROUND_COLOR[1], Constants::BACKGROUND_COLOR[2]);
            FillRect(hdc, &rect, (HBRUSH)(colorRef));
            
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    double viewportX = (x - width / 2) * viewportCanvasWidthRate;
                    double viewportY = (-y + height / 2) * viewportCanvasHeightRate;

                    std::array<double, 3> directionVector = VectorHelper::VectorSub({ viewportX,viewportY,viewportDistance }, {0,0,0});
					directionVector = VectorHelper::VecRotate(directionVector,0, angelY,0);
                    auto colorArray = render->GetViewPointColor(camera->GetPosition(), directionVector, 1, Constants::Infinity, 1);
                    COLORREF colorPixelRef = RGB(colorArray[0], colorArray[1], colorArray[2]);
                    SetPixel(hdc, x, y, colorPixelRef);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            loopCount++;
            delete camera;
            delete render;
        }

        EndPaint(hWnd, &ps);

    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
