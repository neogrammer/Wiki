This provides a helper class for managing multisampling render target/depth-stencil buffers.

[MSAAHelper.h](https://github.com/Microsoft/DirectXTK/wiki/MSAAHelper.h)

```cpp
class MSAAHelper
{
public:
    MSAAHelper(DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
        unsigned int sampleCount = 4);

    void SetDevice(ID3D11Device* device);

    void SizeResources(size_t width, size_t height);

    void ReleaseDevice();

    void Resolve(ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer);

    void SetWindow(const RECT& rect);

    ID3D11Texture2D* GetMSAARenderTarget() const { return m_msaaRenderTarget.Get(); }
    ID3D11Texture2D* GetMSAADepthStencil() const { return m_msaaDepthStencil.Get(); }

    ID3D11RenderTargetView* GetMSAARenderTargetView() const { return m_renderTargetView.Get(); }
    ID3D11DepthStencilView* GetMSAADepthStencilView() const { return m_depthStencilView.Get(); }

    DXGI_FORMAT GetBackBufferFormat() const { return m_backBufferFormat; } 
    DXGI_FORMAT GetDepthBufferFormat() const { return m_depthBufferFormat; }
    unsigned int GetSampleCount() const { return m_sampleCount; }

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_msaaRenderTarget;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_msaaDepthStencil;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    DXGI_FORMAT m_backBufferFormat;
    DXGI_FORMAT m_depthBufferFormat;
    unsigned int m_sampleCount;
    unsigned int m_targetSampleCount;
    size_t m_width;
    size_t m_height;
};
```

[MSAAHelper.cpp](https://github.com/Microsoft/DirectXTK/wiki/MSAAHelper.cpp)

```cpp
#include "MSAAHelper.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

MSAAHelper::MSAAHelper(DXGI_FORMAT backBufferFormat,
    DXGI_FORMAT depthBufferFormat,
    unsigned int sampleCount) :
        m_backBufferFormat(backBufferFormat),
        m_depthBufferFormat(depthBufferFormat),
        m_sampleCount(0),
        m_targetSampleCount(sampleCount),
        m_width(0),
        m_height(0)
{
    if (sampleCount < 2 || sampleCount > D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT)
    {
        throw std::out_of_range("MSAA sample count invalid.");
    }
}


void MSAAHelper::SetDevice(ID3D11Device* device)
{
    if (device == m_device.Get())
        return;

    if (m_device)
    {
        ReleaseDevice();
    }

    {
        UINT formatSupport = 0;
        if (FAILED(device->CheckFormatSupport(m_backBufferFormat, &formatSupport)))
        {
            throw std::exception();
        }

        UINT32 required = D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET;
        if ((formatSupport & required) != required)
        {
            throw std::exception();
        }
    }

    {
        UINT formatSupport = 0;
        if (FAILED(device->CheckFormatSupport(m_depthBufferFormat, &formatSupport)))
        {
            throw std::exception();
        }

        UINT32 required = D3D11_FORMAT_SUPPORT_DEPTH_STENCIL | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET;
        if ((formatSupport & required) != required)
        {
            throw std::exception();
        }
    }

    for (m_sampleCount = m_targetSampleCount; m_sampleCount > 1; m_sampleCount--)
    {
        UINT levels = 0;
        if (FAILED(device->CheckMultisampleQualityLevels(m_backBufferFormat, m_sampleCount, &levels)))
            continue;

        if ( levels > 0)
            break;
    }

    if (m_sampleCount < 2)
    {
        throw std::exception();
    }

    m_device = device;
}


void MSAAHelper::SizeResources(size_t width, size_t height)
{
    if (width == m_width && height == m_height)
        return;

    if (m_width > UINT32_MAX || m_height > UINT32_MAX)
    {
        throw std::out_of_range("Invalid width/height");
    }

    if (!m_device)
        return;

    m_width = m_height = 0;

    // Create an MSAA render target
    CD3D11_TEXTURE2D_DESC renderTargetDesc(
        m_backBufferFormat,
        static_cast<UINT>(width),
        static_cast<UINT>(height),
        1, // The render target view has only one texture.
        1, // Use a single mipmap level.
        D3D11_BIND_RENDER_TARGET,
        D3D11_USAGE_DEFAULT,
        0,
        m_sampleCount
    );

    ThrowIfFailed(m_device->CreateTexture2D(
        &renderTargetDesc,
        nullptr,
        m_msaaRenderTarget.ReleaseAndGetAddressOf()
    ));

    SetDebugObjectName(m_msaaRenderTarget.Get(), "MSAA Render Target");

    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS, m_backBufferFormat);

    ThrowIfFailed(m_device->CreateRenderTargetView(
        m_msaaRenderTarget.Get(),
        &renderTargetViewDesc,
        m_renderTargetView.ReleaseAndGetAddressOf()
    ));

    SetDebugObjectName(m_renderTargetView.Get(), "MSAA Render Target");

    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
    {
        // Create an MSAA depth stencil view
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            m_depthBufferFormat,
            static_cast<UINT>(width),
            static_cast<UINT>(height),
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL,
            D3D11_USAGE_DEFAULT,
            0,
            m_sampleCount
        );

        ThrowIfFailed(m_device->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            m_msaaDepthStencil.ReleaseAndGetAddressOf()
        ));

        SetDebugObjectName(m_msaaDepthStencil.Get(), "MSAA Depth/Stencil");

        CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS, m_depthBufferFormat);

        ThrowIfFailed(m_device->CreateDepthStencilView(
            m_msaaDepthStencil.Get(),
            &dsvDesc,
            m_depthStencilView.ReleaseAndGetAddressOf()
        ));

        SetDebugObjectName(m_depthStencilView.Get(), "MSAA Depth/Stencil");
    }

    m_width = width;
    m_height = height;
}


void MSAAHelper::ReleaseDevice()
{
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_msaaDepthStencil.Reset();
    m_msaaRenderTarget.Reset();

    m_device.Reset();

    m_width = m_height = 0;
}


void MSAAHelper::Resolve(ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer)
{
    context->ResolveSubresource(backBuffer, 0, m_msaaRenderTarget.Get(), 0, m_backBufferFormat);
}


void MSAAHelper::SetWindow(const RECT& output)
{
    // Determine the render target size in pixels.
    auto width = size_t(std::max<LONG>(output.right - output.left, 1));
    auto height = size_t(std::max<LONG>(output.bottom - output.top, 1));

    SizeResources(width, height);
}
```

# Example

In your **Game.h** header file, add:

```cpp
#include "MSAAHelper.h"
```

And then add a variable declaration to the private section of your **Game** class:

```cpp
std::unique_ptr<DX::MSAAHelper> m_msaaHelper;
```

For MSAA rendering, you typically use a MSAA depth/stencil buffer. Therefore, you should create the [[DeviceResources]] instance without a depth buffer (which is non-MSAA) by passing ``DXGI_FORMAT_UNKNOWN`` for the depth-buffer format. In the **Game** constructor modify:

```cpp
    // Depth-buffer managed by MSAAHelper.
    m_deviceResources = std::make_unique<DX::DeviceResources>(
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT_UNKNOWN);
    m_deviceResources->RegisterDeviceNotify(this);
```

Then add (where ``MSAA_COUNT`` is your target sample count)

```
    m_msaaHelper = std::make_unique<DX::MSAAHelper>(
        m_deviceResources->GetBackBufferFormat(),
        DXGI_FORMAT_D32_FLOAT,
        MSAA_COUNT);
```

In the **CreateDeviceDependentResources** method, add:

```cpp
// Set the MSAA device. Note this updates GetSampleCount.
m_msaaHelper->SetDevice(device);
```

In the **CreateWindowSizeDependentResources** method, call:

```cpp
auto size = m_deviceResources->GetOutputSize();

// Set window size for MSAA.
m_msaaHelper->SetWindow(size);
```

Since you are rendering to the MSAA render target & depth/stencil buffer rather than to the standard DeviceResources instances, update your **Clear** method as follows:

```cpp
// Clear the views.
...

auto renderTarget = m_msaaHelper->GetMSAARenderTargetView();
auto depthStencil = m_msaaHelper->GetMSAADepthStencilView();
```

The **Render** function should work as written, but the Present call will need updated:

```cpp
// Show the new frame.
m_msaaHelper->Resolve(context, m_deviceResources->GetRenderTarget());
m_deviceResources->Present();
```

Be sure to add to your **OnDeviceLost**:

```cpp
// Release MSAA resources.
m_msaaHelper->ReleaseDevice();
```

# Remarks

This helper class uses the 'default' quality for simplicity.

If the requested sample count isn't supported, it will use the largest value it can. For example, if you request 8, but the device only supports 4 it will use 4x.


