This lesson will show how to read user input from the mouse and keyboard, and use it to control a simple 3D camera.

# Setup
First create a new project using the instructions from the earlier lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

> If using *DirectX Tool Kit for DX12*, use [Using DeviceResources](https://github.com/microsoft/DirectXTK12/wiki/Using-DeviceResources) and [Adding the DirectX Tool Kit](https://github.com/microsoft/DirectXTK12/wiki/Adding-the-DirectX-Tool-Kit).

# Adding use of mouse and keyboard

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::Keyboard> m_keyboard;
std::unique_ptr<DirectX::Mouse> m_mouse;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
m_keyboard = std::make_unique<Keyboard>();
m_mouse = std::make_unique<Mouse>();
m_mouse->SetWindow(window);
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
auto kb = m_keyboard->GetState();
if ( kb.Escape )
{
    ExitGame();
}

auto mouse = m_mouse->GetState();
```

In **Main.cpp**, add to the ``switch`` statement in **WndProc**:

```cpp
case WM_ACTIVATEAPP:

    ...

    Keyboard::ProcessMessage(message, wParam, lParam);
    Mouse::ProcessMessage(message, wParam, lParam);
    break;

...

case WM_ACTIVATE:
case WM_INPUT:
case WM_MOUSEMOVE:
case WM_LBUTTONDOWN:
case WM_LBUTTONUP:
case WM_RBUTTONDOWN:
case WM_RBUTTONUP:
case WM_MBUTTONDOWN:
case WM_MBUTTONUP:
case WM_MOUSEWHEEL:
case WM_XBUTTONDOWN:
case WM_XBUTTONUP:
case WM_MOUSEHOVER:
    Mouse::ProcessMessage(message, wParam, lParam);
    break;

case WM_KEYDOWN:
case WM_KEYUP:
case WM_SYSKEYUP:
    Keyboard::ProcessMessage(message, wParam, lParam);
    break;

case WM_SYSKEYDOWN:
    Keyboard::ProcessMessage(message, wParam, lParam);
    if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
    {
        ...
    }
    break;
```

Build and run. The application does not display anything other than our cornflower blue screen, but you can use the Escape key to exit.

## Universal Windows Platform (UWP)
If using a Windows Universal Platform (UWP) app template, then in your **Initialize**, use:

```cpp
m_keyboard = std::make_unique<Keyboard>();
m_keyboard->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));

m_mouse = std::make_unique<Mouse>();
m_mouse->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));
```

Be sure you call ``Mouse::SetDpi(m_DPI);`` from **Main.cpp** at the bottom of ``SetWindow`` and ``OnDpiChanged``. The ``Mouse`` class returns position information in pixels, while the UWP platform specifies mouse location in "device-independent pixels" (DIPs) so this value is needed for the scaling conversion.

## Microsoft GDK

For the **Gaming.Desktop.x64** platform, follow the instructions for Win32 desktop above.

For the **Gaming.Xbox.*.x64** platforms, the instructions for Win32 desktop for mouse are the same as above although ``WM_INPUT`` and ``WM_MOUSEHOVER`` are not used, and there is no ``SetWindow`` method. If using 4k rendering, call ``Mouse::SetResolution(true);``. Keyboard integration for these platforms do not require hooking any Win32 messages.

# Adding a simple scene
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
   constexpr float ROTATION_GAIN = 0.004f;
   constexpr float MOVEMENT_GAIN = 0.07f;
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

> _Troubleshooting:_ If you get a runtime exception, then you may have the "roomtexture.dds" in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``CreateDDSTextureFromFile`` and step into the code to find the exact problem.

# Adding keyboard controls

In **Game.cpp**, modify to the TODO of **Update**:

```cpp
auto kb = m_keyboard->GetState();
if ( kb.Escape )
{
    ExitGame();
}

if (kb.Home)
{
    m_cameraPos = START_POSITION.v;
    m_pitch = m_yaw = 0;
}

Vector3 move = Vector3::Zero;

if (kb.Up || kb.W)
    move.y += 1.f;

if (kb.Down || kb.S)
    move.y -= 1.f;

if (kb.Left || kb.A)
    move.x += 1.f;

if (kb.Right || kb.D)
    move.x -= 1.f;

if (kb.PageUp || kb.Space)
    move.z += 1.f;

if (kb.PageDown || kb.X)
    move.z -= 1.f;

Quaternion q = Quaternion::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.f);

move = Vector3::Transform(move, q);

move *= MOVEMENT_GAIN;

m_cameraPos += move;

Vector3 halfBound = (Vector3(ROOM_BOUNDS.v) / Vector3(2.f) )
    - Vector3(0.1f, 0.1f, 0.1f);

m_cameraPos = Vector3::Min(m_cameraPos, halfBound);
m_cameraPos = Vector3::Max(m_cameraPos, -halfBound);

// limit pitch to straight up or straight down
...
```

Build and run. You can use ``Up``, ``Down``, ``Left``, ``Right``, ``PageUp``, ``PageDown``. ``W``, ``A``, ``S``, ``D``, ``X``, and ``Space`` to move through the scene. You can use ``Home`` to return to the start position.

# Adding mouse controls

In **Game.cpp**, add to the TODO of **Update** just before your keyboard code above:

```cpp
auto mouse = m_mouse->GetState();

if (mouse.positionMode == Mouse::MODE_RELATIVE)
{
    Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f)
                    * ROTATION_GAIN;

    m_pitch -= delta.y;
    m_yaw -= delta.x;
}

m_mouse->SetMode(mouse.leftButton
    ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
```

Build and run. Now in addition to keyboard controls, you can press & hold the left mouse button to rotate the view.

## Technical notes

* We rotate the movement vector created by the keyboard commands by the pitch/yaw values so that they are relative to the view direction rather than be fixed in world coordinates.

* The view can never be exactly straight up or straight down to avoid a problem known as
[gimbal lock](https://en.wikipedia.org/wiki/Gimbal_lock) which can cause the camera view to flip unexpectedly or tumble uncontrollably.

* This control implementation is very simple, and really only works on a system with fast frame-rate. For a more robust implementation, the ``Update`` code should make use of ``elapsedTime`` to scale the keyboard movement rate values and ``ROTATION_GAIN`` / ``MOVEMENT_GAIN`` adjusted accordingly.

# Detecting button & key transitions

The controls above were all written using 'instantaneous state' where we only cared when a key or mouse button was up or down. For many controls, you want to trigger an event at the moment a key/button is pressed or released.

In the **Game.h** file, add the following variables  to the bottom of the Game class's private declarations:

```cpp
DirectX::Keyboard::KeyboardStateTracker m_keys;
DirectX::Mouse::ButtonStateTracker m_mouseButtons;
```

In **Game.cpp**, add to the TODO of **OnResuming** and **OnActivated**:

```cpp
m_keys.Reset();
m_mouseButtons.Reset();
```

In **Game.cpp**, modify the TODO section of **Update**:

```cpp
auto kb = m_keyboard->GetState();
m_keys.Update(kb);

if (kb.Escape)
...
```

```cpp
auto mouse = m_mouse->GetState();
m_mouseButtons.Update(mouse);

if (mouse.positionMode == Mouse::MODE_RELATIVE)
...
```

Add to the end of **Update**:

```cpp
if (m_keys.pressed.Tab || m_mouseButtons.rightButton == Mouse::ButtonStateTracker::PRESSED)
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

Build and run. Pressing the ``Tab`` key or the right mouse button will cycle the color of the room through Red, Green, Blue, and White. Holding down the Tab or mouse button does not flash though the colors with the framerate.

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
    constexpr float ROTATION_GAIN = 0.004f;
    constexpr float MOVEMENT_GAIN = 0.07f;

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
auto mouse = m_mouse->GetState();
m_mouseButtons.Update(mouse);

m_radius -= float(mouse.scrollWheelValue) * ROTATION_GAIN;
m_mouse->ResetScrollWheelValue();
m_radius = std::max(c_minRadius, std::min(c_maxRadius, m_radius));

if (mouse.positionMode == Mouse::MODE_RELATIVE)
{
    Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f)
        * ROTATION_GAIN;

    m_phi -= delta.y;
    m_theta -= delta.x;
}

m_mouse->SetMode(mouse.leftButton ?
  Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

auto kb = m_keyboard->GetState();

m_keys.Update(kb);

if (kb.Escape)
{
    ExitGame();
}

if (kb.Home)
{
    m_theta = 0.f;
    m_phi = c_defaultPhi;
    m_radius = c_defaultRadius;
}

Vector3 move = Vector3::Zero;

if (kb.Up || kb.W)
    move.y += 1.f;

if (kb.Down || kb.S)
    move.y -= 1.f;

if (kb.Left || kb.A)
    move.x += 1.f;

if (kb.Right || kb.D)
    move.x -= 1.f;

if (kb.PageUp || kb.Space)
    move.z += 1.f;

if (kb.PageDown || kb.X)
    move.z -= 1.f;

move *= MOVEMENT_GAIN;

m_phi -= move.y;
m_theta -= move.x;
m_radius += move.z;

// limit pitch to straight up or straight down
constexpr float limit = XM_PIDIV2 - 0.01f;
m_phi = std::max(1e-2f, std::min(limit, m_phi));

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

if (m_keys.pressed.Tab || m_mouseButtons.rightButton == Mouse::ButtonStateTracker::PRESSED)
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

Build and run. The same controls are present, but now move the camera in an 'orbital' path around the origin with the mouse scroll-wheel moving closer/further away to the center.

**Next lessons:** [[Using the SimpleMath library]], [[Adding the DirectX Tool Kit for Audio]]

# Further reading

DirectX Tool Kit docs [[Keyboard]], [[Mouse]]  
[DirectX Tool Kit: Keyboard and Mouse support](https://walbourn.github.io/directx-tool-kit-keyboard-and-mouse-support/)
