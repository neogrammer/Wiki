This lesson covers the basics of HDR rendering, tone-mapping, and adding HDR10 wide color gamut rendering with _DirectX Tool Kit_.

# Setup
First create a new project. For this lesson, use the [[DeviceResources]] variant of the [[The basic game loop]], then use the instructions in [[Adding the DirectX Tool Kit]].

# Setting up an HDR render target

Save the files [RenderTexture.h](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.h) and [RenderTexture.cpp](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.cpp) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item....** Select "RenderTexture.h" and hit "OK". Repeat for "RenderTexture.cpp".

Add to the **Game.h** file to the ``#include`` section:

    #include "RenderTexture.h"

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

    std::unique_ptr<DX::RenderTexture> m_hdrScene;
    std::unique_ptr<DirectX::ToneMapPostProcess> m_toneMap;

In the **Game.cpp** file, modify the Game class constructor:

    m_deviceResources = std::make_unique<DX::DeviceResources>(
        DXGI_FORMAT_R10G10B10A2_UNORM,
        DXGI_FORMAT_D32_FLOAT, 2, D3D_FEATURE_LEVEL_10_0);
    m_deviceResources->RegisterDeviceNotify(this);

    m_hdrScene = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_R16G16B16A16_FLOAT);

> We are using ``DXGI_FORMAT_R16G16B16A16_FLOAT`` for the HDR render target. Other reasonable options (depending on your Direct3D hardware feature level) including ``DXGI_FORMAT_R32G32B32A32_FLOAT``, or ``DXGI_FORMAT_R11G1B10_FLOAT``. ``DXGI_FORMAT_R32G32B32_FLOAT`` (a 96 bits-per-pixel format) support is often optional for various rendering operations. The ``DXGI_FORMAT_R9G9B9E5_SHAREDEXP`` is not typically supported for render targets.

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

    m_hdrScene->SetDevice(device);

    m_toneMap = std::make_unique<ToneMapPostProcess>(device);

    // Set tone-mapper as 'pass-through' for now...
    m_toneMap->SetOperator(ToneMapPostProcess::None);
    m_toneMap->SetTransferFunction(ToneMapPostProcess::SRGB);

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

    auto size = m_deviceResources->GetOutputSize();
    m_hdrScene->SetWindow(size);

    m_toneMap->SetHDRSourceTexture(m_hdrScene->GetShaderResourceView());

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_hdrScene->ReleaseDevice();
    m_toneMap.reset();

In **Game.cpp**, modify **Clear** as follows:

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();

    auto renderTarget = m_hdrScene->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    XMVECTORF32 color;
    color.v = XMColorSRGBToRGB(Colors::CornflowerBlue);
    context->ClearRenderTargetView(renderTarget, color);
    ...

In **Game.cpp**, modify **Render** as follows:

    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.

    auto renderTarget = m_deviceResources->GetRenderTargetView();
    context->OMSetRenderTargets(1, &renderTarget, nullptr);

    m_toneMap->Process(context);

    ID3D11ShaderResourceView* nullsrv[] = { nullptr };
    context->PSSetShaderResources(0, 1, nullsrv);

    // Show the new frame.
    m_deviceResources->Present();

Build and run, and the result will be the original 'cornflower blue' screen.

# Rendering a test scene

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

    DirectX::SimpleMath::Matrix m_world;
    DirectX::SimpleMath::Matrix m_view;
    DirectX::SimpleMath::Matrix m_proj;

    std::unique_ptr<DirectX::GeometricPrimitive> m_shape;

    float m_colorScale;

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

    auto context = m_deviceResources->GetD3DDeviceContext();
    m_shape = GeometricPrimitive::CreateTeapot(context);
    
    m_world = Matrix::Identity;

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

    m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
        Vector3::Zero, Vector3::UnitY);
    m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
        float(size.right) / float(size.bottom), 0.1f, 10.f);

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_shape.reset();

In **Game.cpp**, add to the TODO of **Render**:

    m_shape->Draw(m_world, m_view, m_proj, XMVectorSetW(Colors::White * m_colorScale, 1.f));

In **Game.cpp**, add to the TODO of **Update**:

    float time = float(timer.GetTotalSeconds());
    
    m_world = Matrix::CreateRotationZ(cosf(time) * 2.f);

    m_colorScale = 1.f + cosf(time);

Build and run to see the scene with a teapot. The color scales between black and white which for now is full saturated.

# Adding tone mapping

*UNDER DEVELOPMENT*

# Using HDR10 display output

*UNDER DEVELOPMENT*

**Next lessons:** [[Game controller input]], [[Using the SimpleMath library]], [[Adding the DirectX Tool Kit for Audio]]

# Further reading 

DirectX Tool Kit docs [[PostProcess]] and [[ToneMapPostProcess]]

