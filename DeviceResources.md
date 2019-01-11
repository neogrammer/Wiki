In [[the basic game loop]] lesson and other tutorials, we are making use of the **Direct3D 11 Game** VS template which has all the relevant Direct3D code in the ``Game`` class including creating the device and swap chain. This makes it easy to teach with, and for the reader to see all the code in one place. This, however, does result in a lot of 'boiler-plate' code in the main ``Game`` class which could be distracting in larger projects & samples.

There is therefore a "DR" variant of each of the Direct3D Game VS templates in the [VS 2015 / VS 2017](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DUWPGame.vsix) VSIX package.

> The standard Universal Windows Platform app, Windows 8 Store DirectX, and Windows phone 8 VS templates make use of a similar ``DeviceResources`` abstraction.

For the DirectX 12 version, see [DeviceResources](https://github.com/Microsoft/DirectXTK12/wiki/DeviceResources)

# Creating a new project
* From the drop-down menu, select **File** and then **New** -> **Project...**
* Select "Visual C++" on the left-hand tree-view.
* Select "Direct3D Win32 Game DR".
* Fill in the "Name" and "Location" fields as desired.
* Optional: Uncheck "Create directory for solution" to keep the directory structure as bit flatter.
* Select "OK".

![Create New Project](https://github.com/Microsoft/DirectXTK/wiki/images/CreateNewProjectDR.PNG)

The newly created project contains the following files:

![Open Game Cpp](https://github.com/Microsoft/DirectXTK/wiki/images/OpenGameCppDR.PNG)

* Precompiled header files
  * pch.h
  * pch.cpp
* Main application entry-point and classic Windows procedure function
  * Main.cpp
* Device resources abstraction
  * DeviceResources.h
  * DeviceResources.cpp
* [[Timer|StepTimer]] helper class
  * StepTimer.h
* Resources
  * directx.ico
  * resource.rc
  * settings.manifest
* The Game class
  * Game.h
  * Game.cpp

# Running the application

Visual Studio will default to the _Win32_ platform / _Debug_ configuration which builds an x86 (32-bit) application with debugging enabled. The template contains both _Debug_ and _Release_ configurations for both _Win32_ and _x64_ (x64 native 64-bit) platforms.

Press F5 to build and run the application It displays the following window:

![Running Project ](https://github.com/Microsoft/DirectXTK/wiki/images/RunningProject.png)

> _Troubleshooting:_ If the base template fails to start, there are a few possibilities. First, if your system
> doesn't have any Direct3D capable device of any feature level, it will fail. This is pretty unlikely on modern
> versions of Windows. Second if it runs fine in _Release_ but fails in _Debug_, then you likely do not have the
> [proper DirectX Debug Device](http://blogs.msdn.com/b/chuckw/archive/2012/11/30/direct3d-sdk-debug-layer-tricks.aspx) installed for your operating system.

# Tour of the code

## Constructor

The Game class constructor is where you can do first initialization of member variables, as well as where we create the DeviceResources instance.

```cpp
Game::Game()
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}
```

The DeviceResources constructor takes a number of defaulted parameters to control ``backBufferFormat``, ``depthBufferFormat``, ``backBufferCount``, ``minFeatureLevel``, and option ``flags``. You can provide specific values to change them as needed.

If doing _gamma-correct rendering_, you should use ``DXGI_FORMAT_*_UNORM_SRGB`` or a supported HDR format for the ``backBufferFormat``. Be sure to update ``Clear`` below accordingly to use a linear clear color.

```cpp
// Use gamma-correct rendering.
m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB);
```

If you do not want DeviceResources to create a depth/stencil buffer, you can use ``DXGI_FORMAT_UNKNOWN`` for ``depthBufferFormat``. This is useful for 2D only rendering or when doing MSAA on Universal Windows Platform (UWP) apps which requires handling your own render target and depth buffer with ``Sample.Count`` > 1. Be sure to update ``Clear`` below to avoid referencing a null depth buffer object.

```cpp
// Renders only 2D, so no need for a depth buffer.
m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_UNKNOWN);
```

The ``backBufferCount`` defaults to 2, but in some cases you may want to override it to use 3. Larger numbers would be quite unusual and are not recommended.

The ``minFeatureLevel`` defaults to 9.1 for PC and 9.3 for UWP. You can specify a higher hardware level if you want to take a hard dependency on additional capabilities. If you want to make use of [[PostProcess]] then you should specify ``D3D_FEATURE_LEVEL_10_0`` or higher.

The ``flags`` options parameter defaults to 0 which is no flags. You can specify one or more of them as or'd values:

* ``c_AllowTearing``: If supported by the system, this enables present 'tearing' for supporting G-Sync/FreeSync displays.
* ``c_EnableHDR``: If supported by the system, this enables rendering to wide-color gamut displays. See [below](https://github.com/Microsoft/DirectXTK/wiki/DeviceResources#wide-gamut-hdr-rendering) for more information.

## Initialize
When the application first starts, execution is passed to the **Initialize** method. The TODO here by default leaves the applications [[StepTimer]] in the 'variable length' mode. You uncomment the code if you want StepTimer in the 'fixed-step' mode. We'll explain this more once we get to ``Update``.

```cpp
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}
```

The first Game method ``Initialize`` calls is **CreateDeviceDependentResources** for the creation of objects that depend on the device or context, but do not care about the size of the rendering window.

```cpp
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
    device; // This is only here to avoid a warning. You can remove it once you make use of device!
}
```

> Instead of using the class variable ``m_d3dDevice`` we have to obtain the device interface from the ``DeviceResources`` object. See ``Render`` for how you get the device context from ``DeviceResources``.

The second Game method ``Initialize`` calls is **CreateWindowSizeDependentResources** for creation of objects that depend on the size of the rendering window. Note that this function could be creating these objects for the first time, it could be re-creating already existing objects due to a window-size change, or could be creating 'fresh' objects after a Direct3D device-removed or device-reset case.

```cpp
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}
```

## Update
The **Update** method is intended to handle game-world state modification which is typically driven by time passing, simulation, and/or user-input. By default, ``Update`` is called once per 'frame' and can have an arbitrary delta-time. This is called a 'variable-step' mode.

If in the ``Initialize`` method above you uncomment the TODO code, then each ``Update`` will be for a fixed time-step (1/60th of a second), with ``Update`` called as many time in a single 'frame' as needed to keep it up-to-date. This is called a 'fixed-step' mode and potentially be more stable for many kinds of simulations.

```cpp
// Updates the world
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here
    elapsedTime;
}
```

## Render
The **Render** function which should render a single 'frame' of the scene, which starts by a ``Clear`` of the render targetg and setting the rendering viewport. It ends with a call to ``DeviceResources::Present`` to show the rendered frame.

```cpp
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context; // This is only here to avoid a warning. You can remove it once you make use of context!

    m_deviceResources->Present();
}
```

> Instead of using the class variable ``m_d3dContext`` we have to obtain the device context interface from the ``DeviceResources`` object. See ``CreateDeviceDependentResources`` for how you get the device from ``DeviceResources``.

### Clear
The **Clear** method defaults to a background color of the classic "Cornflower blue".

```cpp
void Game::Clear()
{
    // Clear the views
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);
}
```

If you are using _gamma-correct rendering_ with a sRGB or HDR backbuffer format, you need to ensure you are using a linear RGB clear color. DirectXMath colors are defined in sRGB colorspace since they are [.NET color constants](https://docs.microsoft.com/en-us/dotnet/api/system.drawing.color), so you need to replace ``ClearRenderTargetView`` in **Clear** with:

```cpp
// Use linear clear color for gamma-correct rendering.
XMVECTORF32 color;
color.v = XMColorSRGBToRGB(Colors::CornflowerBlue);
context->ClearRenderTargetView(renderTarget, color);
```

If you chose to not have DeviceResources create a depth-stencil buffer (see the _Constructor_ section), you need to update **Clear** to avoid reference to a null depth buffer object.

```cpp
void Game::Clear()
{
    // Clear the views
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->OMSetRenderTargets(1, &renderTarget, nullptr);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);
}
```

## Events
The template includes a number of message handlers that are called for process state changes: **OnActivated**, **OnDeactivated**, **OnSuspending**, **OnResuming**, and **OnWindowSizeChanged**.

Since we are using [[ComPtr]], most cleanup is automatic when the Game class is destroyed. If ``Present`` encounters a device-removed or device-reset, then the application needs to release all Direct3D objects and recreate the device, swapchain, and all Direct3D objects again. Therefore, the TODO in **OnDeviceLost** should be updated to release your application's Direct3D objects.

```cpp
void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here

...
}
```

> You will not get "device lost" all that often. In legacy Direct3D 9, you would routinely get a 'device lost' if you just ALT+TAB away from the application because the GPU used to be an 'exclusive' rather than 'shared' resource. The situation where you'd get ``DXGI_ERROR_DEVICE_RESET`` is if the driver crashes or the video hardware hangs. You get ``DXGI_ERROR_DEVICE_REMOVED`` if a new driver is installed while your application is running, or if you are running on a 'GPU is in the dock' style laptop and the laptop is undocked. You can test this case by opening the *Developer Command Prompt for Visual Studio* as an administrator, and typing ``dxcap -forcetdr`` which will immediately cause all currently running Direct3D apps to get a ``DXGI_ERROR_DEVICE_REMOVED`` event.

# Smart-pointer
We make use of the ``Microsoft::WRL::ComPtr`` smart-pointer for managing the lifetime of the Direct3D 11 COM objects. See [[ComPtr]] for more information and usage.

# Error handling
Many Direct3D functions return an ``HRESULT`` which is the standard for COM APIs. For robustness and easier debugging, it is important that you always check the result of every function that return an ``HRESULT``. If you really can safely assume there is no error condition for a particular function, the function itself will return ``void`` instead of ``HRESULT``.

The Win32 game template makes use of the helper function [[ThrowIfFailed]] in the ``DX`` C++ namespace. This is the same helper that is used by the Windows Store and Windows phone VS templates. This helper throws a C++ exception if the standard ``FAILED`` macro returns true for a given ``HRESULT``.

```cpp
DX::ThrowIfFailed(device->CreateTexture2D(&depthStencilDesc,
    nullptr, &depthStencil));
```

> Do not use ``hr == S_OK`` to check for success. Use ``SUCCEEDED(hr)`` instead.

The DR VS template variants include the enhanced version of [[ThrowIfFailed]].

# Device Resources

The tour above has made use of the following accessors:

* **GetD3DDevice**: Returns the Direct3D device.
* **GetD3DDeviceContext**: Returns the Direct3D device context.
* **GetRenderTargetView**: Returns the render target view for the swap chain back-buffer.
* **GetDepthStencilView**: Returns the depth/stencil buffer created for the back-buffer.
* **GetScreenViewport**: Returns a viewport for the swap chain back-buffer.

The **DeviceResources** class also includes a number of useful assessors not already used above:

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
The **DeviceResources** implementation is designed to support Windows Vista SP2 and Windows 7 RTM with DirectX 11.0, but also supports Direct3D 11.1 on [Windows 7 SP 1](http://blogs.msdn.com/b/chuckw/archive/2012/11/14/directx-11-1-and-windows-7.aspx) or Windows 8 which provides [significant improvements](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/direct3d-11-1-features) such as simplified interop with Direct2D/DirectWrite. Therefore, you should generally prefer to use **GetD3DDevice** / **GetD3DDeviceContext** / **GetSwapChain**, but where you explicitly need 11.1 features you call **GetD3DDevice1** / **GetD3DDeviceContext1** / **GetSwapChain1**. These will be nullptr if the system only has the DirectX 11.0 Runtime.

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

# Tutorial series
You can use the DR variant of the templates in the tutorial lessons making the following adjustments:

When directed to add something to ``CreateDevice``, add it to ``CreateDeviceDependentResources`` instead.

When asked to add something to ``CreateResources``, add it to ``CreateWindowSizeDependentResources`` instead.

Anywhere you are asked to use ``m_d3dDevice.Get()``, use ``m_deviceResources->GetD3DDevice()`` instead:

```cpp
auto device = m_deviceResources->GetD3DDevice();
m_states = std::make_unique<CommonStates>(device);
```

Anywhere you are asked to use ``m_d3dContext.Get()``, use ``m_deviceResources->GetD3DDeviceContext()`` instead:

```cpp
auto context = m_deviceResources->GetD3DDeviceContext();
m_spriteBatch = std::make_unique<SpriteBatch>(context);
```

When asked to use ``backBufferWidth`` or ``backBufferHeight``, use ``m_deviceResources->GetOutputSize()`` instead:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_screenPos.x = size.right / 2.f;
m_screenPos.y = size.bottom / 2.f;
```

When asked to use ``backBufferCount``, use ``m_deviceResources->GetBackBufferCount()`` instead.

**Next lesson**: [[Adding the DirectX Tool Kit]]

# Wide-gamut HDR rendering

If the ctor is created with the ``c_EnableHDR`` option flag, then the **GetColorSpace** property (on PC/UWP) needs to be used to determine if the swapchain is currently using sRGB (``DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709``), Linear (``DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709``), or HDR10 (``DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020``) values. This can change whenever the window is moved or the HDR mode of the TV is modified, so should be checked each frame.

> The HDR display support requires the Windows 10 Creators Update (15063) and must be built with the Windows 10 Creators Update SDK (15063).

The ``backBufferFormat`` for PC/UWP should be ``DXGI_FORMAT_R10G10B10A2_UNORM`` for rendering in HDR10 or ``DXGI_FORMAT_R16G16B16A16_FLOAT`` for rendering linear float values. The [[ToneMapPostProcess]] class supports the required color transformations for preparing an HDR10 signal, or tone-mapping for non-HDR displays.

> For _Xbox One_, the ``backBufferFormat`` is actually the GameDVR SDR swapchain format which can be any 8:8:8:8 or 10:10:10:2 UNORM format. The HDR swapchain in this case is always ``DXGI_FORMAT_R10G10B10A2_UNORM`` using HDR10. You would typically use [[ToneMapPostProcess]] with the MRT mode enabled to generate both HDR10 and SDR signals at the same time.

See the tutorial [[Using HDR rendering]] for example usage.

# Further reading
[Direct3D Win32 Game Visual Studio template (Redux)](http://blogs.msdn.com/b/chuckw/archive/2015/12/17/direct3d-game-visual-studio-templates-redux.aspx)  
[Anatomy of Direct3D 11 Create Device](http://blogs.msdn.com/b/chuckw/archive/2014/02/05/anatomy-of-direct3d-11-create-device.aspx)  
[Visual Studio 2013 Windows Store DirectX Templates](http://blogs.msdn.com/b/ianhu/archive/2014/03/07/visual-studio-2013-windows-store-directx-templates.aspx)  

[Linear-Space Lighting (i.e. Gamma)](http://filmicworlds.com/blog/linear-space-lighting-i-e-gamma/)   
[Chapter 24. The Importance of Being Linear](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch24.html), GPU Gems 3   
[Gamma-correct rendering](https://blog.molecular-matters.com/2011/11/21/gamma-correct-rendering/)   
