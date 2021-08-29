This lesson covers the basics of HDR rendering, tone-mapping, and optionally adding HDR10 wide color gamut (a.k.a. Ultra HD) rendering with _DirectX Tool Kit_.

# Setup
First create a new project using the instructions from the previous lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Setting up an HDR render target

Save the files [RenderTexture.h](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.h) and [RenderTexture.cpp](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.cpp) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item....** Select "RenderTexture.h" and hit "OK". Repeat for "RenderTexture.cpp".

Add to the **Game.h** file to the ``#include`` section:

```cpp
#include "RenderTexture.h"
```

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DX::RenderTexture> m_hdrScene;
std::unique_ptr<DirectX::ToneMapPostProcess> m_toneMap;
```

In the **Game.cpp** file, modify the Game class constructor:

```cpp
m_deviceResources = std::make_unique<DX::DeviceResources>(
    DXGI_FORMAT_R10G10B10A2_UNORM,
    DXGI_FORMAT_D32_FLOAT, 2, D3D_FEATURE_LEVEL_10_0);
m_deviceResources->RegisterDeviceNotify(this);

m_hdrScene = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_R16G16B16A16_FLOAT);
```

> We are using ``DXGI_FORMAT_R16G16B16A16_FLOAT`` for the HDR render target. Other reasonable options (depending on your Direct3D hardware feature level) include ``DXGI_FORMAT_R32G32B32A32_FLOAT``, or ``DXGI_FORMAT_R11G1B10_FLOAT``. ``DXGI_FORMAT_R32G32B32_FLOAT`` (a 96 bits-per-pixel format) support is often optional for various rendering operations, so might not be supported by your GPU. ``DXGI_FORMAT_R9G9B9E5_SHAREDEXP`` is not typically supported for render targets.

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
m_hdrScene->SetDevice(device);

m_toneMap = std::make_unique<ToneMapPostProcess>(device);

// Set tone-mapper as 'pass-through' for now...
m_toneMap->SetOperator(ToneMapPostProcess::None);
m_toneMap->SetTransferFunction(ToneMapPostProcess::SRGB);
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_hdrScene->SetWindow(size);

m_toneMap->SetHDRSourceTexture(m_hdrScene->GetShaderResourceView());
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_hdrScene->ReleaseDevice();
m_toneMap.reset();
```

In **Game.cpp**, modify **Clear** as follows:

```cpp
// Clear the views.
auto context = m_deviceResources->GetD3DDeviceContext();

auto renderTarget = m_hdrScene->GetRenderTargetView();
auto depthStencil = m_deviceResources->GetDepthStencilView();

XMVECTORF32 color;
color.v = XMColorSRGBToRGB(Colors::CornflowerBlue);
context->ClearRenderTargetView(renderTarget, color);
...
```

In **Game.cpp**, modify **Render** as follows:

```cpp
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
```

Build and run, and the result will be the original 'cornflower blue' screen.

# Rendering a test scene

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::SimpleMath::Matrix m_world;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;

std::unique_ptr<DirectX::GeometricPrimitive> m_shape;

float m_colorScale;
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
auto context = m_deviceResources->GetD3DDeviceContext();
m_shape = GeometricPrimitive::CreateTeapot(context);

m_world = Matrix::Identity;
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
    Vector3::Zero, Vector3::UnitY);
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(size.right) / float(size.bottom), 0.1f, 10.f);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_shape.reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
m_shape->Draw(m_world, m_view, m_proj, XMVectorSetW(Colors::White * m_colorScale, 1.f));
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
auto time = static_cast<float>(timer.GetTotalSeconds());

m_world = Matrix::CreateRotationZ(cosf(time) * 2.f);

m_colorScale = 1.f + cosf(time);
```

Build and run to see the scene with a teapot. The color scales between black and white which for now is full saturated.

![Screenshot of passthrough](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotHDRCopy.PNG)

# Adding tone mapping

In the previous render, the color values range from 0 to 2, so the teapot over saturates for half the time. To resolve this, we use tone-mapping. In the first case, this uses a [Reinhard local operator](http://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf) as follows. In **Game.cpp**, modify the **CreateDeviceDependentResources**:

```cpp
// Set the tone-mapper to use Reinhard
m_toneMap->SetOperator(ToneMapPostProcess::Reinhard);
m_toneMap->SetTransferFunction(ToneMapPostProcess::SRGB);
```

Build and run to see the colors less intense white.

![Screenshot of Reinhard](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotHDRReinhard.PNG)

For a better overall color treatment, you may want to use an [ACES](https://github.com/ampas/aces-dev) [Filmic operator](https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/) instead. In **Game.cpp**, modify the **CreateDeviceDependentResources**:

```cpp
// Set the tone-mapper to ACES Filmic
m_toneMap->SetOperator(ToneMapPostProcess::ACESFilmic);
m_toneMap->SetTransferFunction(ToneMapPostProcess::SRGB);
```

Build and run to see a slightly different handling.

![Screenshot of ACES Filmic](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotHDRACES.PNG)

# Using HDR10 display output

Next we'll add support for HDR10 wide color gamut rendering.

> HDR10 display output requires a 4k UHD monitor connected with HDMI 2.0 to a PC running Windows 10 Creators Update. The code has to be built with the Windows 10 Creators Update SDK (15063) which itself requires VS 2017 or later.

In the **Game.cpp** constructor, modify it as follows to use the ``c_EnableHDR`` option of [[DeviceResources]]:

```cpp
m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_R10G10B10A2_UNORM,
    DXGI_FORMAT_D32_FLOAT, 2, D3D_FEATURE_LEVEL_10_0,
    DX::DeviceResources::c_EnableHDR);
```

> You can also use ``DXGI_FORMAT_R16G16B16A16_FLOAT`` instead which means your rendering is always in linear colors, leaving the system to deal with the HDR10 colorspace conversions. This is 'easier' but most games are likely to want to generate the HDR10 signal directly for better control.

In the **Game.cpp** file in the **Render** function, modify the tone-mapping as follows:

```cpp
switch (m_deviceResources->GetColorSpace())
{
default:
    m_toneMap->SetOperator(ToneMapPostProcess::ACESFilmic);
    m_toneMap->SetTransferFunction(
        (m_deviceResources->GetBackBufferFormat() == DXGI_FORMAT_R16G16B16A16_FLOAT)
        ? ToneMapPostProcess::Linear : ToneMapPostProcess::SRGB);
    break;

case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
    m_toneMap->SetOperator(ToneMapPostProcess::None);
    m_toneMap->SetTransferFunction(ToneMapPostProcess::ST2084);
    break;

case DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709:
    // Required if R16G16B16A16_FLOAT is used as display format
    // (otherwise you can omit this case)
    m_toneMap->SetOperator(ToneMapPostProcess::None);
    m_toneMap->SetTransferFunction(ToneMapPostProcess::Linear);
    break;
}

m_toneMap->Process(context);
...
```

Build and run. If the window is on an HDR display, the output will use the ``DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020`` HDR10 path or the ``DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709`` FP16 path. Otherwise it will use the same ACESFilmic sRGB tone-mapping above.

If you don't use ``DXGI_FORMAT_R16G16B16A16_FLOAT`` you can simplify the code a bit:

```cpp
switch (m_deviceResources->GetColorSpace())
{
default:
    m_toneMap->SetOperator(ToneMapPostProcess::ACESFilmic);
    m_toneMap->SetTransferFunction(ToneMapPostProcess::SRGB);
    break;

case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
    m_toneMap->SetOperator(ToneMapPostProcess::None);
    m_toneMap->SetTransferFunction(ToneMapPostProcess::ST2084);
    break;

case DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709:
    throw std::exception("FP16 display output not supported");
    break;
}

m_toneMap->Process(context);
...
```

> If you have difficulty getting HDR10 output to work on your system, be sure to read [HDR and WCG color settings in Windows 10](https://support.microsoft.com/en-us/help/4040263/windows-10-hdr-advanced-color-settings)

# More to explore

* The same technique used here for tone-mapping and HDR10 color conversion can be used for other post-processing effects. See [[PostProcess]], [[BasicPostProcess] and [[DualPostProcess]].

* For HDR10 output, you can select different colorspace transformations for different color grading effects such as using DCI-P3-D65 instead of Rec.709 colorspace as your source or target color primaries. You can also provide a custom color primary rotation matrix. See [[ToneMapPostProcess]] for details.

**Next lessons:** [[Game controller input]], [[Using the SimpleMath library]], [[Adding the DirectX Tool Kit for Audio]]

# Further reading

DirectX Tool Kit docs [[PostProcess]], [[ToneMapPostProcess]], and [[RenderTexture]]
