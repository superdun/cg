#pragma once

#include "resource.h"
#include "framework.h"
#include "Chapter4-Window.h"
#include "D3DApp.h"
#include "GameTimer.h"
#include <DirectXColors.h>

class InitDirect3DApp : public D3DApp
{
public:
    InitDirect3DApp(HINSTANCE hInstance);
    ~InitDirect3DApp();

    virtual bool Initialize()override;

private:
    virtual void OnResize()override;
    virtual void Update(const GameTimer& gt)override;
    virtual void Draw(const GameTimer& gt)override;

};