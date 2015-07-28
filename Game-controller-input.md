This lesson will show how to read user input from game controllers.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Adding use of a gamepad

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

    std::unique_ptr<DirectX::GamePad> m_gamePad;

In **Game.cpp**, add to the end of **Initialize**:

    m_gamePad.reset(new GamePad());

In **Game.cpp**, add to the TODO of **OnSuspending** and **OnDeactivated**:

    m_gamePad->Suspend();

In **Game.cpp**, add to the TODO of **OnResuming** and **OnActivated**:

    m_gamePad->Resume();

In **Game.cpp**, add to the TODO of **Update**:

    auto state = m_gamePad->GetState(0);

    if (state.IsConnected())
    {
        // TODO: Read controller 0 here
    }

Build and run. The application does not display anything or respond to input, but if a Xbox 360 Common Controller or Xbox One controller is plugged into the PC, then it will be detected.

# Detecting game controller button presses

Here we wire up the _View_ button (know as the _Back_ button on Xbox 360 Controllers) to exit the application.

In **Game.cpp**, modify the TODO section of *Update*:

    auto state = m_gamePad->GetState(0);

    if (state.IsConnected())
    {
        if (state.IsViewPressed())
        {
            PostQuitMessage(0); // Win32 desktop API for exiting the application
        }
    }

Build and run. The application exits when you press _View_ / _Back_.

# Using vibration

In **Game.cpp**, modify the TODO section of **Update**:

    auto state = m_gamePad->GetState(0);

    if (state.IsConnected())
    {
        if (state.IsViewPressed())
        {
            PostQuitMessage(0);
        }
        else
        {
            float left = (state.IsAPressed()) ? 1.f : 0;
            float right = (state.IsBPressed()) ? 1.f : 0;

            m_gamePad->SetVibration(0, left, right);
        }
    }


Build and run. If you press and hold A or B, you get vibration motors of the controller to activate.

# Reading game controller position

In this version, we tie the left and right triggers to the vibration which provides an example of reading position information from a game controller.

In **Game.cpp**, modify the TODO section of **Update**:

    if (state.IsConnected())
    {
        if (state.IsViewPressed())
        {
            PostQuitMessage(0);
        }
        else
        {
            m_gamePad->SetVibration( 0, state.triggers.left, state.triggers.right );
        }

Build and run. Slowly depress the left and right triggers to feel the vibration motor change intensity.

# Detecting single button presses

Because we are checking the button states each frame, you need to track the button state to track transitions like "the button just went down" rather than "the button is down now".

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

    DirectX::GamePad::ButtonStateTracker m_buttons;

In **Game.cpp**, add to the TODO of **OnResuming** and **OnActivated**:

    m_buttons.Reset();

In **Game.cpp*, modify the TODO section of *Update*:

    if (state.IsConnected())
    {
        if (state.IsViewPressed())
        {
            PostQuitMessage(0);
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

**Next lessons:** [[Mouse and keyboard input]]

# Further reading

DirectX Tool Kit docs [[GamePad]]  
[DirectX Tool Kit: Now with GamePads](http://blogs.msdn.com/b/chuckw/archive/2014/09/05/directx-tool-kit-now-with-gamepads.aspx)  
[XInput and Windows 8](http://blogs.msdn.com/b/chuckw/archive/2012/04/26/xinput-and-windows-8-consumer-preview.aspx)

