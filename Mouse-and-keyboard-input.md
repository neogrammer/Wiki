This lesson will show how to read user input from the mouse and keyboard.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Adding use of mouse and keyboard

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    std::unique_ptr<DirectX::Mouse> m_mouse;

In **Game.cpp**, add to the end of **Initialize**:

    m_keyboard.reset(new Keyboard);
    m_mouse.reset(new Mouse);
    m_mouse->SetWindow(window);

In **Game.cpp**, add to the TODO of **Update**:

    auto kb = m_keyboard->GetState();
    if ( kb.Escape )
        PostQuitMessage(0);

    auto mouse = m_mouse->GetState();

In **Main.cpp**, add to the ``switch`` statement in **WndProc**:

    case WM_ACTIVATEAPP:

        ...

        Keyboard::ProcessMessage(message, wParam, lParam);
        Mouse::ProcessMessage(message, wParam, lParam);
        break;

    ...

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
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard::ProcessMessage(message, wParam, lParam);
        break;

Build and run. The application does not display anything, but you can use the Escape key to exit.

# Adding a simple scene
Start by saving [roomtexture.dds](https://github.com/Microsoft/DirectXTK/wiki/roomtexture.dds) into your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "roomtexture.dds" and click "OK".

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

    std::unique_ptr<DirectX::GeometricPrimitive> m_room;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roomTex;
    DirectX::SimpleMath::Matrix m_proj;
    DirectX::SimpleMath::Vector3 m_cameraPos;

At the top of **Game.cpp** after the ``using`` statements, add:

    static const XMVECTORF32 START_POSITION = { 0.f, -1.5f, 0.f, 0.f };
    static const XMVECTORF32 ROOM_BOUNDS = { 8.f, 6.f, 12.f, 0.f };

In **Game.cpp** file, add to the **Game** constructor:

    m_cameraPos = START_POSITION.v;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    m_room = GeometricPrimitive::CreateBox(m_d3dContext.Get(),
        XMFLOAT3(ROOM_BOUNDS[0], ROOM_BOUNDS[1], ROOM_BOUNDS[2]),
        false, true);

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(m_d3dDevice.Get(), L"roomtexture.dds",
        nullptr, m_roomTex.ReleaseAndGetAddressOf()));

In **Game.cpp**, add to the TODO of **CreateResources**:

    m_proj = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(70.f),
        float(backBufferWidth) / float(backBufferHeight), 0.01f, 100.f);

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_room.reset();
    m_roomTex.Reset();

In **Game.cpp**, add to the TODO of **Render**:

    XMVECTOR lookAt = XMVectorAdd(m_cameraPos, Vector3::Backward);

    XMMATRIX view = XMMatrixLookAtRH(m_cameraPos, lookAt, Vector3::Up);

    m_room->Draw(Matrix::Identity, view, m_proj, Colors::White, m_roomTex.Get());

Build and run, and you should get the following screen:

![Screenshot of room](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotRoom.PNG)

> _Troubleshooting:_ If you get a runtime exception, then you may have the "roomtexture.dds" in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``CreateDDSTextureFromFile`` and step into the code to find the exact problem.

> **UNDER CONSTRUCTION**

**Next lessons:** [[Using the SimpleMath library]], [[Adding the DirectX Tool Kit for Audio]]

# Further reading

DirectX Tool Kit docs [[Keyboard]], [[Mouse]]
