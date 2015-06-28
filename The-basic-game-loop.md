The first lesson in the [[Getting Started]] guide is to create a basic game loop.

# Setup
Our starting point is to use the **Direct3D Win32 Game** project template. Install the [Direct3DWin32Game.vsix](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DWin32Game.vsix) on your development system, and then start (or restart) Visual Studio 2013.

_Note if you are using VS 2015, install [Direct3DUWPGame.vsix](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DUWPGame.vsix) instead which installs both the **Direct3D Win32 Game** template and a similar **Direct3D UWP Game** template for the universal Windows app platform._

# Creating a new project
* From the drop-down menu, select **File** and then **New** -> **Project...**
* Select "Visual C++" on the left-hand tree-view.
* Select "Direct3D Win32 Game".
* Fill in the "Name" and "Location" fields as desired.
* Optional: Uncheck "Create directory for solution" to keep the directory structure as bit flatter.
* Select "OK".

![Create New Project](https://github.com/Microsoft/DirectXTK/wiki/images/CreateNewProject.png)

The newly created project contains the following files:

![Open Game Cpp](https://github.com/Microsoft/DirectXTK/wiki/images/OpenGameCpp.png)

* Precompiled header files
 * pch.h 
 * pch.cpp
* Main application entry-point and classic Windows procedure function
 * Main.cpp
* Timer helper class
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

_Troubleshooting:_ If the base template fails to start, there are a few possibilities. First, if your system doesn't have any Direct3D capable device of any feature level, it will fail. This is pretty unlikely on modern versions of Windows. Second if it runs fine in _Release_ but fails in _Debug_, then you likely do not have the [proper DirectX Debug Device](http://blogs.msdn.com/b/chuckw/archive/2012/11/30/direct3d-sdk-debug-layer-tricks.aspx) installed for your operating system.

# Tour of the code
For now, we'll focus on the content of ``Game.cpp`` (which is open by default).

When the application first starts, execution is passed to the **Initialize** method. The TODO here by default leaves the applications StepTimer in the 'variable length' mode. You uncomment the code if you want StepTimer in the 'fixed-step' mode. We'll explain this more once we get to Update.

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

One of the two functions called by Initialize is the **CreateDevice** function which sets up a Direct3D 11.0 (and optionally Direct3D 11.1) device using the maximum Direct3D Feature Level supported by the system which could range from 9.1 to 11.1. For Debug builds, it enables the Direct3D "Debug device" which provides additional validation and diagnostic errors (seen in the "Output" window in Visual C++ when showing output from "Debug"). The TODO here is for adding the creation of objects that depend on the ``m_d3dDevice`` or ``m_d3dContext``, but do not care about the size of the rendering window.

    // These are the resources that depend on the device.
    void Game::CreateDevice()
    {
    ...

        // TODO: Initialize device dependent objects here (independent of window size)
    }

The other function called by Initialize is the **CreateResources** function which sets up the swapchain (which defaults to a ``B8G8R8A8_UNORM`` format), and depth buffer (which defaults to ``D24_UNORM_S8_UINT`` which works on all Direct3D feature levels). The TODO here is for adding the creation of objects that depend on the size of the rendering window. Note that this function could be creating these objects for the first time, it could be re-creating already existing objects due to a window-size change, or could be creating 'fresh' objects after a Direct3D device-removed or device-reset case.

    void Game::CreateResources()
    {
    ...
        // TODO: Initialize windows-size dependent objects here
    }

The **Update** method is intended to handle game-world state modification which is typically driven by time passing, simulation, and/or user-input. By default, Update is called once per 'frame' and can have an arbitrary delta-time. This is called a 'variable-step' mode.

If in the Initialize method above you uncomment the TODO code, then each Update will be for a fixed time-step (1/60th of a second), with Update called as many time in a single 'frame' as needed to keep it up-to-date. This is called a 'fixed-step' mode and potentially be more stable for many kinds of simulations.

    // Updates the world
    void Game::Update(DX::StepTimer const& timer)
    {
        float elapsedTime = float(timer.GetElapsedSeconds());

        // TODO: Add your game logic here
        elapsedTime;
    }

The **Render** function which should render a single 'frame' of the scene, which starts by a Clear of the render targeting and setting the rendering viewport, then ends with a Present of the rendered frame.

    void Game::Render()
    {
        // Don't try to render anything before the first Update.
        if (m_timer.GetFrameCount() == 0)
            return;

        Clear();

        CD3D11_VIEWPORT viewPort(0.0f, 0.0f,
            static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
        m_d3dContext->RSSetViewports(1, &viewPort);

        // TODO: Add your rendering code here

        Present();
    }

_Note: In the older version of the template, the viewport was set in ``CreateResources`` and assumed to stay set for the remainder of the program execution or until the window was resized. This approach is outdated, however, as the viewport could be overwritten or cleared by ``ClearState``. When dealing with deferred contexts, Xbox One fast semantics, or the Direct3D 12 API, assumptions of device state persisting from frame-to-frame without being reset is a likely source of rendering bugs. Therefore, this template uses the best practice of resetting the viewport state at the start of each frame._

The **Clear** function defaults to a background color of the classic "Cornflower blue".

    void Game::Clear()
    {
        // Clear the views
        m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(),
            Colors::CornflowerBlue);
        m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(),
            D3D11_CLEAR_DEPTH, 1.0f, 0);
        m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(),
            m_depthStencilView.Get());
    }

_Note: For Xbox One fast semantics, it is important to set the render targets at the end of Clear because clearing the render target unbinds it from the render pipeline. So again, the template is demonstrating best practice._

Since we are using [[ComPtr]], most cleanup is automatic when the Game class is destroyed. If Present encounters a device-removed or device-reset (most commonly the driver crashed or was updated), then the application needs to release all Direct3D objects and recreate the device, swapchain, and all Direct3D objects again. Therefore, the TODO in **OnDeviceLost** should be updated to release your application's Direct3D objects.

    void Game::OnDeviceLost()
    {
        // TODO: Add Direct3D resource cleanup here

    ...
    }

# Smart-pointer
We make use of the ``Microsoft::WRL::ComPtr`` smart-pointer for managing the lifetime of the Direct3D 11 COM objects, which is why we make use of ".Get()" in the code above. See [[ComPtr]] for more information and usage.

# Error handling
Many Direct3D functions return an ``HRESULT`` which is the standard for COM APIs. For robustness and easier debugging, it is important that you always check the result of every function that return an ``HRESULT``. If you really can safely assume there is no error condition for a particular function, the function itself will return ``void`` instead of ``HRESULT``.

The Win32 game template makes use of the helper function ``ThrowIfFailed`` in the ``DX`` C++ namespace. This is the same helper that is used by the Windows Store and Windows phone VS templates. This helper throws a C++ exception if the standard ``FAILED`` macro returns true for a given ``HRESULT``.

    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc,
        nullptr, &depthStencil));

**Next lesson**: [[Adding the DirectX Tool Kit]]

# Further reading
[Direct3D Win32 Game Visual Studio template](http://blogs.msdn.com/b/chuckw/archive/2015/01/06/direct3d-win32-game-visual-studio-template.aspx)  
[Anatomy of Direct3D 11 Create Device](http://blogs.msdn.com/b/chuckw/archive/2014/02/05/anatomy-of-direct3d-11-create-device.aspx)  
[Direct3D Feature Levels](http://blogs.msdn.com/b/chuckw/archive/2012/06/20/direct3d-feature-levels.aspx)  
[Understanding Game Time Revisited](http://blogs.msdn.com/b/chuckw/archive/2014/12/03/understanding-game-time-revisited.aspx)  
[Direct3D SDK Debug Layer Tricks](http://blogs.msdn.com/b/chuckw/archive/2012/11/30/direct3d-sdk-debug-layer-tricks.aspx)  
[Manifest Madness](http://blogs.msdn.com/b/chuckw/archive/2013/09/10/manifest-madness.aspx)  
[64-bit programming for Game Developers](http://msdn.microsoft.com/en-us/library/windows/desktop/ee418798.aspx)