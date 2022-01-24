This lesson covers the basics of playing sounds with _DirectX Tool Kit for Audio_.

# Setup
First create a new project using the instructions from the earlier lessons: [[Using DeviceResources]], then [[Adding the DirectX Tool Kit]], [[Adding the DirectX Tool Kit for Audio]], and finally [[Adding audio to your project]] which we will use for this lesson.

> If using *DirectX Tool Kit for DX12*, use [Using DeviceResources](https://github.com/microsoft/DirectXTK12/wiki/Using-DeviceResources), then [Adding the DirectX Tool Kit](https://github.com/microsoft/DirectXTK12/wiki/Adding-the-DirectX-Tool-Kit), add ``#include <Audio.h>`` to **pch.h**, and finally [[Adding audio to your project]]
# Sounds

Save the following files to your new project's folder: [Explo1.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo1.wav), [Explo2.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo2.wav), [Explo3.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo3.wav), [Explo4.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo4.wav), [NightAmbienceSimple_02.wav](https://github.com/Microsoft/DirectXTK/wiki/media/NightAmbienceSimple_02.wav). Use the top menu and select **Project / Add Existing Item**.... Select each ``.wav`` file in turn and hit "OK".

> These ``.wav`` files are from the _XNA Game Studio_ [SoundLab](https://github.com/SimonDarksideJ/XNAGameStudio/wiki/SoundLab)  which provides a small collection of sounds under the [MS-PL](http://opensource.org/licenses/MS-PL) license.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::SoundEffect> m_explode;
std::unique_ptr<DirectX::SoundEffect> m_ambient;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
m_explode = std::make_unique<SoundEffect>( m_audEngine.get(),
    L"explo1.wav" );
m_ambient = std::make_unique<SoundEffect>( m_audEngine.get(),
    L"NightAmbienceSimple_02.wav" );
```

Build and run. No sounds will be heard, but the audio files are loaded.

<details><summary><i>Click here for troubleshooting advice</i></summary>
<p>If you get a runtime exception, then you may have the <code>.wav</code> files in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on <code>std::make_unique&lt;SoundEffect&gt;</code> and step into the code to find the exact problem.</p></details>

# Playing a sound

In the **pch.h** file, add after the other includes:

```cpp
#include <random>
```

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<std::mt19937> m_random;
float explodeDelay;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
std::random_device rd;
m_random = std::make_unique<std::mt19937>(rd());

explodeDelay = 2.f;
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
explodeDelay -= elapsedTime;
if (explodeDelay < 0.f)
{
    m_explode->Play();

    std::uniform_real_distribution<float> dist(1.f, 10.f);
    explodeDelay = dist(*m_random);
}
```

In **Game.cpp**, add to the TODO of **OnResuming**:

```cpp
explodeDelay = 2.f;
```

Build and run. You will hear an explosion sound effect every 1 to 10 seconds. This is a 'one-shot' sound that you trigger and the audio engine will handle all the voice management automatically.  You can trigger the same sound many times at once and each one-shot will play on it's own 'voice'.

> The ``SoundEffect`` itself must remain in memory as all playback uses the audio data directly from that object. XAudio does _not_ copy the source audio data into it's own buffers, but fetches from the application's memory periodically from a worker thread.

# Looping a sound

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::SoundEffectInstance> m_nightLoop;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
m_nightLoop = m_ambient->CreateInstance();
m_nightLoop->Play(true);
```

In **Game.cpp**, modify the handling of ``m_retryAudio`` in **Update** as follows:

```cpp
...
if (m_retryAudio)
{
    m_retryAudio = false;

    if (m_audEngine->Reset())
    {
        // TODO: restart any looped sounds here
        if ( m_nightLoop )
            m_nightLoop->Play(true);
    }
}
...
```

In **Game.cpp**, modify the destructor:

```cpp
Game::~Game()
{
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }

    m_nightLoop.reset();
}
```

Build and run to hear an ambient night sound looping in the background. In this case, the ``SoundEffectInstance`` has a single assigned voice, so stopping and starting it affects the one playing sound.

> Again, the ``SoundEffect`` must remain in memory as the audio data is used directly from that object.

## Technical notes
We need to ensure that the looping ``SoundEffectInstance`` is destroyed before the object it was created from is released. We therefore added an explicit reset to the Game's destructor to avoid being dependent on the order of destruction in the Game class. We also do this _after_ the audio engine has been explicitly suspended since XAudio2 is multi-threaded and references data directly from the application's objects.

# Adjusting pitch, volume, and panning

When you use **Play** on a one-shot sound, you can specify pitch, volume, and panning settings to use for the playback. If you have a **SoundEffectInstance** you can modify the sound pitch, volume, and panning settings while it is playing.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
float nightVolume;
float nightSlide;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
nightVolume = 1.f;
nightSlide = -0.1f;
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
nightVolume += elapsedTime * nightSlide;
if (nightVolume < 0.f)
{
    nightVolume = 0.f;
    nightSlide = -nightSlide;
}
else if (nightVolume > 1.f)
{
    nightVolume = 1.f;
    nightSlide = -nightSlide;
}
m_nightLoop->SetVolume( nightVolume );
```

Build and run. The looping night ambient sound will slowly decrease and increase volume over time.

**Next lesson:** [[Making use of wave banks]]

# Further reading

DirectX Tool Kit docs [[SoundEffect]], [[SoundEffectInstance]]  

[XAudio2BasicSound](https://github.com/walbourn/directx-sdk-samples/tree/main/XAudio2/XAudio2BasicSound), [SimplePlaySound (UWP)](https://github.com/microsoft/Xbox-ATG-Samples/tree/master/UWPSamples/Audio/SimplePlaySoundUWP), [SimplePlaySound (GDK)](https://github.com/microsoft/Xbox-GDK-Samples/tree/main/Samples/Audio/SimplePlaySound) samples
