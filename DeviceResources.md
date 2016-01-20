In [[the basic game loop]] lesson and other tutorials, we are making use of the **D3D11Game** VS template which has all the relevant Direct3D code in the ``Game`` class including creating the device and swap chain. This makes it easy to teach with, and for the reader to see all the code in one place. This, however, does result in a lot of 'boiler-plate' code in the main ``Game`` class which could be distracting in larger projects & samples.

There is therefore a "DR" variant of each of the D3DGame VS templates in the [VS 2013](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DWin32Game.vsix) / [VS 2015](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DUWPGame.vsix) VSIX package.

> The standard Windows Store DirectX and Windows phone 8 VS templates make use of a similar ``DeviceResources`` abstraction which this variant is modeled after.

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
* Timer helper class
 * StepTimer.h
* Resources
 * directx.ico
 * resource.rc
 * settings.manifest
* The Game class
 * Game.h
 * Game.cpp

# Tour of the code

Here we are focusing on the differences in ``Game.cpp`` between the original *D3D11Game* and the *D3D11GameDR* template variant. **Update** is the same as before, as are most of the events. **OnDeviceLost** no longer has the calls to re-create the objects as this is now handled by **OnDeviceRestored**. Both are callbacks from ``DeviceResources``.

## Initialize
When the application first starts, execution is passed to the **Initialize** method. The TODO here by default leaves the applications [StepTimer](http://blogs.msdn.com/b/chuckw/archive/2014/12/03/understanding-game-time-revisited.aspx) in the 'variable length' mode.

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

The first Game method ``Initialize`` calls is **CreateDeviceDependentResources** for the creation of objects that depend on the device or context, but do not care about the size of the rendering window.

    void Game::CreateDeviceDependentResources()
    {
        auto device = m_deviceResources->GetD3DDevice();

        // TODO: Initialize device dependent objects here (independent of window size).
        device; // This is only here to avoid a warning. You can remove it once you make use of device!
    }

> Instead of using the class variable ``m_d3dDevice`` we have to obtain the device interface from the ``DeviceResources`` object. See ``Render`` for how you get the device context from ``DeviceResources``.

The second Game method ``Initialize`` calls is **CreateWindowSizeDependentResources** for creation of objects that depend on the size of the rendering window. Note that this function could be creating these objects for the first time, it could be re-creating already existing objects due to a window-size change, or could be creating 'fresh' objects after a Direct3D device-removed or device-reset case.

    void Game::CreateWindowSizeDependentResources()
    {
        // TODO: Initialize windows-size dependent objects here.
    }

## Render
The **Render** function which should render a single 'frame' of the scene, which starts by a ``Clear`` of the render targeting and setting the rendering viewport, then ends with a call to ``DeviceResources::Present`` to show the rendered frame.

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

> Instead of using the class variable ``m_d3dContext`` we have to obtain the device context interface from the ``DeviceResources`` object. See ``CreateDeviceDependentResources`` for how you get the device from ``DeviceResources``.

The **Clear** method defaults to a background color of the classic "Cornflower blue".

    void Game::Clear()
    {
        // Clear the views
        auto context = m_deviceResources->GetD3DDeviceContext();
        auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
        auto depthStencil = m_deviceResources->GetDepthStencilView();

        context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
        context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        context->OMSetRenderTargets(1, &renderTarget, depthStencil);

        // Set the viewport.
        auto viewport = m_deviceResources->GetScreenViewport();
        context->RSSetViewports(1, &viewport);
    }

# Device Resources

The **DeviceResources** class includes a number of useful assessors not already used above

* **GetOutputSize**: Returns the output size of the presentation window associated with the device.
* **GetSwapChain**: Returns the DXGI swap chain interface.
* **GetDeviceFeatureLevel**: Returns the Direct3D hardware feature level in use.
* **GetBackBufferFormat**: Returns the format of the render target returned by ``GetBackBufferRenderTargetView``.
* **GetDepthBufferFormat**: Returns the format of the depth/stencil buffer returned by ``GetDepthStencilView``.
* **GetBackBufferCount**: Returns the number of back-buffers in the swap chain.

# Notes

Since the ``DeviceResources`` class is now in it's own file and no longer directly impacts the readability of the template, it has a few enhancements.

* If the SDK Debug Layer is not present on the target system when running ``Debug`` configurations, it will automatically fallback to creating the device without debugging.
* The DR version always uses ``D3D11_CREATE_DEVICE_BGRA_SUPPORT`` which is required for Direct2D/DirectWrite interop if that's desired.
* If no hardware device is available, the DR version will fall back to using [WARP](https://msdn.microsoft.com/en-us/library/windows/desktop/gg615082.aspx) in non-production builds as this is useful for debugging and validation.
* In ``Debug`` configurations, additional diagnostic messages are output to the debug window.
* Rather than always using the default Direct3D device, the DR version will filter out the Microsoft Basic Render Driver adapter as this fallback software device is seldom acceptable performance for games.

The DR VS template variants include the enhanced version of [[ThrowIfFailed]].

# Further reading

[Direct3D Win32 Game Visual Studio template (Redux)](http://blogs.msdn.com/b/chuckw/archive/2015/12/17/direct3d-game-visual-studio-templates-redux.aspx)  
[Anatomy of Direct3D 11 Create Device](http://blogs.msdn.com/b/chuckw/archive/2014/02/05/anatomy-of-direct3d-11-create-device.aspx)
