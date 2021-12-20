This lesson covers the basics of adding the use of *DirectX Tool Kit for Audio*'s audio engine to your project.

# Setup
First create a new project using the instructions from the earlier lessons: [[Using DeviceResources]], then [[Adding the DirectX Tool Kit]], and finally [[Adding the DirectX Tool Kit for Audio]] which we will use for this lesson.

> If using *DirectX Tool Kit for DX12*, use [Using DeviceResources](https://github.com/microsoft/DirectXTK12/wiki/Using-DeviceResources), then [Adding the DirectX Tool Kit](https://github.com/microsoft/DirectXTK12/wiki/Adding-the-DirectX-Tool-Kit), and finally add ``#include <Audio.h>`` to **pch.h**.

# The basic game loop with the audio engine

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::AudioEngine> m_audEngine;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
eflags |= AudioEngine_Debug;
#endif
m_audEngine = std::make_unique<AudioEngine>(eflags);
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
if (!m_audEngine->Update())
{
    // more about this below...
}
```

In **Game.cpp**, add to the TODO of **OnSuspending**:

```cpp
m_audEngine->Suspend();
```

In **Game.cpp**, add to the TODO of **OnResuming**:

```cpp
m_audEngine->Resume();
```

In the **Game.h** file, add the following to the Game class public declarations:

```cpp
~Game();
```

In **Game.cpp**, add after the constructor:

```cpp
Game::~Game()
{
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
}
```

> For Universal Windows apps and Xbox One apps, this destructor is not required.

Build and run. You won't hear anything yet, but we do have XAudio2 up and running.

> _Troubleshooting:_ If you get the link  error 'unresolved external symbol' for ``CreateFX``, ``X3DAudioInitialize``, or ``X3DAudioCalculate``, you may have incorrectly configured the x86 vs. x64 versions of the VC++ Directory library paths in the previous lesson.

## Technical notes
We have an explicit destructor for Game that ensures the audio engine is suspended because of the multi-threaded nature of XAudio2 which directly uses data owned by the application for playback. This makes the code more robust as otherwise it would be very dependent on the order of destruction of _DirectX Tool Kit for Audio_ objects in our Game class.

# Silent mode

With graphics, your application can safely just fail to start up without a suitable video card, but not every system has an audio device. Therefore to simplify audio coding, _DirectX Tool Kit for Audio_ will successfully start even without an audio device being found. This is called 'silent mode'. You won't get callbacks, but creating objects in the library will succeed so you can avoid a lot of messy conditional testing code in your project.

Your application can detect this mode in two ways:  First, after you first create the ``AudioEngine``, you might want to check this via **IsAudioDevicePresent**:

```cpp
if ( !m_audEngine->IsAudioDevicePresent() )
{
    // we are in 'silent mode'.
}
```

You could use this to force the display of 'subtitles' for voice overs and the like.

The second way you can detect this is by **Update** returning false, which indicates that audio is not currently playing. This second case is a bit more complicated because there is another reason you might be not playing audio: the audio device you originally created went away!

```cpp
if (!m_audEngine->Update())
{
    if (m_audEngine->IsCriticalError())
    {
        // We lost the audio device!
    }
}
```

# Losing the audio device

The main reason you can encounter a 'critical error' with XAudio2 is that the currently connected WASAPI end-point has been removed from the system. With Windows, this can be caused by unplugging the speakers/headphones or device driver updates. With Windows phone, this is due to switching between headset and Bluetooth. With Xbox One, this doesn't happen with the default device, but could happen with 'headset' audio.

The application's response to this should be to try a single **Reset** to see if there is a new default device that is available. Otherwise, you'll go to being in 'silent mode'.

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

```cpp
bool m_retryAudio;
```

In the **Game** Ctor, add initialization of ``m_retryAudio``:

```cpp
Game::Game() noexcept(false) :
    m_retryAudio(false)
{
```

In **Initialize**, modify the audio initialization to be:

```cpp
AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
eflags |= AudioEngine_Debug;
#endif
m_audEngine = std::make_unique<AudioEngine>(eflags);
```

In **Update**, modify the audio update to be:

```cpp
if (m_retryAudio)
{
    m_retryAudio = false;
    if (m_audEngine->Reset())
    {
        // TODO: restart any looped sounds here
    }
}
else if (!m_audEngine->Update())
{
    if (m_audEngine->IsCriticalError())
    {
        m_retryAudio = true;
    }
}
```

# Detecting new audio devices

One last case you need to consider for Windows is if your application starts in 'silent mode' but then enables audio later (perhaps by plugging in a headset). Ideally, your 'silent' application should use the new device. You can also have a delay between when you lose an audio device and a new one becomes available. The solution to both these problems is to try calling *Reset* once when a new audio device becomes available on the system.

In the **Game.h** file, add the following method to public interface of the Game class:

```cpp
void OnNewAudioDevice() noexcept { m_retryAudio = true; }
```

In the **Main.cpp** file after the other includes at the top, add:

```cpp
#include <Dbt.h>
#include <ksmedia.h>
```

In **Main.cpp**, modify the **wWinMain** function as follows:

```cpp
...
    // Register class and create window
    HDEVNOTIFY hNewAudio = nullptr;
    {
        // Register class
...
        // Create window
...
        g_game->Initialize( hwnd );

        // Listen for new audio devices
        DEV_BROADCAST_DEVICEINTERFACE filter = {};
        filter.dbcc_size = sizeof( filter );
        filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        filter.dbcc_classguid = KSCATEGORY_AUDIO;

        hNewAudio = RegisterDeviceNotification( hwnd, &filter,
            DEVICE_NOTIFY_WINDOW_HANDLE );
    }

    // Main message loop
...
    g_game.reset();

    if (hNewAudio)
    {
        UnregisterDeviceNotification(hNewAudio);
        hNewAudio = nullptr;
    }

    CoUninitialize();

    return static_cast<int>(msg.wParam);
```

Lastly, in **Main.cpp**, add the following case to the switch statement in **WndProc**:

```cpp
case WM_DEVICECHANGE:
    if ( wParam == DBT_DEVICEARRIVAL )
    {
        auto pDev = reinterpret_cast<PDEV_BROADCAST_HDR>( lParam );
        if( pDev )
        {
            if ( pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
            {
                auto pInter = reinterpret_cast<
                    const PDEV_BROADCAST_DEVICEINTERFACE>( pDev );
                if ( pInter->dbcc_classguid == KSCATEGORY_AUDIO )
                {
                    if (game)
                        game->OnNewAudioDevice();
                }
            }
        }
    }
    return 0;
```

> **Universal Windows Platform apps:** Since there is no Win32 message loop for these applications, you'd make use of the WinRT class ``DeviceWatcher`` in the ``Windows.Device.Enumeration`` namespace. See [DeviceEnumerationAndPairing](https://github.com/Microsoft/Windows-universal-samples/tree/main/Samples/DeviceEnumerationAndPairing), and the [DirectX Tool Kit Simple Sample for UWP](https://github.com/walbourn/directxtk-samples/blob/main/SimpleSampleWindows10/Main.cpp)

**Next lesson:** [[Creating and playing sounds]]

# Further reading

DirectX Tool Kit docs [[Audio]], [[AudioEngine]]

[DirectX Tool Kit for Audio](https://walbourn.github.io/directx-tool-kit-for-audio/)  
[Learning XAudio2](https://walbourn.github.io/learning-xaudio2/)
