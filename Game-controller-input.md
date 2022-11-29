|[[Getting Started]] for DX11|[Getting Started](https://github.com/microsoft/DirectXTK12/wiki/Getting-Started) for DX12|
|---|---|

This lesson will show how to read user input from game controllers, and use it to control a simple 3D camera.

# Setup
First create a new project using the instructions from the earlier lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

> If using *DirectX Tool Kit for DX12*, use [Using DeviceResources](https://github.com/microsoft/DirectXTK12/wiki/Using-DeviceResources) and [Adding the DirectX Tool Kit](https://github.com/microsoft/DirectXTK12/wiki/Adding-the-DirectX-Tool-Kit).

# Adding use of a gamepad

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::GamePad> m_gamePad;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
m_gamePad = std::make_unique<GamePad>();
```

In **Game.cpp**, add to the TODO of **OnSuspending** and **OnDeactivated**:

```cpp
m_gamePad->Suspend();
```

In **Game.cpp**, add to the TODO of **OnResuming** and **OnActivated**:

```cpp
m_gamePad->Resume();
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
auto pad = m_gamePad->GetState(0);

if (pad.IsConnected())
{
    if (pad.IsViewPressed())
    {
        ExitGame();
    }
}
```

Build and run. The application does not display anything other than our cornflower blue screen, but you can use the *View* button on the Xbox One controller or *Back* on the Xbox 360 Controller to exit.

<details><summary><i>Click here for troubleshooting advice</i></summary>
<p>If using MinGW, you will likely get unresolved link errors for <code>XInputGetState</code>, etc. You should explicitly link to <code>xinput1_4.lib</code> or <code>xinput9_1_0.lib</code> to fix this.</p></details>

# Adding a a simple scene
Start by saving [roomtexture.dds](https://github.com/Microsoft/DirectXTK/wiki/media/roomtexture.dds) into your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "roomtexture.dds" and click "OK".

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::GeometricPrimitive> m_room;

DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;

float m_pitch;
float m_yaw;
DirectX::SimpleMath::Vector3 m_cameraPos;

DirectX::SimpleMath::Color m_roomColor;
```

At the top of **Game.cpp** after the ``using`` statements, add:

```cpp
namespace
{
   const XMVECTORF32 START_POSITION = { 0.f, -1.5f, 0.f, 0.f };
   const XMVECTORF32 ROOM_BOUNDS = { 8.f, 6.f, 12.f, 0.f };
   constexpr float ROTATION_GAIN = 0.1f;
}
```

In **Game.cpp** file, modify the **Game** constructor to initialize our variables:

```cpp
Game::Game() noexcept(false) :
    m_pitch(0),
    m_yaw(0),
    m_cameraPos(START_POSITION),
    m_roomColor(Colors::White)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_proj = Matrix::CreatePerspectiveFieldOfView(
    XMConvertToRadians(70.f),
    float(size.right) / float(size.bottom), 0.01f, 100.f);
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
// limit pitch to straight up or straight down
constexpr float limit = XM_PIDIV2 - 0.01f;
m_pitch = std::max(-limit, m_pitch);
m_pitch = std::min(+limit, m_pitch);

// keep longitude in sane range by wrapping
if (m_yaw > XM_PI)
{
    m_yaw -= XM_2PI;
}
else if (m_yaw < -XM_PI)
{
    m_yaw += XM_2PI;
}

float y = sinf(m_pitch);
float r = cosf(m_pitch);
float z = r * cosf(m_yaw);
float x = r * sinf(m_yaw);

XMVECTOR lookAt = m_cameraPos + Vector3(x, y, z);

m_view = XMMatrixLookAtRH(m_cameraPos, lookAt, Vector3::Up);
```

Continue below depending on which version of *DirectX Tool Kit* you are using.

## DirectX 11

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roomTex;
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
auto context = m_deviceResources->GetD3DDeviceContext();
m_room = GeometricPrimitive::CreateBox(context,
    XMFLOAT3(ROOM_BOUNDS[0], ROOM_BOUNDS[1], ROOM_BOUNDS[2]),
    false, true);

DX::ThrowIfFailed(
    CreateDDSTextureFromFile(device, L"roomtexture.dds",
        nullptr, m_roomTex.ReleaseAndGetAddressOf()));
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_room.reset();
m_roomTex.Reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
m_room->Draw(Matrix::Identity, m_view, m_proj,
    m_roomColor, m_roomTex.Get());
```

## DirectX 12

> This assumes you've already added *DirectX Tool Kit for 12* support including ``GraphicsMemory`` to your project.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
Microsoft::WRL::ComPtr<ID3D12Resource> m_roomTex;
std::unique_ptr<DirectX::DescriptorHeap> m_resourceDescriptors;
std::unique_ptr<DirectX::CommonStates> m_states;
std::unique_ptr<DirectX::BasicEffect> m_roomEffect;
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
m_resourceDescriptors = std::make_unique<DescriptorHeap>(device, 1);

m_states = std::make_unique<CommonStates>(device);

m_room = GeometricPrimitive::CreateBox(
    XMFLOAT3(ROOM_BOUNDS[0], ROOM_BOUNDS[1], ROOM_BOUNDS[2]),
    false, true);

RenderTargetState rtState(m_deviceResources->GetBackBufferFormat(),
    m_deviceResources->GetDepthBufferFormat());

{
    EffectPipelineStateDescription pd(
        &GeometricPrimitive::VertexType::InputLayout,
        CommonStates::Opaque,
        CommonStates::DepthDefault,
        CommonStates::CullCounterClockwise,
        rtState);

    m_roomEffect = std::make_unique<BasicEffect>(device,
        EffectFlags::Lighting | EffectFlags::Texture, pd);
    m_roomEffect->EnableDefaultLighting();
}

ResourceUploadBatch resourceUpload(device);

resourceUpload.Begin();

DX::ThrowIfFailed(CreateDDSTextureFromFile(device, resourceUpload,
    L"roomtexture.dds",
    m_roomTex.ReleaseAndGetAddressOf()));

CreateShaderResourceView(device, m_roomTex.Get(),
    m_resourceDescriptors->GetFirstCpuHandle());

m_roomEffect->SetTexture(m_resourceDescriptors->GetFirstGpuHandle(),
    m_states->LinearClamp());

auto uploadResourcesFinished = resourceUpload.End(
    m_deviceResources->GetCommandQueue());
uploadResourcesFinished.wait();

m_deviceResources->WaitForGpu();
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_room.reset();
m_roomTex.Reset();
m_resourceDescriptors.reset();
m_states.reset();
m_roomEffect.reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
ID3D12DescriptorHeap* heaps[] = {
    m_resourceDescriptors->Heap(), m_states->Heap()
};
commandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)),
    heaps);

m_roomEffect->SetMatrices(Matrix::Identity, m_view, m_proj);
m_roomEffect->SetDiffuseColor(m_roomColor);
m_roomEffect->Apply(commandList);
m_room->Draw(commandList);
```

## Build and run

Build and run, and you should get the following screen:

![Screenshot of room](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotRoom.PNG)

<details><summary><i>Click here for troubleshooting advice</i></summary>
<p>If you get a runtime exception, then you may have the "roomtexture.dds" in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on <code>CreateDDSTextureFromFile</code> and step into the code to find the exact problem.</p></details>

# Adding gamepad controls

In **Game.cpp**, modify to the TODO of **Update**:

```cpp
auto pad = m_gamePad->GetState(0);
if (pad.IsConnected())
{
    if (pad.IsViewPressed())
    {
        ExitGame();
    }

    if (pad.IsLeftStickPressed())
    {
        m_yaw = m_pitch = 0.f;
    }
    else
    {
        m_yaw += -pad.thumbSticks.leftX * ROTATION_GAIN;
        m_pitch += pad.thumbSticks.leftY * ROTATION_GAIN;
    }
}

// limit pitch to straight up or straight down
...
```

Build and run. The left thumb stick on the gamepad can be used to adjust the view. If you click the left-stick, it resets the view.

## Technical notes

* The view can never be exactly straight up or straight down to avoid a problem known as
[gimbal lock](https://wikipedia.org/wiki/Gimbal_lock) which can cause the camera view to flip unexpectedly or tumble uncontrollably.

* This control implementation is very simple, and really only works on a system with fast frame-rate. For a more robust implementation, the ``Update`` code should make use of ``elapsedTime`` to scale the keyboard movement rate values and ``ROTATION_GAIN`` adjusted accordingly.

# Using vibration

In **Game.cpp**, modify the TODO section of **Update**:

```cpp
auto pad = m_gamePad->GetState(0);

if (pad.IsConnected())
{
    if (pad.IsViewPressed())
    {
        ExitGame();
    }
...

    {
        float left = (pad.IsAPressed()) ? 1.f : 0;
        float right = (pad.IsBPressed()) ? 1.f : 0;

        m_gamePad->SetVibration(0, left, right);
    }
}
```

Build and run. If you press and hold A or B, you get vibration motors of the controller to activate.

## Scaling vibration
In **Game.cpp**, modify the TODO section of **Update** to replace the use of ``SetVibration`` above with:

```cpp
m_gamePad->SetVibration( 0, pad.triggers.left, pad.triggers.right);
```

Build and run. Slowly depress the left and right triggers to feel the vibration motor change intensity.

# Detecting button transitions

The use of gamepad above is all written using 'instantaneous state' where we only care about the position or state at the time we read the gamepad. For many controls, you want to trigger an event at the moment a button is pressed or released.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::GamePad::ButtonStateTracker m_buttons;
```

In **Game.cpp**, add to the TODO of **OnResuming** and **OnActivated**:

```cpp
m_buttons.Reset();
```

In **Game.cpp**, modify the TODO section of *Update*:

```cpp
auto pad = m_gamePad->GetState(0);
if (pad.IsConnected())
{
    m_buttons.Update(pad);

    if (pad.IsViewPressed())
    {
        ExitGame();
    }

...
}
else
{
    m_buttons.Reset();
}
...

if (m_buttons.a == GamePad::ButtonStateTracker::PRESSED)
{
    if (m_roomColor == Colors::Red.v)
        m_roomColor = Colors::Green;
    else if (m_roomColor == Colors::Green.v)
        m_roomColor = Colors::Blue;
   else if (m_roomColor == Colors::Blue.v)
        m_roomColor = Colors::White;
   else
        m_roomColor = Colors::Red;
}
```

Build and run. Pressing the A button will cycle the color of the room through Red, Green, Blue, and White. Holding down the A button does not flash though the colors with the framerate.

# Orbit vs. FPS Style Camera

The code above implements an "First-Person Shooter" (FPS) style look camera. This has the viewpoint at a specific location, and then rotates to look around from there. Another common style of camera is an 'orbit' camera that moves around a point-of-focus, often used for viewing objects. To modify our FPS-style to an Orbit style, follow these instructions.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
float m_theta;
float m_phi;
float m_radius;
```

> You should delete or comment out the ``m_pitch``,  ``m_yaw``, and ``m_cameraPos``  declarations.

At the top of **Game.cpp** after the ``using`` statements, modify the anonymous namespace:

```cpp
namespace
{
  const XMVECTORF32 ROOM_BOUNDS = { 8.f, 6.f, 12.f, 0.f };
  constexpr float ROTATION_GAIN = 0.1f;

  constexpr float c_defaultPhi = XM_2PI / 6.0f;
  constexpr float c_defaultRadius = 3.3f;
  constexpr float c_minRadius = 0.1f;
  constexpr float c_maxRadius = 5.f;
}
```

In **Game.cpp** file, modify the **Game** constructor to initialize our new variables:

```cpp
Game::Game() noexcept(false) :
    m_theta(0.f),
    m_phi(c_defaultPhi),
    m_radius(c_defaultRadius),
    m_roomColor(Colors::White)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}
```

In **Game.cpp**, modify the TODO of **Update**:

```cpp
auto pad = m_gamePad->GetState(0);
if (pad.IsConnected())
{
    m_buttons.Update(pad);

    if (pad.IsViewPressed())
    {
        ExitGame();
    }

    if (pad.IsLeftStickPressed())
    {
        m_theta = 0.f;
        m_phi = c_defaultPhi;
        m_radius = c_defaultRadius;
    }
    else
    {
        m_theta += pad.thumbSticks.rightX * XM_PI * ROTATION_GAIN;
        m_phi -= pad.thumbSticks.rightY * XM_PI * ROTATION_GAIN;
        m_radius -= pad.thumbSticks.leftY * 5.f * ROTATION_GAIN;
    }

    m_gamePad->SetVibration(0, pad.triggers.left, pad.triggers.right);
}
else
{
    m_buttons.Reset();
}

// Limit to avoid looking directly up or down
constexpr float limit = XM_PIDIV2 - 0.01f;
m_phi = std::max(1e-2f, std::min(limit, m_phi));
m_radius = std::max(c_minRadius, std::min(c_maxRadius, m_radius));

if (m_theta > XM_PI)
{
    m_theta -= XM_2PI;
}
else if (m_theta < -XM_PI)
{
    m_theta += XM_2PI;
}

XMVECTOR lookFrom = XMVectorSet(
    m_radius * sinf(m_phi) * cosf(m_theta),
    m_radius * cosf(m_phi),
    m_radius * sinf(m_phi) * sinf(m_theta),
    0);

m_view = XMMatrixLookAtRH(lookFrom, g_XMZero, Vector3::Up);

if (m_buttons.a == GamePad::ButtonStateTracker::PRESSED)
{
    if (m_roomColor == Colors::Red.v)
        m_roomColor = Colors::Green;
    else if (m_roomColor == Colors::Green.v)
        m_roomColor = Colors::Blue;
    else if (m_roomColor == Colors::Blue.v)
        m_roomColor = Colors::White;
    else
        m_roomColor = Colors::Red;
}
```

Build and run. The same controls are present, but now move the camera in an 'orbital' path around the origin with the left stick moving closer/further away to the center.

**Next lessons:** [[Mouse and keyboard input]]

# Further reading

DirectX Tool Kit docs [[GamePad]]  
[DirectX Tool Kit: Now with GamePads](https://walbourn.github.io/directx-tool-kit-now-with-gamepads/)  
[XInput and Windows 8](https://walbourn.github.io/xinput-and-windows-8/)
