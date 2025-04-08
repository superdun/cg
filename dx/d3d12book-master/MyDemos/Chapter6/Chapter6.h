#pragma once

#include "resource.h"
#include "framework.h"
#include "../Chapter4-Window/D3DApp.h"
#include "../Chapter4-Window/GameTimer.h"
#include <DirectXColors.h>

class BoxApp : public D3DApp
{
public:
    BoxApp(HINSTANCE hInstance);
    ~BoxApp();

    virtual bool Initialize()override;

private:
    virtual void OnResize()override;
    virtual void Update(const GameTimer& gt)override;
    virtual void Draw(const GameTimer& gt)override;
    void BuildBoxGeometry();
};