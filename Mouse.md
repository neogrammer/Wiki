This is a helper for simplified mouse tracking modeled after the [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.input.mouse.aspx) Mouse class.

![Microsoft Sculpt Mobile Mouse - Blue](https://github.com/Microsoft/DirectXTK/wiki/images/sculptmobilemouse.jpg)

**Related tutorial**: [[Mouse and keyboard input]]

# Header 
    #include <Mouse.h>

# Initialization
Mouse is a singleton.

    std::unique_ptr<Mouse> mouse( new Mouse );

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr``.

# Integration

## Windows desktop
The application needs to call **SetWindow** and make calls during the main ``WndProc`` message processing to **ProcessMessage**:

    mouse->SetWindow(hWnd);

    ...

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
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

## Universal Windows apps, Windows Store apps
You need to call **SetWindow** and **SetDpi** in the appropriate places.

    void App::SetWindow(CoreWindow^ window)
    {
        mouse->SetWindow(window);
    }

    void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
    {
        mouse->SetDpi(sender->LogicalDpi);
    }

# Basic use

**GetState** queries the current state of the mouse.

    auto state = mouse->GetState();

    if ( state.leftButton )
       // Left button is down

    XMFLOAT2 mousePosInPixels( float(m.x), float(m.y) );
    // This is the absolute position of the mouse relative
    // to the upper-left corner of the window

> Since Mouse is a singleton, you can make use of the static method **Get** if desired: ``auto state = Mouse::Get().GetState()``.

# Button state tracker

A common pattern is to trigger an action when a mouse button is pressed or released, but you don't want to trigger the action every single frame if the button is held down for more than a single frame. This helper class simplifies this.

    std::unique_ptr<Mouse::ButtonStateTracker> tracker(
        new Mouse::ButtonStateTracker );

    ...

    auto state = mouse->GetState();
    tracker->Update( state );

    if ( tracker->rightButton == Mouse::ButtonStateTracker::PRESSED )
        // Take an action when Right mouse button is first pressed,
        // but don't do it again until the button is released and
        // then pressed again

_When resuming from a pause or suspend, be sure to call **Reset** on the tracker object to clear the state history._

# Absolute vs. Relative Mouse position

By default, the mouse state is returned as a absolute pixel location in the ``x`` and ``y`` values of ``State``. For 'mouse-look' behavior in games, however, relative mouse movement is desired. While there are some older tricks for emulating this with absolute pixel locations and computing deltas, there are better options which are implemented by Mouse.

Control of the mode is set by **SetMode** passing either ``MODE_ABSOLUTE`` (the default) or ``MODE_RELATIVE``. The current mode is returned in ``State`` in the ``positionMode`` value to inform your input code locally the mode of the ``x``, ``y`` values.

Here, we are using relative movement whenever the left mouse button is held down:

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

> When using ``MODE_RELATIVE``, the system cursor is hidden so a user can't navigate away to another monitor or app or even exit. If your game makes use of 'mouse-look' controls, you should ensure that a simple key (like the ESC key) returns to the game's menu/pause screen and that needs to restore ``MODE_ABSOLUTE`` behavior.

# Remark

The mouse scroll wheel value is accumulated. To reset the value to 0, use **ResetScrollWheelValue**.

# Threading model
The Mouse class should be thread-safe with the exception of the **ProcessMessage** which should only be called in your windows message loop.

# Platform notes
The Xbox One platform doesn't support pointer or mouse input devices.

For Windows Store, universal Windows apps, and Windows phone touch/pointer devices are captured as mouse movement. Touch/pointer devices do not, however, result in changes to button state. Relative mouse movement is captured per this [MSDN](https://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh994925.aspx) article.

For Windows desktop apps, relative mouse movement is captured using "raw input" per the article [Taking Advantage of High-Definition Mouse Movement](https://msdn.microsoft.com/en-us/library/windows/desktop/ee418864.aspx). Note that a consequence of this implementation is that relative mouse movement is not available when using the application through Remote Desktop.

