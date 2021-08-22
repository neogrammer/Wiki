This provides a helper class for managing an offscreen render target.

[RenderTexture.h](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.h)

```cpp
class RenderTexture
{
public:
    RenderTexture(DXGI_FORMAT format);

    void SetDevice(ID3D11Device* device);

    void SizeResources(size_t width, size_t height);

    void ReleaseDevice();

    void SetWindow(const RECT& rect);

    ID3D11Texture2D* GetRenderTarget() const { return m_renderTarget.Get(); }
    ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView.Get(); }
    ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }

    DXGI_FORMAT GetFormat() const { return m_format; }

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTarget;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

    DXGI_FORMAT m_format;

    size_t m_width;
    size_t m_height;
};
```

[RenderTexture.cpp](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.cpp)

```cpp
#include "RenderTexture.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

RenderTexture::RenderTexture(DXGI_FORMAT format) :
    m_format(format),
    m_width(0),
    m_height(0)
{
}

void RenderTexture::SetDevice(ID3D11Device* device)
{
    if (device == m_device.Get())
        return;

    if (m_device)
    {
        ReleaseDevice();
    }

    {
        UINT formatSupport = 0;
        if (FAILED(device->CheckFormatSupport(m_format, &formatSupport)))
        {
            throw std::runtime_error("CheckFormatSupport");
        }

        UINT32 required = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET;
        if ((formatSupport & required) != required)
        {
            throw std::runtime_error("RenderTexture");
        }
    }

    m_device = device;
}


void RenderTexture::SizeResources(size_t width, size_t height)
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

    // Create a render target
    CD3D11_TEXTURE2D_DESC renderTargetDesc(
        m_format,
        static_cast<UINT>(width),
        static_cast<UINT>(height),
        1, // The render target view has only one texture.
        1, // Use a single mipmap level.
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
        D3D11_USAGE_DEFAULT,
        0,
        1
    );

    ThrowIfFailed(m_device->CreateTexture2D(
        &renderTargetDesc,
        nullptr,
        m_renderTarget.ReleaseAndGetAddressOf()
    ));

    // Create RTV.
    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_format);

    ThrowIfFailed(m_device->CreateRenderTargetView(
        m_renderTarget.Get(),
        &renderTargetViewDesc,
        m_renderTargetView.ReleaseAndGetAddressOf()
    ));

    // Create SRV.
    CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, m_format);

    ThrowIfFailed(m_device->CreateShaderResourceView(
        m_renderTarget.Get(),
        &shaderResourceViewDesc,
        m_shaderResourceView.ReleaseAndGetAddressOf()
    ));

    m_width = width;
    m_height = height;
}


void RenderTexture::ReleaseDevice()
{
    m_renderTargetView.Reset();
    m_shaderResourceView.Reset();
    m_renderTarget.Reset();

    m_device.Reset();

    m_width = m_height = 0;
}

void RenderTexture::SetWindow(const RECT& output)
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
#include "RenderTexture.h"
```

And then add a variable declaration to the private section of your **Game** class:

```cpp
std::unique_ptr<DX::RenderTexture> m_renderTexture;
```

In the **Game** constructor add:

```cpp
m_renderTexture = std::make_unique<DX::RenderTexture>(
    m_deviceResources->GetBackBufferFormat());
```

> For this example, our render texture is the same size & format as the swapchain, and we are using the depth/stencil buffer created by [[DeviceResources]].

In the **CreateDeviceDependentResources** method, add:

```cpp
m_renderTexture->SetDevice(device);
```

In the **CreateWindowSizeDependentResources** method, call:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_renderTexture->SetWindow(size);
```

Be sure to add to your **OnDeviceLost**:

```cpp
m_renderTexture->ReleaseDevice();
```

Rendering to the offscreen rendering texture, update the **Clear** method as follows:

```cpp
// Clear the views.
...
auto renderTarget = m_renderTexture->GetRenderTargetView();
```

In the **Render** function, our scene is rendered to the offscreen texture. To see it, we need to render using it to the swapchain. For example, you could use a [[SpriteBatch]]:

```cpp
Clear();

// Render scene (which will go to m_renderTexture due to our changes above)

...

auto renderTarget = m_deviceResources->GetRenderTargetView();
context->OMSetRenderTargets(1, &renderTarget, nullptr);

m_spriteBatch->Begin();

m_spriteBatch->Draw(m_renderTexture->GetShaderResourceView(),
    m_deviceResources->GetOutputSize());

m_spriteBatch->End();

ID3D11ShaderResourceView* nullsrv[] = { nullptr };
context->PSSetShaderResources(0, 1, nullsrv);

// Show the new frame.
m_deviceResources->Present();
```

If using a Direct3D Hardware Feature Level 10.0 or better device, you can use [[PostProcess]]:

```cpp
Clear();

// Render scene (which will go to m_renderTexture due to our changes above)

...

auto renderTarget = m_deviceResources->GetRenderTargetView();
context->OMSetRenderTargets(1, &renderTarget, nullptr);

m_basicPostProcess->SetSourceTexture(m_renderTexture->GetShaderResourceView());
m_basicPostProcess->SetEffect(BasicPostProcess::Copy);
m_basicPostProcess->Process(context);

ID3D11ShaderResourceView* nullsrv[] = { nullptr };
context->PSSetShaderResources(0, 1, nullsrv);

// Show the new frame.
m_deviceResources->Present();
```

# Applications

* The primary use of **RenderTexture** is for post-processing and/or tone-mapping the scene before presenting for viewing.

* In addition to post-processing and tone-mapping, you can use **RenderTexture** to resize and/or format-convert textures with the GPU. Note that you will need to call **SetViewport** to the target size of the render texture before rendering.

# Remarks

The **SetWindow** method is a simple wrapper for **SizeResources** which makes it easier to use with ``RECT`` like ``m_deviceResources->GetOutputSize();``. For specific sizes, you can just call **SizeResources** directly.
