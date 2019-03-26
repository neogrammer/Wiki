This is a helper for simplified access to gamepad controllers modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Input``) GamePad class.

![Xbox 360 Common Controller](https://github.com/Microsoft/DirectXTK/wiki/images/xbox360controller.png)

**Related tutorial:** [[Game controller input]]

# Header
```cpp
#include <GamePad.h>
```

# Initialization
GamePad is a singleton.

```cpp
std::unique_ptr<GamePad> gamepad;
gamepad = std::make_unique<GamePad>();
```

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr``.

# Basic use
**GetState** queries the controller status given a _player_ index. If connected, it returns the status of the buttons (A, B, X, Y, left & right stick, left & right shoulder, back, and start), the directional pad (DPAD), the left & right thumb sticks, and the left & right triggers.

```cpp
auto state = gamePad->GetState( 0 );

if ( state.IsConnected() )
{
    if ( state.IsAPressed() )
        // Do action for button A being down

    if ( state.buttons.y )
        // Do action for button Y being down

    if ( state.IsDPadLeftPressed() )
        // Do action for DPAD Left being down

    if ( state.dpad.up || state.dpad.down || state.dpad.left || state.dpad.right )
        // Do action based on any DPAD change

    float posx = state.thumbSticks.leftX;
    float posy = state.thumbSticks.leftY;
        // These values are normalized to -1 to 1

    float throttle = state.triggers.right;
        // This value is normalized 0 -> 1

    if ( state.IsLeftTriggerPressed() )
        // Do action based on a left trigger pressed more than halfway

    if ( state.IsViewPressed() )
        // This is an alias for the Xbox 360 'Back' button
        // which is called 'View' on the Xbox One controller.
}
```

The valid range for _player_ is 0 to ``GamePad::MAX_PLAYER_COUNT - 1``. Outside that range, the state is always reported as disconnected.

> If _player_ is passed as -1, then GamePad will use the most recently connected gamepad. This is not a good usage behavior for games or apps, but is useful for tests and samples where you don't have "press a key to start" logic for handling multiple gamepads.

> Since GamePad is a singleton, you can make use of the static method **Get** if desired: ``auto state = GamePad::Get().GetState()``

# Dead zones
GamePad implements the same dead zone scheme as XNA.

* ``DEAD_ZONE_INDEPENDENT_AXES`` which is the default
* ``DEAD_ZONE_CIRCULAR`` which provides a deadzone for the combined X/Y axes
* ``DEAD_ZONE_NONE`` which provides 'raw' scaled information to allow the application to implement dead zones

For example:

```cpp
auto state = gamePad->GetState( 0, GamePad::DEAD_ZONE_CIRCULAR );
```

See [Shawn's blog](http://www.shawnhargreaves.com/blog/gamepads-suck.html) for details.

# Vibration
Many controllers include vibration motors to provide force-feedback to the user, which can be controlled with **SetVibration** and the _player_ index. The motor values range from 0 to 1.

```cpp
if ( gamePad->SetVibration( 0, 0.5f, 0.25f ) )
    // If true, the vibration was successfully set.
```

# Device capabilities
The GamePad class provides a simplified model for the device capabilities.

```cpp
auto caps = gamePad->GetCapabilities( 0 );
if ( caps.IsConnected() )
{
    if ( caps.gamepadType == GamePad::Capabilities::FLIGHT_STICK )
        // Use specific controller layout based on a flight stick controller
    else
        // Default to treating any unknown type as a standard gamepad
}
```

> Much of the information reported in ``XINPUT_CAPABILITIES`` is omitted. This is because much of it is unreliable (see below), but also because the actionable information is entirely captured by the gamepadType subtype.

# Application focus
Unlike mouse or keyboard input on Windows, XInput has 'global' focus when reading the game controller. Therefore, applications should ignore the input when in the background. To implement this, you can call the following method when you lose focus, which will shut off any active vibration and then return 'neutral' data for all connected gamepads.

```cpp
gamePad->Suspend();
```

When focus is returned to the application, call the following method to restore any active vibration and read gamepad data again.

```cpp
gamePad->Resume();
```

# Button state tracker

A common pattern for gamepads is to trigger an action when a button is pressed or released, but you don't want to trigger the action every single frame if the button is held down for more than a single frame. This helper class simplifies this.

```cpp
GamePad::ButtonStateTracker tracker;

...

auto state = gamePad->GetState( 0 );
if ( state.IsConnected() )
{
    tracker.Update( state );

    if ( tracker.a == GamePad::ButtonStateTracker::PRESSED )
        // Take an action when Button A is first pressed, but don't do it again until
        // the button is released and then pressed again
}
else
{
    tracker.Reset();
}
```

Each button is reported by the tracker with a state ``UP``, ``HELD``, ``PRESSED``, or ``RELEASED``.

You may find that using ``GamePad::ButtonStateTracker::PRESSED`` is a bit verbose. You can simplify the code by doing:

```cpp
if ( state.IsConnected() )
{
    tracker.Update( state );

    using ButtonState = GamePad::ButtonStateTracker::ButtonState;

    if ( tracker.a == ButtonState::PRESSED )
        // Take an action when Button A is first pressed, but don't do it again until
        // the button is released and then pressed again
}
```

> When resuming from a pause or suspend, be sure to call **Reset** on the tracker object to clear the state history.

# Threading model
The GamePad class provides no special synchronization above the underlying API. XInput on Windows is thread-safe through a internal global lock, so performance is best when only a single thread accesses the controller.

# Remark
The GamePad object and the underlying XInput APIs are polling based. Typically the controller is read once per render frame, which means that low frame rate can result in missing user input.

# Platform notes

## Windows
For the Windows platform, the GamePad class ensures that attempts to locate unconnected controllers do not happen too frequently to avoid a potential performance issue with XInput.

When built for Windows 8.0 or 8.1, it makes use of XInput 1.4 (linking to ``xinput.lib``). When built for down-level support, it makes use of XInput 9.1.0 which avoids the need for any dependancy on the legacy DirectSetup (linking to ``xinput9_1_0.lib``).

When built for Windows 10, it makes use of ``Windows.Gaming.Input``. This class assumes that the client code has called ``Windows::Foundation::Initialize`` as needed.

> For a Universal Windows Platform (UWP) app, the Windows Runtime (and COM generally) is initialized by the C/C++ Run-Time. For a classic Windows desktop application you have to do this explicitly:

```cpp
Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
if (FAILED(initialize))
    // Error
```

Note that subtype capabilities information is somewhat unreliable down-level depending on your exact mix of device and driver, and in some cases is hard-coded. All capabilities information is reliable on Windows 8.0 or later.

XInput supports controllers compatible with the Xbox 360 Common Controller for Windows, the Xbox 360 Wireless Receiver for Windows, and the Xbox One Controller.

Vibration settings for the trigger impulse motors (``leftTrigger``, ``rightTrigger``) on the [Xbox One Controller](http://support.xbox.com/en-US/xbox-one/accessories/controller-pc-compatibility) are not supported by XInput--this is supported by ``Windows.Gaming.Input`` APIs. The "View" button is reported as the "Back" button, and the "Menu" button is reported as the "Start" button.

## Xbox One
On Xbox One, this class is implemented using the _Windows.Xbox.Input_ interfaces rather than XInput. It is abstracted to return the same structures. Here are a few notes:

* state.packet is a timestamp in "Universal time" format.
* ``MAX_PLAYER_COUNT`` is 8 rather than 4
* Currently only the GAMEPAD type is reported for Xbox One controllers

The _player_ index mapping is not correlated directly with a user as it is on Xbox 360 or Windows, and is assigned 'upon arrival'. To determine the actual user for a given gamepad, you should use the controller ID reported as part of the Capabilities.

```cpp
auto caps = gamePad->GetCapabilities( playerIndex );
if ( caps.IsConnected() )
{
    try
    {
        auto ctrl = Controller::GetControllerById( caps.id );
        if ( ctrl )
        {
            User^ user = ctrl->User;
            // user is the user associated with the controller, if any
        }
    }
    catch ( Platform::Exception ^ e )
    {
        // error handling, likely the controller has been removed
        // since the caps were obtained
    }
}
```

## Universal Windows app Platform (UWP)
When built for Windows 10, the GamePad class is implemented using a new WinRT ``Windows.Gaming.Input`` API similar to the Xbox One API. Here are a few notes:

* Full support for ``leftTrigger`` and ``rightTrigger`` motors for the Xbox One controller on Windows.
* ``MAX_PLAYER_COUNT`` is 8 rather than 4
* Currently only the GAMEPAD type is reported for Xbox One controllers, and ``caps.id`` is always 0.

> Technically the ``Windows.Gaming.Input`` API can support more than 8 devices, but the _DirectX Tool Kit_ ``GamePad`` implementation can only support up to ``MAX_PLAYER_COUNT``.

## UWP on Xbox One

Whenever the B button on a gamepad controller is pressed on Xbox One, the running UWP app is sent a "back request" (like the hardware 'Back' button on Windows Mobile). If this is unhandled, the application will be suspended and the previous application is brought forward. This can make using the B button in your UI design a challenge, so the recommended solution is to add a message handler to 'handle' the request:

### C++/CX
```cpp
void SetWindow(CoreWindow^ window)
{
    ...
    auto navigation = Windows::UI::Core::SystemNavigationManager::GetForCurrentView();

    navigation->BackRequested +=
        ref new EventHandler<BackRequestedEventArgs^>(this, &ViewProvider::OnBackRequested);
    ...
}

void OnBackRequested(Platform::Object^, Windows::UI::Core::BackRequestedEventArgs^ args)
{
    // UWP on Xbox One triggers a back request whenever the B button is pressed
    // which can result in the app being suspended if unhandled
    args->Handled = true;
}
```

### C++/WinRT
```cpp
#include <winrt/Windows.UI.Core.h>

// UWP on Xbox One triggers a back request whenever the B button is pressed
// which can result in the app being suspended if unhandled
using namespace winrt::Windows::UI::Core;

auto navigation = SystemNavigationManager::GetForCurrentView();

navigation.BackRequested([](const winrt::Windows::Foundation::IInspectable&, const BackRequestedEventArgs& args)
{
    args.Handled(true);
});
```

# Further reading
[DirectX Tool Kit: Now with GamePads](https://walbourn.github.io/directx-tool-kit-now-with-gamepads/)  
[XInput and Windows 8](https://walbourn.github.io/xinput-and-windows-8/)  
[XInput and XAudio2](https://walbourn.github.io/xinput-and-xaudio2/)  
[Xbox 360 Controller Images](https://github.com/SimonDarksideJ/XNAGameStudio/wiki/Xbox-360-Controller-Images)  
[Xbox 360 Controller Buttons](https://github.com/SimonDarksideJ/XNAGameStudio/wiki/Xbox-360-Controller-Button-Images)
