|[[Getting Started]]|
|---|

The first lesson in the [[Getting Started]] guide is to create a basic game loop which creates a Direct3D device, swap chain, and update/render loop.

# Background

The basic rendering setup for Direct3D 11 consists of the following interface objects:

* The *Direct3D device* is the primary graphics COM interface for creating additional Direct3D resource objects. Each instance of the device is associated with a specific GPU, and in the case of these tutorials we always use the 'default' Direct3D device.

* The *immediate Direct3D device context* is the primary graphics COM interface for drawing. It's also used for mapping/un-mapping resources into a fixed memory location for CPU access such as loading graphics data.

> In older versions of Direct3D, the operations for creating objects and drawing were all on the same interface. With Direct3D 11, these were split into two distinct interfaces. The device is 'thread-safe' meaning you can use the same device instance at the same time across multiple threads. The device context is not thread-safe, meaning the application must ensure only a single thread uses that interface object at time.

* The *swap chain* is an interface object that manages 2 or more "back-buffers". These are render target resources where one of them is being displayed on the output monitor (which is referred to as the "front-buffer"), while the other is available for rendering the next frame for display. Each time ``Present`` is called, the buffers 'flip' (also known as "buffer rotation") so the next back-buffer is made the front-buffer by being displayed and the previous front-buffer is now available for reuse.

* A *render target view* interface is the object that tells Direct3D the current properties of the render target, i.e. the surface on which graphics output is written.

* The *depth buffer* is a graphics resource which contains a "z-buffer" used for Hidden Surface Removal (HSV). This is typically not used for 2D rendering but is essential for 3D rendering to ensure proper sorting of the drawn pixels. This resource can optionally have some space set aside for a *stencil buffer* which can be used for specialized rendering techniques such as generating shadows.

* A *depth/stencil view* interface is the object that tells Direct3D the current properties of the depth/stencil resource.

# Setup
Our starting point is to use the **Direct3D Win32 Game** or the **Direct3D <abbr title="Universal Windows Platform">UWP</abbr> Game** project template. Install the [VS 2019/2022](https://github.com/walbourn/directx-vs-templates/raw/main/VSIX/Direct3DUWPGame.vsix) VSIX on your development system, and then start (or restart) Visual Studio.

# Creating a new project

## Visual Studio 2019 / 2022
* From the drop-down menu, select **File** and then **New** -> **Project...** or on the startup dialog select **Create a new project**
* Select "Games" in the project types filter. You can optionally type "Win32" or "UWP" in the search box as well.
* Select "Direct3D Win32 Game" or "Direct3D UWP Game".
* Select "Next"
* Fill in the "Project name" and "Location" fields as desired.
* Optional: Check "Place solution and project in the same directory" to keep the directory structure as bit flatter.
* Select "Create".

![Create New Project](https://github.com/Microsoft/DirectXTK/wiki/images/CreateNewProject2019.png)

## Windows SDK Selection
The UWP templates use the Windows SDK, so the project wizard will trigger this dialog to select the Windows SDK version to use.

![Windows SDK Selection](https://github.com/Microsoft/DirectXTK/wiki/images/Windows10SDKSelect.png)

For Win32 projects, the default is to use the latest installed Windows SDK.

## Newly Created Project
The newly created project contains the following files:

![Open Game Cpp](https://github.com/Microsoft/DirectXTK/wiki/images/OpenGameCpp.png)

* Precompiled header files
  * pch.h
  * pch.cpp
* Main application entry-point and classic Windows procedure function or CoreWindow class
  * Main.cpp
* [[Timer|StepTimer]] helper class
  * StepTimer.h
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
  * logo <abbr title="Portable Network Graphics">PNG</abbr> files

## CMake projects

If you prefer to make use of Visual Studio's integrated [CMake](https://docs.microsoft.com/cpp/build/cmake-projects-in-visual-studio?view=vs-2019) support or a standalone install of CMake, there are ``CMakeLists.txt`` and ``CMakePresets.json`` files available for download on [directx-vs-templates](https://github.com/walbourn/directx-vs-templates/wiki#cmake-projects).

The simplest way to use these is to clone *directx-vs-templates*, open up PowerShell, change to the ``directx-vs-templates/VSIX`` directory, and run the following script which will create a fresh instance of the template set up for CMake development:

```
.\createcmake.ps1 d3d11game_win32 Direct3DGame $Env:USERPROFILE\source
```

## COM

The Win32 and UWP templates ensure that the <abbr title="Component Object Model">COM</abbr> library is initialized. This is required for *DirectX Tool Kit* when using Windows Imaging Component ([WIC](https://docs.microsoft.com/windows/win32/wic/-wic-lh)) functionality. The UWP template also initialize the Windows Runtime, which is required to use ``Windows.Gaming.Input``.

# Running the application
Visual Studio will default to the _x64_ platform / _Debug_ configuration which builds an x64 (64-bit) application with debugging enabled. The template contains both _Debug_ and _Release_ configurations for both _x86_ (32-bit) and _x64_ (x64 native 64-bit) platforms, with UWP also including the ARM/ARM64 platforms. The UWP template also initialize the Windows Runtime, which is required to use ``Windows.Gaming.Input``.

Press F5 to build and run the application It displays the following window:

![Running Project ](https://github.com/Microsoft/DirectXTK/wiki/images/RunningProject.png)

<details><summary><i>Click here for troubleshooting advice</i></summary>
<p>If the base template fails to start, there are a few possibilities:
<ul><li>First, if your system doesn't have any Direct3D capable device of any feature level, it will fail.
This is pretty unlikely on modern versions of Windows.</li>
<li>Second if it runs fine in <i>Release</i> but fails in <i>Debug</i>, then you likely do not have the
<a href="https://walbourn.github.io/direct3d-sdk-debug-layer-tricks/">proper DirectX Debug Device</a> installed
for your operating system.</li>
<li>Finally, if you are on Windows 7 Service Pack 1, you need to have <a href="https://walbourn.github.io/directx-11-1-and-windows-7-update/">KB2670838</a> installed for DirectX 11.1 support.</li></ul></p></details>

> **Xbox**: the background color may be slightly oversaturated. This is because the basic Xbox template uses a backBufferFormat of ``DXGI_FORMAT_B8G8R8A8_UNORM_SRGB``. The DirectXMath Colors values are defined using standard [sRGB](https://en.wikipedia.org/wiki/SRGB) colorspace which is slightly different. All the colors defines need to be adjusted slightly for the linear RGB colorspace (aka gamma correct rendering) via ``XMColorSRGBToRGB``.

> **ARM64**: With the ARM64 compiler installed targeting a Windows on ARM64 device such as a *Microsoft Surface X*, you can build using the ARM64 platform for desktop as well.

# Tour of the code
For now, we'll focus on the content of ``Game.cpp`` (which is open by default).

## Initialize
When the application first starts, execution is passed to the **Initialize** method. The TODO here by default leaves the applications [[StepTimer]] in the 'variable length' mode. You uncomment the code if you want **StepTimer** in the 'fixed-step' mode. We'll explain this more once we get to ``Update``.

```cpp
// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max( width, 1 );
    m_outputHeight = std::max( height, 1 );

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default
    // variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}
```

One of the two functions called by ``Initialize`` is the **CreateDevice** function which sets up a ([Direct3D 11.1](https://walbourn.github.io/anatomy-of-direct3d-11-create-device/)) device using the maximum [Direct3D Feature Level](https://aka.ms/Apsgrj) supported by the system which could range from 9.1 to 11.1. For _Debug_ builds, it enables the Direct3D "Debug device" which provides additional validation and diagnostic errors (seen in the "Output" window in Visual C++ when showing output from "Debug"). The TODO here is for adding the creation of objects that depend on the ``m_d3dDevice`` or ``m_d3dContext``, but do not care about the size of the rendering window.

```cpp
// These are the resources that depend on the device.
void Game::CreateDevice()
{
...

    // TODO: Initialize device dependent objects here (independent of window size)
}
```

The other function called by ``Initialize`` is the **CreateResources** function which sets up the swapchain (which defaults to a ``B8G8R8A8_UNORM`` format), and depth/stencil buffer (which defaults to ``D24_UNORM_S8_UINT`` which works on all Direct3D hardware feature levels in basic template). The TODO here is for adding the creation of objects that depend on the size of the rendering window. Note that this function could be creating these objects for the first time, it could be re-creating already existing objects due to a window-size change, or could be creating 'fresh' objects after a Direct3D device-removed or device-reset case.

```cpp
void Game::CreateResources()
{
...
    // TODO: Initialize windows-size dependent objects here
}
```

> Universal Windows Platform (UWP) apps require the use of 'flip' style swap effects, either ``DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL`` or ``DXGI_SWAP_EFFECT_FLIP_DISCARD``. These [DXGI](https://docs.microsoft.com/windows/win32/direct3ddxgi/dx-graphics-dxgi) swap chains cannot be created with an ``DXGI_FORMAT_x_UNORM_SRGB`` format or use MSAA (aka ``SampleDesc.Count`` > 1). Both sRGB gamma-correction and MSAA require special handling. Use of these newer 'flip' style modes are also recommended for Win32 desktop applications on Windows 10 (see [this blog post](https://devblogs.microsoft.com/directx/dxgi-flip-model/)). See [this blog series](https://walbourn.github.io/care-and-feeding-of-modern-swapchains/) and [[DeviceResources]] for more details.

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
The **Render** function which should render a single 'frame' of the scene, which starts by a ``Clear`` of the render target and setting the rendering viewport. It ends with a ``Present`` of the rendered frame.

```cpp
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
        return;

    Clear();

    // TODO: Add your rendering code here

    Present();
}
```

### Clear
The **Clear** function defaults to a background color of the classic "Cornflower blue".

```cpp
void Game::Clear()
{
    // Clear the views
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(),
        Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(),
        m_depthStencilView.Get());

    CD3D11_VIEWPORT viewPort(0.0f, 0.0f,
        static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewPort);
}
```

> In the older version of the template, the viewport was set in ``CreateResources`` and assumed to stay
> set for the remainder of the program execution or until the window was resized. This approach is outdated,
> however, as the viewport could be overwritten or cleared by ``ClearState``. When dealing with deferred
> contexts, Xbox One fast semantics, or the Direct3D 12 API, assumptions of device state persisting from
> frame-to-frame without being reset is a likely source of rendering bugs. Therefore, this template uses the
> best practice of resetting the viewport state at the start of each frame.

**Xbox One:** For Xbox One DirectX 11.X "fast semantics", it is important to set the render targets at the end of ``Clear`` because clearing the render target unbinds it from the render pipeline. We also explicitly set the viewport every frame as all state is reset between frames.

### Present

The **Present** method presents the swapchain and handles the device removed ("lost device") detection.

```cpp
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}
```

**Xbox:** Xbox titles do not encounter lost device or device removed scenarios. [UWP on Xbox](https://walbourn.github.io/directx-and-uwp-on-xbox-one/) apps still needs to handle these scenarios.

## Events
The template includes a number of message handlers that are called for process state changes: **OnActivated**, **OnDeactivated**, **OnSuspending**, **OnResuming**, and **OnWindowSizeChanged**. The UWP version also includes **ValidateDevice**, and display orientation is provided long with the window size.

> For Win32 desktop, the **OnSuspending** / **OnResuming** messages are triggered when (a) the window is minimized/unminimized or (b) in reaction to the ``WM_POWERBROADCAST`` message. On other platforms, this is driven by Process Lifecycle Management ([PLM](https://docs.microsoft.com/windows/uwp/launch-resume/app-lifecycle)).

Since we are using [[ComPtr]], most cleanup is automatic when the Game class is destroyed. If ``Present`` encounters a device-removed or device-reset, then the application needs to release all Direct3D objects and recreate the device, swapchain, and all Direct3D objects again. Therefore, the TODO in **OnDeviceLost** should be updated to release your application's Direct3D objects.

```cpp
void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here

...
}
```

> You will not get "device lost" all that often. In legacy Direct3D 9, you would routinely get a 'device lost' if you just <kbd>Alt</kbd>+<kbd>TAB</kbd> away from the application because the GPU used to be an 'exclusive' rather than 'shared' resource. The situation where you'd get ``DXGI_ERROR_DEVICE_RESET`` is if the driver crashes or the video hardware hangs. You get ``DXGI_ERROR_DEVICE_REMOVED`` if a new driver is installed while your application is running, or if you are running on a 'GPU is in the dock' style laptop and the laptop is undocked. You can test this case by opening the *Developer Command Prompt for Visual Studio* as an administrator, and typing ``dxcap -forcetdr`` which will immediately cause all currently running Direct3D apps to get a ``DXGI_ERROR_DEVICE_REMOVED`` event.

# Smart-pointer
We make use of the ``Microsoft::WRL::ComPtr`` smart-pointer for managing the lifetime of the Direct3D 11 COM objects, which is why we make use of ``.Get()`` in the code above. See [[ComPtr]] and [Microsoft Docs](https://docs.microsoft.com/windows/win32/prog-dx-with-com) for more information and usage.

# Error handling
Many Direct3D functions return an ``HRESULT`` which is the standard for COM APIs. For robustness and easier debugging, it is important that you always check the result of every function that return an ``HRESULT``. If you really can safely assume there is no error condition for a particular function, the function itself will return ``void`` instead of ``HRESULT``.

The Win32 game template makes use of the helper function [[ThrowIfFailed]] in the ``DX`` C++ namespace declared in ``pch.h``. This is the same helper that is used by the Windows Store and Windows phone VS templates. This helper throws a C++ exception if the standard ``FAILED`` macro returns true for a given ``HRESULT``. This is used for [fail fast](https://en.wikipedia.org/wiki/Fail-fast) error handling.

```cpp
DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc,
    nullptr, &depthStencil));
```

> Do not use ``hr == S_OK`` to check for success. Use ``SUCCEEDED(hr)`` instead.

# Gamma
For these tutorials, we make use of the ``DXGI_FORMAT_B8G8R8A8_UNORM`` backbuffer format. This does not give gamma-correct results, but is easier to set up. If you want to implement linear-space rendering which would use ``DXGI_FORMAT_B8G8R8A8_UNORM_SRGB``, see [[DeviceResources]].

[Linear-Space Lighting (i.e. Gamma)](http://filmicworlds.com/blog/linear-space-lighting-i-e-gamma/)   
[Chapter 24. The Importance of Being Linear](https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-24-importance-being-linear), GPU Gems 3   
[Gamma-correct rendering](https://blog.molecular-matters.com/2011/11/21/gamma-correct-rendering/)   
[The Care and Feeding of Modern Swap Chains](https://walbourn.github.io/care-and-feeding-of-modern-swapchains/)

# Fullscreen

The Win32 desktop and UWP templates implement [immersive fullscreen](https://walbourn.github.io/care-and-feeding-of-modern-swap-chains-3/). You can toggle this using the traditional hotkey <kbd>Alt</kbd>+<kbd>Enter</kbd>. If you want to default to full-screen at startup, see the ``TODO`` comments in ``Main.cpp``.

# Message 'pump'

For the Win32 desktop versions of the template, the ``Main.cpp`` source file contains the **WndProc** and the basic loop:

```
MSG msg = {};
while (WM_QUIT != msg.message)
{
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    else
    {
        g_game->Tick();
    }
}
```

This ensures that all pending Win32 messages are consumed before updating/rendering the next frame. This is critical for maintaining responsiveness.

For UWP, the equivalent thing is done inside the ``ViewProvider`` **Run** method.

**Next lesson**: [[Using DeviceResources]]

# Further reading
[Direct3D Win32 Game Visual Studio template](https://walbourn.github.io/direct3d-win32-game-visual-studio-template/)  
[Direct3D Game Visual Studio templates (Redux)](https://walbourn.github.io/direct3d-game-visual-studio-templates-redux/)  
[Anatomy of Direct3D 11 Create Device](https://walbourn.github.io/anatomy-of-direct3d-11-create-device/)   
[Manifest Madness](https://aka.ms/I6kdnw)  
[64-bit programming for Game Developers](https://docs.microsoft.com/windows/desktop/DxTechArts/sixty-four-bit-programming-for-game-developers)
