In [[the basic game loop]] lesson, we made use of the **Direct3D Game** VS template which has all the relevant Direct3D code in the ``Game`` class including creating the device and swap chain. This makes it easy to see how to create the device & swapchain. This, however, results in a lot of 'boiler-plate' code in the main ``Game`` class which is distracting for the remainder of our tutorial lessons.

There is therefore a "DR" variant of each of the Direct3D Game VS template in the [VS 2017/2019](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DUWPGame.vsix) VSIX package which adds [[DeviceResources]].

> The standard Universal Windows Platform app ``DirectXApp`` VS templates make use of a similar ``DeviceResources`` abstraction.

# Creating a new project
## Visual Studio 2017
* From the drop-down menu, select **File** and then **New** -> **Project...**
* Select "Visual C++" on the left-hand tree-view.
* Select "Direct3D Win32 Game DR"  or "Direct3D UWP Game DR".
* Fill in the "Name" and "Location" fields as desired.
* Optional: Uncheck "Create directory for solution" to keep the directory structure as bit flatter.
* Select "OK".

![Create New Project](https://github.com/Microsoft/DirectXTK/wiki/images/CreateNewProjectDR.PNG)

## Visual Studio 2019
* From the drop-down menu, select **File** and then **New** -> **Project...** or on the startup dialog select **Create a new project**
* Select "Games" on the project types filter. You can optionally type "Win32" or "UWP" in the search box as well.
* Select "Direct3D Win32 Game" or "Direct3D UWP Game".
* Select "Next"
* Fill in the "Project name" and "Location" fields as desired.
* Optional: Check "Place solution and project in the same directory" to keep the directory structure as bit flatter.
* Select "Create".

![Create New Project](https://github.com/Microsoft/DirectXTK/wiki/images/CreateNewProjectDR2019.png)

## Windows 10 SDK Selection
The templates use the Windows 10 SDK, so the project wizard will trigger this dialog to select the Windows 10 SDK version to use. Note that for Win32 Windows desktop apps, the "Minimum version" setting is ignored.

![Windows 10 SDK Selection](https://github.com/Microsoft/DirectXTK/wiki/images/Windows10SDKSelect.png)

## Newly Created Project
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

The Win32 version will have the following files as well:

* Resources
  * directx.ico
  * resource.rc
  * settings.manifest

While the UWP version will have:

* Package.appxmanifest
* Name_TemporaryKey.pfx
* Assets
  * logo PNG files

## CMake projects

If you prefer to make use of VS 2019's integrated [CMake](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=vs-2019) support or a standalone install of CMake, there are ``CMakeLists.txt`` and ``CMakeSettings.json`` files available for download on [directx-vs-templates](https://github.com/walbourn/directx-vs-templates/wiki#cmake-projects).

The simplest way to use these is to clone *directx-vs-templates*, open up PowerShell, change to the ``directx-vs-templates/VSIX`` directory, and run the following script which will create a fresh instance of the template set up for CMake development:

```
.\createcmake.ps1 ../d3d11game_win32_dr Direct3DGame $Env:USERPROFILE\source\Direct3DGameDR
```

## COM

The Win32 and UWP templates ensure that COM is initialized. This is required for *DirectX Tool Kit* when using Windows Imaging Component ([WIC](https://docs.microsoft.com/en-us/windows/win32/wic/-wic-lh)) functionality. The UWP template also initialize the Windows Runtime, which is required to use ``Windows.Gaming.Input``.

# Running the application
Visual Studio will default to the _x64_ platform / _Debug_ configuration which builds an x64 (64-bit) application with debugging enabled. The template contains both _Debug_ and _Release_ configurations for both _x86_ (32-bit) and _x64_ (x64 native 64-bit) platforms, with UWP also including the ARM platforms.

Press F5 to build and run the application It displays the following window:

![Running Project](https://github.com/Microsoft/DirectXTK/wiki/images/RunningProject.png)

> _Troubleshooting:_ If the base template fails to start, there are a few possibilities. First, if your system
> doesn't have any Direct3D capable device of any feature level, it will fail. This is pretty unlikely on modern
> versions of Windows. Second if it runs fine in _Release_ but fails in _Debug_, then you likely do not have the
> [proper DirectX Debug Device](https://walbourn.github.io/direct3d-sdk-debug-layer-tricks/) installed for your operating system. Finally, if you are on Windows 7, you need to have the [KB2670838](https://walbourn.github.io/directx-11-1-and-windows-7-update/) installed.

> **ARM64**: With the ARM64 compiler installed targeting a Windows 10 on ARM64 device such as a *Microsoft Surface X*, you can build using the ARM64 platform for desktop as well.

# Tour of the code

## Constructor

The Game class constructor is where you can do first initialization of member variables, as well as where we create the DeviceResources instance.

```cpp
Game::Game() noexcept(false)
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

The ``minFeatureLevel`` defaults to 10 for PC and 9.3 for UWP. See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-downlevel-intro) for the definitions of Direct3D hardware feature levels. You can specify a higher hardware level if you want to take a hard dependency on additional capabilities.

If you want to make use of [[DebugEffect]], [[DGSLEffect]], [[NormalMapEffect]], [[PBREffect]], [[PostProcess]], or dual-parabolic environment maps then you need ``D3D_FEATURE_LEVEL_10_0`` or higher:

```cpp
m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_D32_FLOAT, 2, D3D_FEATURE_LEVEL_10_0);
```

Finally, remember that the choice of ``minFeatureLevel`` also impacts which ``backBufferFormat`` and ``depthBufferFormat`` values you can rely on being supported per [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-downlevel-intro). For example, to use 9.x feature levels, you must use ``DXGI_FORMAT_D24_UNORM_S8_UINT`` or ``DXGI_FORMAT_D16_UNORM`` for the depth/stencil format instead of the default value of ``DXGI_FORMAT_D32_FLOAT``.

```cpp
m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_D24_UNORM_S8_UINT, 2, D3D_FEATURE_LEVEL_9_1);
```

> There is one more defaulted parameter for enabling additional features such as such as [variable refresh rate](https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/variable-refresh-rate-displays) or [HDR10](https://docs.microsoft.com/en-us/windows/win32/direct3darticles/high-dynamic-range) output. See [[DeviceResources]] for details and usage.

## Initialize
When the application first starts, execution is passed to the **Initialize** method. The TODO here by default leaves the applications [[StepTimer]] in the 'variable length' mode. You uncomment the code if you want **StepTimer** in the 'fixed-step' mode. We'll explain this more once we get to ``Update``.

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

### Present

The ``DeviceResources::Present`` method presents the swapchain and checks for "device removed" scenarios.

## Events
The template includes a number of message handlers that are called for process state changes: **OnActivated**, **OnDeactivated**, **OnSuspending**, **OnResuming**, and **OnWindowSizeChanged**. The UWP version also includes **ValidateDevice**, and display orientation is provided long with the window size.

> For Win32 desktop, the **OnSuspending** / **OnResuming** messages are triggered when (a) the window is minimized/unminimized or (b) in reaction to the ``WM_POWERBROADCAST`` message. On other platforms, this is driven by Process Lifecycle Management ([PLM](https://docs.microsoft.com/en-us/windows/uwp/launch-resume/app-lifecycle)).

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
We make use of the ``Microsoft::WRL::ComPtr`` smart-pointer for managing the lifetime of the Direct3D 11 COM objects, which is why we make use of ``.Get()`` in the code above. See [ComPtr](https://github.com/Microsoft/DirectXTK/wiki/ComPtr) and [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/prog-dx-with-com) for more information and usage.

# Error handling
Many Direct3D functions return an ``HRESULT`` which is the standard for COM APIs. For robustness and easier debugging, it is important that you always check the result of every function that return an ``HRESULT``. If you really can safely assume there is no error condition for a particular function, the function itself will return ``void`` instead of ``HRESULT``.

The Win32 game template makes use of the helper function [[ThrowIfFailed]] in the ``DX`` C++ namespace declared in ``pch.h``. This is the same helper that is used by the Windows Store and Windows phone VS templates. This helper throws a C++ exception if the standard ``FAILED`` macro returns true for a given ``HRESULT``. This is used for [fail fast](https://en.wikipedia.org/wiki/Fail-fast) error handling.

```cpp
DX::ThrowIfFailed(device->CreateTexture2D(&depthStencilDesc,
    nullptr, &depthStencil));
```

> Do not use ``hr == S_OK`` to check for success. Use ``SUCCEEDED(hr)`` instead.

The DR VS template variants include the enhanced version of [[ThrowIfFailed]].

# Gamma

The DeviceResources implementation supports using ``DXGI_FORMAT_*_SRGB`` formats for the backbuffer using the method compatible with modern and legacy swap effects. See [The Care and Feeding of Modern Swap Chains](https://walbourn.github.io/care-and-feeding-of-modern-swapchains/) for more details.

# Fullscreen

The Win32 desktop and UWP templates implement [immersive fullscreen](https://walbourn.github.io/care-and-feeding-of-modern-swap-chains-3/). You can toggle this using the traditional hotkey ``ALT+Enter``. If you want to default to full-screen at startup, see the ``TODO`` comments in ``Main.cpp``.

**Next lesson**: [[Adding the DirectX Tool Kit]]

# Further reading
[Direct3D Win32 Game Visual Studio template](https://walbourn.github.io/direct3d-win32-game-visual-studio-template/)  
[Direct3D Game Visual Studio templates (Redux)](https://walbourn.github.io/direct3d-game-visual-studio-templates-redux/)  
[Anatomy of Direct3D 11 Create Device](https://walbourn.github.io/anatomy-of-direct3d-11-create-device/)   
[Manifest Madness](https://aka.ms/I6kdnw)  
[64-bit programming for Game Developers](https://docs.microsoft.com/en-us/windows/desktop/DxTechArts/sixty-four-bit-programming-for-game-developers)
