The first lesson in the [[Getting Started]] guide is to create a basic game loop.

# Setup
Our starting point is to use the **Direct3D Win32 Game** or the **Direct3D UWP Game** project template. Install the [VS 2015/2017/2019](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DUWPGame.vsix) VSIX on your development system, and then start (or restart) Visual Studio.

# Creating a new project
## Visual Studio 2015 / 2017
* From the drop-down menu, select **File** and then **New** -> **Project...**
* Select "Visual C++" on the left-hand tree-view.
* Select "Direct3D Win32 Game" or "Direct3D UWP Game".
* Fill in the "Name" and "Location" fields as desired.
* Optional: Uncheck "Create directory for solution" to keep the directory structure as bit flatter.
* Select "OK".

![Create New Project](https://github.com/Microsoft/DirectXTK/wiki/images/CreateNewProject.png)

## Visual Studio 2019
* From the drop-down menu, select **File** and then **New** -> **Project...** or on the startup dialog select **Create a new project**
* Select "Games" on the project types filter. You can optionally type "Win32" or "UWP" in the search box as well.
* Select "Direct3D Win32 Game" or "Direct3D UWP Game".
* Select "Next"
* Fill in the "Project name" and "Location" fields as desired.
* Optional: Check "Place solution and project in the same directory" to keep the directory structure as bit flatter.
* Select "Create".

![Create New Project](https://github.com/Microsoft/DirectXTK/wiki/images/CreateNewProject2019.png)

## Windows 10 SDK Selection
The templates use the Windows 10 SDK, so the project wizard will trigger this dialog to select the Windows 10 SDK version to use. Note that for Win32 Windows desktop apps, the "Minimum version" setting is ignored.

![Windows 10 SDK Selection](https://github.com/Microsoft/DirectXTK/wiki/images/Windows10SDKSelect.png)

## Newly Created Project
The newly created project contains the following files:

![Open Game Cpp](https://github.com/Microsoft/DirectXTK/wiki/images/OpenGameCpp.png)

* Precompiled header files
  * pch.h
  * pch.cpp
* Main application entry-point and classic Windows procedure function
  * Main.cpp
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

# Running the application

Visual Studio will default to the _Win32_ platform / _Debug_ configuration which builds an x86 (32-bit) application with debugging enabled. The template contains both _Debug_ and _Release_ configurations for both _Win32_ and _x64_ (x64 native 64-bit) platforms, with UWP also including the ARM platforms.

Press F5 to build and run the application It displays the following window:

![Running Project ](https://github.com/Microsoft/DirectXTK/wiki/images/RunningProject.png)

> _Troubleshooting:_ If the base template fails to start, there are a few possibilities. First, if your system
> doesn't have any Direct3D capable device of any feature level, it will fail. This is pretty unlikely on modern
> versions of Windows. Second if it runs fine in _Release_ but fails in _Debug_, then you likely do not have the
> [proper DirectX Debug Device](https://walbourn.github.io/direct3d-sdk-debug-layer-tricks/) installed for your operating system. Finally, if you are on Windows 7, you need to have the [KB2670838](https://walbourn.github.io/directx-11-1-and-windows-7-update/) installed.

> **Xbox One**: the background color may be slightly oversaturated. This is because the basic Xbox One XDK template uses a backBufferFormat of ``DXGI_FORMAT_B8G8R8A8_UNORM_SRGB``. The DirectXMath Colors values are defined using standard [sRGB](https://en.wikipedia.org/wiki/SRGB) colorspace which is slightly different. All the colors defines need to be adjusted slightly for the linear RGB colorspace (aka gamma correct rendering) via ``XMColorSRGBToRGB``.

# Tour of the code
For now, we'll focus on the content of ``Game.cpp`` (which is open by default).

## Initialize
When the application first starts, execution is passed to the **Initialize** method. The TODO here by default leaves the applications [[StepTimer]] in the 'variable length' mode. You uncomment the code if you want StepTimer in the 'fixed-step' mode. We'll explain this more once we get to ``Update``.

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

The other function called by ``Initialize`` is the **CreateResources** function which sets up the swapchain (which defaults to a ``B8G8R8A8_UNORM`` format), and depth buffer (which defaults to ``D24_UNORM_S8_UINT`` which works on all Direct3D feature levels). The TODO here is for adding the creation of objects that depend on the size of the rendering window. Note that this function could be creating these objects for the first time, it could be re-creating already existing objects due to a window-size change, or could be creating 'fresh' objects after a Direct3D device-removed or device-reset case.

```cpp
void Game::CreateResources()
{
...
    // TODO: Initialize windows-size dependent objects here
}
```

> Universal Windows Platform (UWP) apps require the use of 'flip' style swap effects, either ``DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL`` or ``DXGI_SWAP_EFFECT_FLIP_DISCARD``. These DXGI swap chains cannot be created with an ``DXGI_FORMAT_x_UNORM_SRGB`` format or use MSAA (aka ``SampleDesc.Count`` > 1). Both sRGB gamma-correction and MSAA require special handling. See [[DeviceResources]] for more details.

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

**Xbox One:** For Xbox One fast semantics, it is important to set the render targets at the end of ``Clear`` because clearing the render target unbinds it from the render pipeline.

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

**Xbox One:** With Xbox One XDK development, you won't ever encounter a 'device lost' situation.

# Smart-pointer
We make use of the ``Microsoft::WRL::ComPtr`` smart-pointer for managing the lifetime of the Direct3D 11 COM objects, which is why we make use of ``.Get()`` in the code above. See [[ComPtr]] for more information and usage.

# Error handling
Many Direct3D functions return an ``HRESULT`` which is the standard for COM APIs. For robustness and easier debugging, it is important that you always check the result of every function that return an ``HRESULT``. If you really can safely assume there is no error condition for a particular function, the function itself will return ``void`` instead of ``HRESULT``.

The Win32 game template makes use of the helper function [[ThrowIfFailed]] in the ``DX`` C++ namespace. This is the same helper that is used by the Windows Store and Windows phone VS templates. This helper throws a C++ exception if the standard ``FAILED`` macro returns true for a given ``HRESULT``.

```cpp
DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc,
    nullptr, &depthStencil));
```

> Do not use ``hr == S_OK`` to check for success. Use ``SUCCEEDED(hr)`` instead.

# Gamma
For these tutorials, we make use of the ``DXGI_FORMAT_B8G8R8A8_UNORM`` backbuffer format. This does not give gamma-correct results, but is easier to set up. If you want to implement linear-space rendering which would use ``DXGI_FORMAT_B8G8R8A8_UNORM_SRGB``, see [[DeviceResources]].

[Linear-Space Lighting (i.e. Gamma)](http://filmicworlds.com/blog/linear-space-lighting-i-e-gamma/)   
[Chapter 24. The Importance of Being Linear](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch24.html), GPU Gems 3   
[Gamma-correct rendering](https://blog.molecular-matters.com/2011/11/21/gamma-correct-rendering/)   

**Next lesson**: [[Adding the DirectX Tool Kit]]

# Further reading
[Direct3D Win32 Game Visual Studio template](https://walbourn.github.io/direct3d-win32-game-visual-studio-template/)  
[Direct3D Game Visual Studio templates (Redux)](https://walbourn.github.io/direct3d-game-visual-studio-templates-redux/)  
[Manifest Madness](https://aka.ms/I6kdnw)  
[64-bit programming for Game Developers](https://docs.microsoft.com/en-us/windows/desktop/DxTechArts/sixty-four-bit-programming-for-game-developers)
