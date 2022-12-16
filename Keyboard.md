|[[DirectXTK]]|[DirectXTK12](https://github.com/microsoft/DirectXTK12/wiki/DirectXTK)|
|---|---|

This is a helper for simplified keyboard state tracking modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Input``) Keyboard class.

![Microsoft Wireless Keyboard 800](https://github.com/Microsoft/DirectXTK/wiki/images/wirelesskeyboard.jpg)

**Related tutorial**: [[Mouse and keyboard input]]

# Header
```cpp
#include <Keyboard.h>
```

# Initialization
Keyboard is a singleton.

```cpp
std::unique_ptr<Keyboard> keyboard;
keyboard = std::make_unique<Keyboard>();
```

For exception safety, it is recommended you make use of the C++ [RAII](http://wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr``.

# Integration

## Windows desktop
For Windows desktop applications, the application needs to make the appropriate calls during the main **WndProc** message processing:

```cpp
#include <Windows.h>
#include "Keyboard.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
        Keyboard::ProcessMessage(message, wParam, lParam);
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
            // This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
            ...
        }
        Keyboard::ProcessMessage(message, wParam, lParam);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard::ProcessMessage(message, wParam, lParam);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
```

You also want the following in your ``WndProc`` to avoid getting a 'beep' sound for key inputs assumed to be unsupported menu accelerator keys:

```cpp
    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
```

## Universal Windows Platform (UWP) apps
For Universal Windows Platform apps, you need to call **SetWindow** in the appropriate place.

### C++/CX (/ZW)

```cpp
void ViewProvider::SetWindow(CoreWindow^ window)
{
    keyboard->SetWindow(window);
}
```

### C++/WinRT

```cpp
void ViewProvider::SetWindow(winrt::Windows::UI::Core::CoreWindow window window)
{
    keyboard->SetWindow(window);
}
```

> You only get the C++/WinRT ``SetWindow`` helper function if you have included ``winrt/Windows.UI.Core.h`` before you include ``Keyboard.h``. Alternatively, you can always just do what the helper does for you:

```cpp
keyboard->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(winrt::get_abi(window)));
```

## Xbox

For Microsoft GDKX when targeting Gaming.Xbox.*.x64, no additional integration is required.

For the Xbox One XDK, you follow the pattern above for *Universal Windows Platform (UWP) apps*.

# Basic use

**GetState** queries the current state of the keyboard.

```cpp
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
```

> Since Keyboard is a singleton, you can make use of the static method **Get** if desired: ``auto kb = Keyboard::Get().GetState()``

# Key state

The ``Keys`` enumeration and ``State`` structure is a mapping of the virtual keys rather than alphanumeric values, so the number 0 from the keyboard top-row and the 0 on the 10-key keypad are distinct, as are the left shift vs. right shift, etc.

There is no specific virtual key for lower-case (i.e. ``a``) vs. upper-case (i.e. ``A``). You have to determine this from tracking the state of <kbd>CapsLock</kbd> and <kbd>Shift (left/right)</kbd> when you get the <kbd>A</kbd> key down. The same is true for ``+`` vs. ``=`` based on <kbd>NumLock</kbd> and <kbd>Shift (left/right)</kbd>.

Due to limitations of C identifiers, the keyboard's top-row digits <kbd>1</kbd> through <kbd>9</kbd> and <kbd>0</kbd> are ``D1`` through ``D9`` and ``D0``. The number keypad digits are ``NumPad0`` through ``NumPad9``.

Most of the ``Oem*`` values vary based on country/region, but you can count on ``OemPlus`` (<kbd>+</kbd>), ``OemComma`` (<kbd>,</kbd>), ``OemMinus`` (<kbd>-</kbd>), and ``OemPeriod``  (<kbd>.</kbd>) being the same for all layouts.

*XNA Game Studio* and the Keyboard enumeration both use the names ``OemSemicolon`` for ``VK_OEM_1``, ``OemQuestion`` for ``VK_OEM_2``, and ``OemTilde`` for ``VK_OEM_3`` as they map to the <kbd>: ;</kbd>, <kbd>? /</kbd>, and <kbd>~ `</kbd> keys on the US keyboard.

> The <kbd>Windows</kbd> logo key is visible in the Keyboard state, but you should avoid using it for your control scheme. <kbd>Windows</kbd> logo key is normally a system-wide hotkey for Windows per [Microsoft Docs](https://support.microsoft.com/en-us/windows/keyboard-shortcuts-in-windows-dcc61a57-8ff0-cffe-9796-cb9706c75eec). In particular on Windows 10 or later, <kbd>Windows</kbd>+<kbd>G</kbd> brings up the [Xbox Game Bar](https://developer.microsoft.com/en-us/games/products/game-bar/), <kbd>Windows</kbd>+<kbd>Alt</kbd>+<kbd>B</kbd> is used to toggle HDR mode, <kbd>Windows</kbd>+<kbd>Alt</kbd>+<kbd>PrtScn</kbd> is used to capture a screenshot, and <kbd>Windows</kbd>+<kbd>Alt</kbd>+<kbd>R</kbd> is used to record a video with *Xbox Game Bar*.

> If accidentally pressing the <kbd>Windows</kbd> key or triggering the Windows Accessibility hotkeys is a common problem for your control scheme, see [Disabling Shortcut Keys in Games](https://docs.microsoft.com/windows/win32/dxtecharts/disabling-shortcut-keys-in-games).

For more information on virtual keys, see [Microsoft Docs](https://docs.microsoft.com/windows/win32/inputdev/virtual-key-codes).

# Keyboard state tracker

A common pattern is to trigger an action when a key is pressed or released, but you don't want to trigger the action every single frame if the key is held down for more than a single frame. This helper class simplifies this.

```cpp
Keyboard::KeyboardStateTracker tracker;

...

auto state = keyboard->GetState();
tracker.Update( state );

if ( tracker.pressed.Space )
    // Space was just pressed down

if ( tracker.IsKeyReleased( VK_F1 ) )
    // F1 key was just released
```

The ``pressed`` member of the state tracker indicates each key that was just pushed down since the previous ``Update``. The helper ``IsKeyPressed`` can be used to check this state.

The ``released`` member of the state tracker indicates each key that was just went up since the previous ``Update``. The helper ``IsKeyReleased`` can be used to check this state.

You can determine which keys are 'down' vs. 'up' by looking at the original ``Keyboard::GetState`` to get the equivalent of the Mouse/GamePad ``HELD`` vs. ``UP`` tracker states.

> When resuming from a pause or suspend, be sure to call **Reset** on the tracker object to clear the state history.

# Threading model
The Keyboard class should be thread-safe with the exception of the **ProcessMessage** which should only be called in your windows message loop.

# Remarks
This helper is intended for game controls tied to the keyboard. For chat input and editable text, you should use the platform-specific methods for text input.

Due to some quirks of the platform, If you press both Left & Right <kbd>Shift</kbd> keys at the same time, they will both appear down until both are released.

The **IsConnected** method can be used to test if a keyboard device is present on the system.

> Note that on some systems, support for a virtual keyboard is reported as 'true' even without a physical keyboard.

# International layouts
Keep in mind when designing the keyboard controls for your game the different layouts of standard keyboards.  In particularly, note the red keys which are in different locations for international keyboards than the traditional English <kbd>Q</kbd><kbd>W</kbd><kbd>E</kbd><kbd>R</kbd><kbd>T</kbd><kbd>Y</kbd> keyboard.

![International Keyboard Layout](https://github.com/Microsoft/DirectXTK/wiki/images/KeyboardInternational.png)

The Keyboard class makes use of virtual keys and not scancodes so your code has to be aware of these layout differences.

[QWERTY](https://wikipedia.org/wiki/QWERTY), [QWERTZ](https://wikipedia.org/wiki/QWERTZ), [AZERTY](https://wikipedia.org/wiki/AZERTY), [QZERTY](https://wikipedia.org/wiki/Keyboard_layout#QZERTY)

# Further reading
[DirectX Tool Kit: Keyboard and Mouse support](https://walbourn.github.io/directx-tool-kit-keyboard-and-mouse-support/)  
[Using Keyboard Input](https://docs.microsoft.com/windows/win32/inputdev/using-keyboard-input)  
