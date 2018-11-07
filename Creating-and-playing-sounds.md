This lesson covers the basics of playing sounds with _DirectX Tool Kit for Audio_.

# Setup
First create a new project using the instructions from these four lessons to enable the audio engine for _DirectX Tool Kit for Audio_: [[The basic game loop]], [[Adding the DirectX Tool Kit]], [[Adding the DirectX Tool Kit for Audio]], and [[Adding audio to your project]]. We'll be using the result of that process for this lesson.

# Sounds
Save the following files to your new project's folder: [Explo1.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo1.wav), [Explo2.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo2.wav), [Explo3.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo3.wav), [Explo4.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo4.wav), [NightAmbienceSimple_02.wav](https://github.com/Microsoft/DirectXTK/wiki/media/NightAmbienceSimple_02.wav). Use the top menu and select **Project / Add Existing Item**.... Select each ``.wav`` file in turn and hit "OK".

> These ``.wav`` files are from the _XNA Game Studio_ [SoundLab](http://xbox.create.msdn.com/en-US/education/catalog/utility/soundlab) sample which provides a small collection of sounds under the [MS-PL](http://opensource.org/licenses/MS-PL) license.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::SoundEffect> m_explode;
std::unique_ptr<DirectX::SoundEffect> m_ambient;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
m_explode = std::make_unique<SoundEffect>( m_audEngine.get(), L"explo1.wav" );
m_ambient = std::make_unique<SoundEffect>( m_audEngine.get(),
    L"NightAmbienceSimple_02.wav" );
```

Build and run. No sounds will be heard, but the audio files are loaded.

> _Troubleshooting:_ If you get a runtime exception, then you may have the ``.wav`` files in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``new SoundEffect`` and step into the code to find the exact problem.

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

# Using wave banks

Above we loaded the individual sound files as distinct **SoundEffect** objects, but games commonly have hundreds or thousands of distinct sounds, music, and voice recordings. A more efficient way to manage this data is to make of a 'wave bank' which is a single file that contains a collection of ``.wav ``files that can be loaded all at once.

1. Download the [XWBTool.exe](https://github.com/Microsoft/DirectXTK/releases/download/sep2016b/XWBTool.exe) from the _DirectX Tool Kit_ site and extract the EXE into your project's folder.
1. Open a [command-prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq) and then change to your project's folder.

Run the following command-line

    XWBTool -f -o sounds.xwb Explo1.wav Explo2.wav Explo3.wav Explo4.wav NightAmbienceSimple_02.wav

Then from the top menu in Visual Studio select **Project / Add Existing Item**.... Select [sounds.xwb](https://github.com/Microsoft/DirectXTK/wiki/media/sounds.xwb) and click "OK".

> If you are using a Universal Windows Platform app or Xbox One project rather than a Windows desktop app, you need to manually edit the Visual Studio project properties on the ``sounds.xwb`` file and make sure "Content" is set to "Yes" so the data file will be included in your packaged build.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::WaveBank> m_sounds;
```

In **Game.cpp**, modify **Initialize**:

```cpp
...
m_sounds = std::make_unique<WaveBank>( m_audEngine.get(), L"sounds.xwb" );

m_nightLoop = m_sounds->CreateInstance( "NightAmbienceSimple_02" );
if ( m_nightLoop )
    m_nightLoop->Play(true);
...
```

In **Game.cpp**, modify **Update** as follows:

```cpp
...
explodeDelay -= elapsedTime;
if (explodeDelay < 0.f)
{
    std::uniform_int_distribution<unsigned int> dist2(0, 3);
    m_sounds->Play( dist2(*m_random) );

    std::uniform_real_distribution<float> dist(1.f, 10.f);
    explodeDelay = dist(*m_random);
}
...
```

Build and run. This time the sounds are being played from the wave bank, and the explosion now selects from 4 different exploding sounds at random to provide some variation.

> Once again, ``WaveBank`` must remain in memory as the audio data is played directly from that object.

> _Troubleshooting:_ If you get a runtime exception, then you may have the ``sounds.xwb`` file in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``new WaveBank`` and step into the code to find the exact problem.

## Technical notes
To make use of wavebanks a bit more robust, calls to ``CreateInstance`` for 'missing sounds' can return nullptr rather than throwing an exception. This is why we guard the use of ``m_nightLoop`` elsewhere. This lets your audio developer mess around with the content of the wave banks without necessarily causing the game to crash.

Wavebank entries can be referenced by a 0-based index or by an optional friendly name if present in the wave bank--this is why we used ``-f`` in our command-line to **xwbtool** above to have it include friendly names. You can also have **xwbtool** generate a C header with an enumeration for the indices by using ``-h``.

    XWBTool -f -o sounds.xwb -h sounds.h Explo1.wav Explo2.wav Explo3.wav Explo4.wav NightAmbienceSimple_02.wav

which would generate a ``sounds.h`` file you could use instead of having 'magic' numbers in your code:

```cpp
#pragma once

enum XACT_WAVEBANK_SOUNDS
{
    XACT_WAVEBANK_SOUNDS_EXPLO1 = 0,
    XACT_WAVEBANK_SOUNDS_EXPLO2 = 1,
    XACT_WAVEBANK_SOUNDS_EXPLO3 = 2,
    XACT_WAVEBANK_SOUNDS_EXPLO4 = 3,
    XACT_WAVEBANK_SOUNDS_NIGHTAMBIENCESIMPLE_02 = 4,
};

#define XACT_WAVEBANK_SOUNDS_ENTRY_COUNT 5
```

**Next lesson:** [[Using positional audio]]

# Further reading

DirectX Tool Kit docs [[SoundEffect]], [[SoundEffectInstance]], [[WaveBank]]
