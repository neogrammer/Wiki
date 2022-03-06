|[[DirectXTK]]|[DirectXTK12](https://github.com/microsoft/DirectXTK12/wiki/DirectXTK)|
|---|---|

This is a helper for simplified mouse tracking modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Input``) Mouse class.

![Microsoft Sculpt Mobile Mouse - Blue](https://github.com/Microsoft/DirectXTK/wiki/images/sculptmobilemouse.jpg)

**Related tutorial**: [[Mouse and keyboard input]]

# Header
```cpp
#include <Mouse.h>
```

# Initialization
Mouse is a singleton.

```cpp
std::unique_ptr<Mouse> mouse;
mouse = std::make_unique<Mouse>();
```

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr``.

# Integration

## Windows desktop
The application needs to call **SetWindow** and make calls during the main ``WndProc`` message processing to **ProcessMessage**:

```cpp
#include <Windows.h>
#include "Mouse.h"

mouse->SetWindow(hWnd);

...

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
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
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
```

One additional message is optional depending on desired application behavior. When your application has lost focus, and the user "click activates" it with the mouse, you will see that first mouse button event come through the Mouse interface. If you want the system to automatically ignore that first mouse button click when reactivating the window this way, add the following to your ``WndProc``:

```cpp
    case WM_MOUSEACTIVATE:
        // When you click to activate the window, we want Mouse to ignore that event.
        return MA_ACTIVATEANDEAT;
```

## Universal Windows Platform (UWP) apps
You need to call **SetWindow** and **SetDpi** in the appropriate places.

### C++/CX (/ZW)

```cpp
void ViewProvider::SetWindow(CoreWindow^ window)
{
    mouse->SetWindow(window);

    auto currentDisplayInformation = DisplayInformation::GetForCurrentView();
    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &ViewProvider::OnDpiChanged);

    mouse->SetDpi(currentDisplayInformation->LogicalDpi);
}

void ViewProvider::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
    Mouse::SetDpi(sender->LogicalDpi);
}
```

### C++/WinRT

```cpp
void ViewProvider::SetWindow(winrt::Windows::UI::Core::CoreWindow window window)
{
    mouse->SetWindow(window);

    auto currentDisplayInformation = DisplayInformation::GetForCurrentView();
    currentDisplayInformation.DpiChanged({ this, &ViewProvider::OnDpiChanged });

    mouse->SetDpi(currentDisplayInformation.LogicalDpi());
}

void ViewProvider::OnDpiChanged(DisplayInformation const & sender, IInspectable const & /*args*/)
{
    Mouse::SetDpi(sender.LogicalDpi());
}
```

> You only get the C++/WinRT ``SetWindow`` helper function if you have included ``winrt/Windows.UI.Core.h`` before you include ``Mouse.h``. Alternatively, you can always just do what the helper does for you:

```cpp
mouse->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(winrt::get_abi(window)));
```

## Xbox

For Microsoft GDKX when targeting Gaming.Xbox.*.x64, you follow the pattern above for *Windows desktop*, although only a subset of Win32 messages are required (i.e. it does not use ``WM_INPUT``, ``WM_MOUSEHOVER``, or ``WM_MOUSEACTIVATE``):

```cpp
#include <Windows.h>
#include "Mouse.h"

mouse->SetWindow(hWnd);

...

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
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
        Mouse::ProcessMessage(message, wParam, lParam);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
```

If using 4k or 1440p instead of 1080p, be sure to call **SetResolution** as well:

```cpp
float uiScale = 1.f;
if (height == 2160)  uiScale = 2.f;
else if (height == 1440)  uiScale = 1.333333f;
Mouse::SetResolution(uiScale);
```

For the Xbox One XDK, you follow the pattern above for *Universal Windows Platform (UWP) apps*. You need to use **SetDpi** for 4k instead of 1080p:

```cpp
Mouse::SetDpi((width == 3840) ? 192.f : 96.f);
```

# Basic use

**GetState** queries the current state of the mouse.

```cpp
auto state = mouse->GetState();

if ( state.leftButton )
   // Left button is down

XMFLOAT2 mousePosInPixels( float(m.x), float(m.y) );
// This is the absolute position of the mouse relative
// to the upper-left corner of the window
```

> Since Mouse is a singleton, you can make use of the static method **Get** if desired: ``auto state = Mouse::Get().GetState()``

# Button state tracker

A common pattern is to trigger an action when a mouse button is pressed or released, but you don't want to trigger the action every single frame if the button is held down for more than a single frame. This helper class simplifies this.

```cpp
Mouse::ButtonStateTracker tracker;

...

auto state = mouse->GetState();
tracker.Update( state );

if ( tracker.rightButton == Mouse::ButtonStateTracker::PRESSED )
    // Take an action when Right mouse button is first pressed,
    // but don't do it again until the button is released and
    // then pressed again
```

Each button is reported by the tracker with a state:

* ``PRESSED``: Indicates that the button was just pushed down since the previous ``Update``.
* ``RELEASED``: Indicates that the button was just let up since the previous ``Update``.
* ``UP``: This indicates the button has been up both for this ``Update`` and the previous one.
* ``HELD``: This indicates the button has been held down both for this ``Update`` and the previous one.

> The ``UP`` and ``HELD`` states are for convenience and readability as they provide exactly the same information as the result from ``Mouse::GetState``.

You may find that using ``Mouse::ButtonStateTracker::PRESSED`` is a bit verbose. You can simplify the code by doing:

```cpp
using ButtonState = Mouse::ButtonStateTracker::ButtonState;

if ( tracker.rightButton == ButtonState::PRESSED )
    // Take an action when Right mouse button is first pressed,
    // but don't do it again until the button is released and
    // then pressed again
```

> When resuming from a pause or suspend, be sure to call **Reset** on the tracker object to clear the state history.

# Absolute vs. Relative Mouse position

By default, the mouse state is returned as an absolute pixel location in the ``x`` and ``y`` values of ``State``. For 'mouse-look' behavior in games, however, relative mouse movement is desired. While there are some older tricks for emulating this with absolute pixel locations and computing deltas, there are better options which are implemented by Mouse.

Control of the mode is set by **SetMode** passing either ``MODE_ABSOLUTE`` (the default) or ``MODE_RELATIVE``. The current mode is returned in ``State`` in the ``positionMode`` value to inform your input code locally the mode of the ``x``, ``y`` values.

> Note: While in the *relative* mode, you should call ``GetState`` only once per frame as this resets the delta ``x`` and ``y`` values on return.

Here, we are using relative movement whenever the left mouse button is held down:

```cpp
auto state = g_mouse->GetState();
if (state.positionMode == Mouse::MODE_RELATIVE)
{
    // state.x and state.y are relative values; system cursor is not visible
}
else
{
    // state.x and state.y are absolute pixel values; system cursor is visible
}

tracker.Update(state);

if (tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
{
    mouse->SetMode(Mouse::MODE_RELATIVE);
}
else if (tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
{
    mouse->SetMode(Mouse::MODE_ABSOLUTE);
}
```

> When using ``MODE_RELATIVE``, the system cursor is hidden so a user can't navigate away to another monitor or app or even exit using the mouse--they can of course use keyboard shortcuts (<kbd>Alt</kbd>+<kbd>TAB</kbd> or <kbd>Alt</kbd>+<kbd>F4</kbd>). If your game makes use of 'mouse-look' controls, you should ensure that a simple key (like the <kbd>Esc</kbd> key) returns to the game's menu/pause screen and that needs to restore ``MODE_ABSOLUTE`` behavior.

# Cursor Visibility

The system cursor is shown by default on Windows desktop systems. Mouse will hide the cursor when using relative mode, but restores it when returning to absolute mode.

The property **IsVisible** can be checked at any time. Use **SetVisible** to hide/show the cursor while in absolute mode.

# Remark

The mouse scroll wheel value is accumulated. To reset the value to 0, use **ResetScrollWheelValue**.

The **IsConnected** method can be used to test if a mouse device is present on the system.

# Threading model
The Mouse class should be thread-safe with the exception of the **ProcessMessage** which should only be called in your windows message loop.

# Platform notes

For Universal Windows Platform apps, touch/pointer devices are captured as [mouse movement](https://docs.microsoft.com/en-us/windows/uwp/gaming/tutorial--adding-touch-controls-to-your-directx-game). Touch/pointer devices do not, however, result in changes to button state. Relative mouse movement is captured per this [Microsoft Docs](https://docs.microsoft.com/en-us/windows/uwp/gaming/relative-mouse-movement) article.

> For UWP applications on Xbox One, the game controller can be made to emulate a mouse which will provide input through the Mouse class, but the input paradigm is more natural if you use the [[GamePad]] class directly.

For Windows desktop apps, relative mouse movement is captured using "raw input" per the article [Taking Advantage of High-Definition Mouse Movement](https://docs.microsoft.com/en-us/windows/desktop/DxTechArts/taking-advantage-of-high-dpi-mouse-movement). Note that a consequence of this implementation is that relative mouse movement is not available when using the application through Remote Desktop.

# Further reading
[DirectX Tool Kit: Keyboard and Mouse support](https://walbourn.github.io/directx-tool-kit-keyboard-and-mouse-support/)  
[Mouse Input](https://docs.microsoft.com/en-us/windows/desktop/inputdev/mouse-input)  
