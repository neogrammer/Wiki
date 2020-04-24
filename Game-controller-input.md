This lesson will show how to read user input from game controllers.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

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
auto state = m_gamePad->GetState(0);

if (state.IsConnected())
{
    // TODO: Read controller 0 here
}
```

Build and run. The application does not display anything or respond to input, but if a Xbox 360 Common Controller or Xbox One controller is plugged into the PC, then it will be detected.

# Detecting game controller button presses

Here we wire up the _View_ button (know as the _Back_ button on Xbox 360 Controllers) to exit the application.

In **Game.cpp**, modify the TODO section of *Update*:

```cpp
auto state = m_gamePad->GetState(0);

if (state.IsConnected())
{
    if (state.IsViewPressed())
    {
        ExitGame();
    }
}
```

Build and run. The application exits when you press _View_ / _Back_.

# Using vibration

In **Game.cpp**, modify the TODO section of **Update**:

```cpp
auto state = m_gamePad->GetState(0);

if (state.IsConnected())
{
    if (state.IsViewPressed())
    {
        ExitGame();
    }
    else
    {
        float left = (state.IsAPressed()) ? 1.f : 0;
        float right = (state.IsBPressed()) ? 1.f : 0;

        m_gamePad->SetVibration(0, left, right);
    }
}
```

Build and run. If you press and hold A or B, you get vibration motors of the controller to activate.

# Reading game controller position

In this version, we tie the left and right triggers to the vibration which provides an example of reading position information from a game controller.

In **Game.cpp**, modify the TODO section of **Update**:

```cpp
if (state.IsConnected())
{
    if (state.IsViewPressed())
    {
        ExitGame();
    }
    else
    {
        m_gamePad->SetVibration( 0, state.triggers.left, state.triggers.right );
    }
}
```

Build and run. Slowly depress the left and right triggers to feel the vibration motor change intensity.

# Detecting single button presses

Because we are checking the button states each frame, you need to track the button state to track transitions like "the button just went down" rather than "the button is down now".

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

```cpp
DirectX::GamePad::ButtonStateTracker m_buttons;
```

In **Game.cpp**, add to the TODO of **OnResuming** and **OnActivated**:

```cpp
m_buttons.Reset();
```

In **Game.cpp**, modify the TODO section of *Update*:

```cpp
if (state.IsConnected())
{
    if (state.IsViewPressed())
    {
        ExitGame();
    }
    m_buttons.Update(state);
    if (m_buttons.a == GamePad::ButtonStateTracker::PRESSED)
    {
        // A was up last frame, it just went down this frame
    }
    if (m_buttons.b == GamePad::ButtonStateTracker::RELEASED)
    {
        // B was down last frame, it just went up this frame
    }
}
else
{
    m_buttons.Reset();
}
```

**Next lessons:** [[Mouse and keyboard input]]

# Further reading

DirectX Tool Kit docs [[GamePad]]  
[DirectX Tool Kit: Now with GamePads](https://walbourn.github.io/directx-tool-kit-now-with-gamepads/)  
[XInput and Windows 8](https://walbourn.github.io/xinput-and-windows-8/)
