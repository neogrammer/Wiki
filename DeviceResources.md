The DeviceResources class is an abstraction for the device & swapchain, and optionally a depth buffer. It's used in a number of DirectX Visual Studio Templates and samples as it simplifies the code by moving 'boilerplate' to a distinct set of files.

For the DirectX 12 version, see [DeviceResources](https://github.com/Microsoft/DirectXTK12/wiki/DeviceResources)

**Related tutorial:** [[Using DeviceResources]]

# Header
```cpp
#include "DeviceResources.h"
```

# Interfaces

The Win32 and UWP versions both implement an abstract interface to simplify handling of "lost device" scenarios.

```cpp
interface IDeviceNotify
{
    virtual void OnDeviceLost() = 0;
    virtual void OnDeviceRestored() = 0;
};
```

The ``Game`` class derives from this interface, and you should call **RegisterDeviceNotify** to provide the instance to the DR abstraction.

> Xbox One XDK applications do not encounter "device removed" or "lost device" scenarios, so that version of DeviceResources does not have this interface.

# Initialization

Typically the DR is used as follows:

```cpp
Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    // Create device-dependent resources

    m_deviceResources->CreateWindowSizeDependentResources();
    // Create windows-size dependent resourcdes
}
```

# Methods

*TODO*

# Accessors

* **GetD3DDevice**: Returns the Direct3D device.
* **GetD3DDeviceContext**: Returns the Direct3D device context.
* **GetRenderTargetView**: Returns the render target view for the swap chain back-buffer.
* **GetDepthStencilView**: Returns the depth/stencil buffer created for the back-buffer.
* **GetScreenViewport**: Returns a viewport for the swap chain back-buffer.

* **GetOutputSize**: Returns the output size of the presentation window associated with the device.
* **GetSwapChain**: Returns the DXGI swap chain interface.
* **GetDeviceFeatureLevel**: Returns the Direct3D hardware feature level in use.
* **GetBackBufferFormat**: Returns the format of the render target returned by ``GetRenderTargetView``.
* **GetDepthBufferFormat**: Returns the format of the depth/stencil buffer returned by ``GetDepthStencilView``.
* **GetBackBufferCount**: Returns the number of back-buffers in the swap chain.
* **GetRenderTarget**, **GetDepthStencil**: Returns the texture objects for the swap chain & depth/stencil buffer.
* **GetDeviceOptions**: Returns the device options based on the ctor flags. If the system does not support the option, the flag will be cleared.

# Threading model

The DeviceResources class methods are intended to be called from the main presenting thread, and not from other threads. The device context associated with DeviceResources is the immediate context, and is intended for use by this 'main' thread. Per the [DirectX Graphics Infrastructure (DXGI): Best Practices](https://docs.microsoft.com/en-us/windows/desktop/direct3darticles/dxgi-best-practices#multithreading_and_dxgi), this presenting thread should also be the same thread as the main window message processing.

# Platform notes

## Windows desktop apps
The **DeviceResources** implementation is designed to support Direct3D 11.1 on [Windows 7 Service Pack 1](https://walbourn.github.io/directx-11-1-and-windows-7/), Windows 8.x, or later which provides [significant improvements](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/direct3d-11-1-features) such as simplified interop with Direct2D/DirectWrite.

> Older versions of this DeviceResources supported Windows Vista Service Pack 2 and Windows 7 RTM with DirectX 11.0. The implementation has since been changed to require DirectX 11.1 to simplify the code. If you need details on the differences, see [this blog post](https://walbourn.github.io/anatomy-of-direct3d-11-create-device/)

## Universal Windows Platform apps
The UWP version of **DeviceResources** always uses [DirectX 11.3 interfaces](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/direct3d-11-3-features).

It includes **GetRotation** and **GetOrientationTransform3D** to simplify handling of display orientation.

```cpp
m_spritesBatch->SetRotation( m_deviceResources->GetRotation() );

Matrix perspectiveMatrix = Matrix::CreatePerspectiveFieldOfView(
    fovAngleY, aspectRatio, nearDist, farDist );
m_projection = perspectiveMatrix * m_deviceResources->GetOrientationTransform3D();
```

-or-

```cpp
XMMATRIX projection = XMMatrixPerspectiveFovLH(
    fovAngleY, aspectRatio, nearDist, farDist );
XMMATRIX orient = XMLoadFloat4x4( &m_deviceResources->GetOrientationTransform3D() );
projection *= orient;
```

This platform also uses two additional methods:

* **ValidateDevice** is called from the ``DisplayInformation::DisplayContentsInvalidated`` handler, which can trigger a 'device removed' event.
* **Trim** is called from the ``CoreApplication::Suspending`` handler, generally after calling ``ClearState`` on the device context.

> The version of DeviceResources in the Visual Studio Universal Windows platform "Direct3D 11 App" template is a little different than the one linked above, although the basic design is the same. The primary difference is that the "Direct3D 11 App" template includes Direct2D and explicit WIC factory handling. It also returns the render target view with ``GetBackBufferRenderTargetView`` instead of ``GetRenderTargetView``.

## Xbox One
The Xbox One XDK version of **DeviceResources** does not include the 'device lost' handling, and always uses a fixed back-buffer size. There is also a ``Prepare`` method for optional support of Direct3D 11.X Fast Semantics.

The DR version for Xbox One also uses ``DXGIX_SWAP_CHAIN_FLAG_QUANTIZATION_RGB_FULL`` rather than ``DXGIX_SWAP_CHAIN_MATCH_XBOX360_AND_PC``.

# Notes
Since the ``DeviceResources`` class is now in it's own file and no longer directly impacts the readability of the rest of the template, it has a few enhancements compared to the handling in non-DR templates.

* If the SDK Debug Layer is not present on the target system when running ``Debug`` configurations, it will automatically fallback to creating the device without debugging.
* The DR version always uses ``D3D11_CREATE_DEVICE_BGRA_SUPPORT`` which is required for Direct2D/DirectWrite interop if that's desired.
* If no hardware device is available, the DR version will fall back to using [WARP](https://docs.microsoft.com/en-us/windows/desktop/direct3darticles/directx-warp) in non-production builds as this is useful for debugging and validation.
* In ``Debug`` configurations, additional diagnostic messages are output to the debug window.
* Rather than always using the default Direct3D device, the DR version will filter out the _Microsoft Basic Render Driver_ adapter as this fallback software device is seldom acceptable performance for games.

# Source
* Direct3D 11 Win32 version: [DeviceResources.h](https://raw.githubusercontent.com/walbourn/directx-vs-templates/master/d3d11game_win32_dr/DeviceResources.h) / [DeviceResources.cpp](https://raw.githubusercontent.com/walbourn/directx-vs-templates/master/d3d11game_win32_dr/DeviceResources.cpp)
* Direct3D 11 UWP version: [DeviceResources.h](https://raw.githubusercontent.com/walbourn/directx-vs-templates/master/d3d11game_uwp_dr/DeviceResources.h) / [DeviceResources.cpp](https://raw.githubusercontent.com/walbourn/directx-vs-templates/master/d3d11game_uwp_dr/DeviceResources.cpp)
* Direct3D 11 Xbox One XDK version: [DeviceResources.h](https://github.com/walbourn/directx-vs-templates/blob/xbox/d3d11game_xdk_dr/DeviceResources.h) / [DeviceResources.cpp](https://github.com/walbourn/directx-vs-templates/blob/xbox/d3d11game_xdk_dr/DeviceResources.cpp)

# Wide-gamut HDR rendering

If the ctor is created with the ``c_EnableHDR`` option flag, then the **GetColorSpace** property (on PC/UWP) needs to be used to determine if the swapchain is currently using sRGB (``DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709``), Linear (``DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709``), or HDR10 (``DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020``) values. This can change whenever the window is moved or the HDR mode of the TV is modified, so should be checked each frame.

> The HDR display support requires the Windows 10 Creators Update (15063) and must be built with the Windows 10 Creators Update SDK (15063).

The ``backBufferFormat`` for PC/UWP should be ``DXGI_FORMAT_R10G10B10A2_UNORM`` for rendering in HDR10 or ``DXGI_FORMAT_R16G16B16A16_FLOAT`` for rendering linear float values. The [[ToneMapPostProcess]] class supports the required color transformations for preparing an HDR10 signal, or tone-mapping for non-HDR displays.

> For _Xbox One_, the ``backBufferFormat`` is actually the GameDVR SDR swapchain format which can be any 8:8:8:8 or 10:10:10:2 UNORM format. The HDR swapchain in this case is always ``DXGI_FORMAT_R10G10B10A2_UNORM`` using HDR10. You would typically use [[ToneMapPostProcess]] with the MRT mode enabled to generate both HDR10 and SDR signals at the same time.

See the tutorial [[Using HDR rendering]] for example usage.

# Further reading
[Direct3D Win32 Game Visual Studio template (Redux)](https://walbourn.github.io/direct3d-game-visual-studio-templates-redux/)  
[Anatomy of Direct3D 11 Create Device](https://walbourn.github.io/anatomy-of-direct3d-11-create-device/)  
[Visual Studio 2013 Windows Store DirectX Templates](http://blogs.msdn.com/b/ianhu/archive/2014/03/07/visual-studio-2013-windows-store-directx-templates.aspx)  

[Linear-Space Lighting (i.e. Gamma)](http://filmicworlds.com/blog/linear-space-lighting-i-e-gamma/)   
[Chapter 24. The Importance of Being Linear](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch24.html), GPU Gems 3   
[Gamma-correct rendering](https://blog.molecular-matters.com/2011/11/21/gamma-correct-rendering/)   
