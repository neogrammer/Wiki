This is a helper for simplified keyboard state tracking modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Input``) Keyboard class.

![Microsoft Wireless Keyboard 800](https://github.com/Microsoft/DirectXTK/wiki/images/wirelesskeyboard.jpg)

**Related tutorial**: [[Mouse and keyboard input]]

# Header 
    #include <Keyboard.h>

# Initialization
Keyboard is a singleton.

    std::unique_ptr<Keyboard> keyboard;
    keyboard = std::make_unique<Keyboard>();

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr``.

# Integration
For Windows desktop applications, the application needs to make the appropriate calls during the main **WndProc** message processing:

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_ACTIVATEAPP:
            Keyboard::ProcessMessage(message, wParam, lParam);
            break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            Keyboard::ProcessMessage(message, wParam, lParam);
            break;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

For universal Windows apps or Windows Store apps, you need to call **SetWindow** in the appropriate place.

    void App::SetWindow(CoreWindow^ window)
    {
        keyboard->SetWindow(window);
    }

# Basic use

**GetState** queries the current state of the keyboard.

    auto kb = keyboard->GetState();

    if (kb.Back)
        // Backspace key is down

    if (kb.W)
        // W key is down

    if (kb.A)
        // A key is down

    if (kb.S)
        // S key is down

    if (kb.D)
        // D key is down

    if (kb.LeftShift)
        // Left shift key is down

    if (kb.RightShift)
        // Right shift key is down

    if ( kb.IsKeyDown( VK_RETURN ) )
        // Return key is down

> Since Keyboard is a singleton, you can make use of the static method **Get** if desired: ``auto kb = Keyboard::Get().GetState()``

# Keyboard state tracker

A common pattern is to trigger an action when a key is pressed or released, but you don't want to trigger the action every single frame if the key is held down for more than a single frame. This helper class simplifies this.

    Keyboard::KeyboardStateTracker tracker;

    ...

    auto state = keyboard->GetState();
    tracker.Update( state );

    if ( tracker.pressed.Space )
        // Space was just pressed down

    if ( tracker.IsKeyReleased( VK_F1 ) )
        // F1 key was just released

_When resuming from a pause or suspend, be sure to call **Reset** on the tracker object to clear the state history._

# Threading model
The Keyboard class should be thread-safe with the exception of the **ProcessMessage** which should only be called in your windows message loop.

# Remarks
This helper is intended for game controls tied to the keyboard. For chat input and editable text, you should use the platform-specific methods for text input.

Due to some quirks of the platform, If you press both Left & Right Shift keys at the same time, they will both appear down until both are released.

The **IsConnected** method can be used to test if a keyboard device is present on the system.

> Note that on some systems, support for a virtual keyboard is reported as 'true' even without a physical keyboard.

# International layouts
Keep in mind when designing the keyboard controls for your game the different layouts of standard keyboards.  In particularly, note the red keys which are in different locations for international keyboards than the traditional English QWERTY keyboard.

![International Keyboard Layout](https://github.com/Microsoft/DirectXTK/wiki/images/KeyboardInternational.png)

The Keyboard class makes use of virtual keys and not scancodes so your code has to be aware of these layout differences.

[QWERTY](https://en.wikipedia.org/wiki/QWERTY), [QWERTZ](https://en.wikipedia.org/wiki/QWERTZ), [AZERTY](https://en.wikipedia.org/wiki/AZERTY), [QZERTY](https://en.wikipedia.org/wiki/Keyboard_layout#QZERTY)

# Platform notes

For Windows phone, the Keyboard and [[Mouse]] classes exist in the library to avoid the need for conditional compilation. The Keyboard and Mouse class instance on this platform, however, are "null" devices and will not return any input.

> In theory they could be associated with the virtual keyboard on these platforms, but since the purpose of ``Keyboard`` abstraction is to use key presses as a game controller it makes little sense as a virtual keyboard.

# Further reading
[DirectX Tool Kit: Keyboard and Mouse support](http://blogs.msdn.com/b/chuckw/archive/2015/08/06/directx-tool-kit-keyboard-and-mouse-support.aspx)  
[Using Keyboard Input](https://msdn.microsoft.com/en-us/library/windows/desktop/ms646268.aspx)  