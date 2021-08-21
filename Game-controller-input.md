This lesson will show how to read user input from game controllers.

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

# Adding a a simple scene
Start by saving [roomtexture.dds](https://github.com/Microsoft/DirectXTK/wiki/media/roomtexture.dds) into your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "roomtexture.dds" and click "OK".

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::GeometricPrimitive> m_room;
DirectX::SimpleMath::Matrix m_proj;
DirectX::SimpleMath::Vector3 m_cameraPos;
float m_pitch;
float m_yaw;
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
m_proj = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(70.f),
    float(size.right) / float(size.bottom), 0.01f, 100.f);
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
float y = sinf(m_pitch);
float r = cosf(m_pitch);
float z = r * cosf(m_yaw);
float x = r * sinf(m_yaw);

XMVECTOR lookAt = m_cameraPos + Vector3(x, y, z);

XMMATRIX view = XMMatrixLookAtRH(m_cameraPos, lookAt, Vector3::Up);

m_room->Draw(Matrix::Identity, view, m_proj,
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
        CommonStates::CullNone,
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
float y = sinf(m_pitch);
float r = cosf(m_pitch);
float z = r * cosf(m_yaw);
float x = r * sinf(m_yaw);

XMVECTOR lookAt = m_cameraPos + Vector3(x, y, z);

XMMATRIX view = XMMatrixLookAtRH(m_cameraPos, lookAt, Vector3::Up);

ID3D12DescriptorHeap* heaps[] = {
    m_resourceDescriptors->Heap(), m_states->Heap()
};
commandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)),
    heaps);

m_roomEffect->SetMatrices(Matrix::Identity, view, m_proj);
m_roomEffect->SetDiffuseColor(m_roomColor);
m_roomEffect->Apply(commandList);
m_room->Draw(commandList);
```

## Build and run

Build and run, and you should get the following screen:

![Screenshot of room](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotRoom.PNG)

> _Troubleshooting:_ If you get a runtime exception, then you may have the "roomtexture.dds" in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``CreateDDSTextureFromFile`` and step into the code to find the exact problem.

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

const float limit = XM_PI / 2.0f - 0.01f;
m_pitch = std::max(-limit, std::min(+limit, m_pitch));

if (m_yaw > XM_PI)
{
    m_yaw -= XM_PI * 2.f;
}
else if (m_yaw < -XM_PI)
{
    m_yaw += XM_PI * 2.f;
}
```

Build and run. The left thumb stick on the gamepad can be used to adjust the view. If you click the left-stick, it resets the view.

## Technical notes

* The view can never be exactly straight up or straight down to avoid a problem known as
[gimbal lock](https://en.wikipedia.org/wiki/Gimbal_lock) which can cause the camera view to flip unexpectedly or tumble uncontrollably.

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

**Next lessons:** [[Mouse and keyboard input]]

# Further reading

DirectX Tool Kit docs [[GamePad]]  
[DirectX Tool Kit: Now with GamePads](https://walbourn.github.io/directx-tool-kit-now-with-gamepads/)  
[XInput and Windows 8](https://walbourn.github.io/xinput-and-windows-8/)
