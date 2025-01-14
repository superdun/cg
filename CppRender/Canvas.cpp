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
	FillRect(hMemDC, &rect, (HBRUSH)(colorRef));
}

void Canvas::RenderSection(int startY, int endY,std::vector<std::vector<COLORREF>>& buffer) const
{
    for (int x = 0; x < canvasWidth; x++)
    {
        for (int y = startY; y < endY; y++)
        {
            double viewportX = (x - canvasMaxX) * viewportCanvasWidthRate;
            double viewportY = (-y + canvasMaxY) * viewportCanvasHeightRate;

            std::array<double, 3> directionVector = VectorHelper::VectorSub({ viewportX,viewportY,viewportDistance }, { 0,0,0 });
            directionVector = VectorHelper::VecRotate(directionVector,0, camera->GetAngleY(), 0);
            auto colorArray = render->GetViewPointColor(camera->GetPosition(), directionVector, 1, Constants::Infinity, 1);
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



Canvas::Canvas(const RayTraceRender* render,const HDC& hMemDC, const int canvasWidth, const int canvasHeight, const Camera* camera, const double viewportWidth, const double viewportHeight, const double viewportDistance)
    :render(render), hMemDC(hMemDC), canvasWidth(canvasWidth), canvasHeight(canvasHeight), camera(camera),
    canvasMinX(canvasWidth / -2), canvasMaxX(canvasWidth / 2),
    canvasMinY(canvasHeight / -2), canvasMaxY(canvasHeight / 2),viewportWidth(viewportWidth),viewportHeight(viewportHeight),
	viewportDistance(viewportDistance), viewportCanvasWidthRate(viewportWidth / canvasWidth), viewportCanvasHeightRate(viewportHeight/canvasHeight)


{
}

Canvas::~Canvas()
{
}

void Canvas::RunRender() const
{
    const int numThreads = std::thread::hardware_concurrency(); // ��ȡ���õ��߳���
    std::vector<std::thread> threads;
    std::vector<std::vector<COLORREF>> buffer(canvasHeight, std::vector<COLORREF>(canvasWidth));

    int sectionHeight = canvasHeight / numThreads;
    for (int i = 0; i < numThreads; ++i)
    {
        int startY = i * sectionHeight;
        int endY = (i == numThreads - 1) ? canvasHeight : startY + sectionHeight; // ȷ�����һ���̴߳���ʣ�����
        threads.emplace_back(&Canvas::RenderSection, this, startY, endY, std::ref(buffer));
    }

    // �ȴ������߳����
    for (auto& thread : threads)
    {
        thread.join();
    }

   MergeBuffer(buffer);

}
