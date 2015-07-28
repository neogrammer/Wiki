This lesson will show how to read user input from the mouse and keyboard.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Adding use of mouse and keyboard

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    std::unique_ptr<DirectX::Mouse> m_mouse;

In **Game.cpp**, add to the end of **Initialize**:

    m_keyboard.reset(new Keyboard);
    m_mouse.reset(new Mouse);
    m_mouse->SetWindow(window);

In **Game.cpp**, add to the TODO of **OnResuming** and **OnActivated**:

    m_keyboard->Reset();

In **Game.cpp**, add to the TODO of **Update**:

    auto kb = m_keyboard->GetState();
    if ( kb.Escape )
        PostQuitMessage(0);

    auto mouse = m_mouse->GetState();

In **Main.cpp**, add to the ``switch`` statement in **WndProc**:

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

> **UNDER CONSTRUCTION**

**Next lessons:** [[Using the SimpleMath library]], [[Adding the DirectX Tool Kit for Audio]]

# Further reading

DirectX Tool Kit docs [[Keyboard]], [[Mouse]]
