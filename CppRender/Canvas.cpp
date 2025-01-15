#include "Canvas.h"
#include <iostream>
#include <thread>


void Canvas::resetCanvas() const
{
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = rect.left + canvasWidth;
    rect.bottom = rect.top + canvasHeight;

    COLORREF colorRef = RGB(Constants::BACKGROUND_COLOR[0], Constants::BACKGROUND_COLOR[1], Constants::BACKGROUND_COLOR[2]);
    HBRUSH brush = CreateSolidBrush(colorRef); // Create a brush with the specified color
    FillRect(hMemDC, &rect, brush);
    DeleteObject(brush); 
}

void Canvas::RenderSectionByRayTrace(int startY, int endY,std::vector<std::vector<COLORREF>>& buffer) const
{
    for (int x = 0; x < canvasWidth; x++)
    {
        for (int y = startY; y < endY; y++)
        {
            double viewportX = (x - canvasMaxX) * viewportCanvasWidthRate;
            double viewportY = (-y + canvasMaxY) * viewportCanvasHeightRate;

            std::array<double, 3> directionVector = VectorHelper::VectorSub({ viewportX,viewportY,viewportDistance }, { 0,0,0 });
            directionVector = VectorHelper::VecRotate(directionVector,0, camera->GetAngleY(), 0);
            auto colorArray = renderRayTrace->GetViewPointColor(camera->GetPosition(), directionVector, 1, Constants::Infinity, 1);
            COLORREF colorPixelRef = RGB(colorArray[0], colorArray[1], colorArray[2]);
            buffer[y][x] = colorPixelRef;
        }
    }
}

void Canvas::MergeBuffer(std::vector<std::vector<COLORREF>>& buffer) const
{
    for (int y = 0; y < canvasHeight; y++)
    {
        for (int x = 0; x < canvasWidth; x++)
        {
            SetPixel(hMemDC, x, y, buffer[y][x]);
        }
    }
}



Canvas::Canvas(const RayTraceRender* renderRayTrace, const RasterizationRender* renderRasterizationTrace,const HDC& hMemDC, const int canvasWidth, const int canvasHeight, const Camera* camera, const double viewportWidth, const double viewportHeight, const double viewportDistance)
    :renderRayTrace(renderRayTrace), renderRasterizationTrace(renderRasterizationTrace), hMemDC(hMemDC), canvasWidth(canvasWidth), canvasHeight(canvasHeight), camera(camera),
    canvasMinX(canvasWidth / -2), canvasMaxX(canvasWidth / 2),
    canvasMinY(canvasHeight / -2), canvasMaxY(canvasHeight / 2),viewportWidth(viewportWidth),viewportHeight(viewportHeight),
	viewportDistance(viewportDistance), viewportCanvasWidthRate(viewportWidth / canvasWidth), viewportCanvasHeightRate(viewportHeight/canvasHeight)


{
}

Canvas::~Canvas()
{
}

void Canvas::RunRenderByRayTrace() const
{
    const int numThreads = std::thread::hardware_concurrency(); // 获取可用的线程数
    std::vector<std::thread> threads;
    std::vector<std::vector<COLORREF>> buffer(canvasHeight, std::vector<COLORREF>(canvasWidth));

    int sectionHeight = canvasHeight / numThreads;
    for (int i = 0; i < numThreads; ++i)
    {
        int startY = i * sectionHeight;
        int endY = (i == numThreads - 1) ? canvasHeight : startY + sectionHeight; // 确保最后一个线程处理剩余的行
        threads.emplace_back(&Canvas::RenderSectionByRayTrace, this, startY, endY, std::ref(buffer));
    }

    // 等待所有线程完成
    for (auto& thread : threads)
    {
        thread.join();
    }

   MergeBuffer(buffer);

}

void Canvas::RunRenderByRasterization() const
{
	resetCanvas();
	std::array<int, 2> p0 = { 100,100 };
	std::array<int, 2> p1 = { 200,300 };
    std::array<int, 2> pp0 = { 300,500 };
    std::array<int, 2> pp1 = { 200,100 };
	std::array<int, 3> color = { 255,0,0 };
    std::vector<Pixel*> pixels1 = renderRasterizationTrace->DrawLine(p0, p1, color);
    std::vector<Pixel*> pixels2 = renderRasterizationTrace->DrawLine(pp0, pp1, color);
	for (auto pixel : pixels1)
	{
		const auto& position = pixel->GetPosition();
		const auto& color = pixel->GetColor();
		SetPixel(hMemDC, position[0], position[1], RGB(color[0], color[1], color[2]));
	}
    for (auto pixel : pixels2)
    {
        const auto& position = pixel->GetPosition();
        const auto& color = pixel->GetColor();
        SetPixel(hMemDC, position[0], position[1], RGB(color[0], color[1], color[2]));
    }
    for (auto pixel : pixels1)
    {
        delete pixel; // Ensure to free each pixel if dynamically allocated
    }
    for (auto pixel : pixels2)
    {
        delete pixel; // Ensure to free each pixel if dynamically allocated
    }
}
