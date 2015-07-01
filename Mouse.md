This is a helper for simplified mouse tracking modeled after the [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.input.mouse.aspx) Mouse class.

# Header 
    #include <Mouse.h>

# Initialization
Mouse is a singleton.

    std::unique_ptr<Mouse> mouse( new Mouse );

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr``.

# Integration
For Windows desktop applications, the application needs to make the appropriate calls during the main **WndProc** message processing:

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
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

For universal Windows apps or Windows Store apps, you need to call **SetWindow** and **SetDpi** in the appropriate places.

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

# Remark

The mouse scroll wheel value is accumulated. To reset the value to 0, use **ResetScrollWheelValue**.

# Threading model
The Mouse class should be thread-safe with the exception of the **ProcessMessage** which should only be called in your windows message loop.

# Platform notes
The Xbox One platform doesn't support pointer or mouse input devices.

For Windows Store, universal Windows apps, and Windows phone touch/pointer devices are captured as mouse movement.
