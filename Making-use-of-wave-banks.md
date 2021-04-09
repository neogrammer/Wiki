This lesson covers the basics of playing sounds with _DirectX Tool Kit for Audio_ using of XACT-style wave banks.

# Setup

First create a new project in the [[The basic game loop]] or [[Using DeviceResources]], then [[Adding the DirectX Tool Kit]], then [[Adding the DirectX Tool Kit for Audio]], and finally [[Adding audio to your project]] which we will use for this lesson.

# Background

In the previous tutorial we loaded the individual sound files as distinct **SoundEffect** objects, but games commonly have hundreds or thousands of distinct sounds, music, and voice recordings. A more efficient way to manage this data is to make of a 'wave bank' which is a single file that contains a collection of ``.wav ``files that can be loaded all at once.

_DirectX Tool Kit for Audio_ supports "wave banks" which were created for the Cross-platform Audio Creation Tool ([XACT](https://en.wikipedia.org/wiki/Cross-platform_Audio_Creation_Tool)) from the legacy DirectX SDK. It makes no use of "sound banks" or "cues".

# Creating wave bank files

1. Download the [XWBTool.exe](https://github.com/microsoft/DirectXTK/releases/latest/download/XWBTool.exe) from the _DirectX Tool Kit_ site and extract the EXE into your project's folder.
1. Download these media files to your project's folder [Explo1.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo1.wav), [Explo2.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo2.wav), [Explo3.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo3.wav), [Explo4.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Explo4.wav), [NightAmbienceSimple_02.wav](https://github.com/Microsoft/DirectXTK/wiki/media/NightAmbienceSimple_02.wav).
1. Open a [command-prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq) and then change to your project's folder.

> These ``.wav`` files are from the _XNA Game Studio_ [SoundLab](https://github.com/SimonDarksideJ/XNAGameStudio/wiki/SoundLab)  which provides a small collection of sounds under the [MS-PL](http://opensource.org/licenses/MS-PL) license.

Run the following command-line:

```
XWBTool -f -o sounds.xwb Explo1.wav Explo2.wav Explo3.wav Explo4.wav NightAmbienceSimple_02.wav
```

Then from the top menu in Visual Studio select **Project / Add Existing Item**.... Select [sounds.xwb](https://github.com/Microsoft/DirectXTK/wiki/media/sounds.xwb) and click "OK".

If you are using a Universal Windows Platform app or Xbox One project rather than a Windows desktop app, you need to manually edit the Visual Studio project properties on the ``sounds.xwb`` file and make sure "Content" is set to "Yes" so the data file will be included in your packaged build.

> The ``xactbld`` tool from the legacy DirectX SDK can also create the same ``xwb`` files.

# Playing a sound

In the **pch.h** file, add after the other includes:

```cpp
#include <random>
```

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::WaveBank> m_sounds;
std::unique_ptr<std::mt19937> m_random;
std::unique_ptr<DirectX::SoundEffectInstance> m_nightLoop;
float explodeDelay;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
std::random_device rd;
m_random = std::make_unique<std::mt19937>(rd());

explodeDelay = 2.f;

m_sounds = std::make_unique<WaveBank>( m_audEngine.get(), L"sounds.xwb" );

m_nightLoop = m_sounds->CreateInstance( "NightAmbienceSimple_02" );
if ( m_nightLoop )
    m_nightLoop->Play(true);
...
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
explodeDelay -= elapsedTime;
if (explodeDelay < 0.f)
{
    std::uniform_int_distribution<unsigned int> dist2(0, 3);
    m_sounds->Play( dist2(*m_random) );

    std::uniform_real_distribution<float> dist(1.f, 10.f);
    explodeDelay = dist(*m_random);
}
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

In **Game.cpp**, add to the TODO of **OnResuming**:

```cpp
explodeDelay = 2.f;
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

Build and run. This is the same as the last tutorial, but this time the sounds are being played from the wave bank. The explosion also now selects from 4 different exploding sounds at random to provide some variation.

> ``WaveBank`` must remain in memory as the audio data is played directly from that object. XAudio does _not_ copy the source audio data into it's own buffers, but fetches from the application's memory periodically from a worker thread.

> _Troubleshooting:_ If you get a runtime exception, then you may have the ``sounds.xwb`` file in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``std::make_unique<WaveBank>`` line and step into the code to find the exact problem.

## Technical notes
We need to ensure that the looping ``SoundEffectInstance`` is destroyed before the object it was created from is released. We therefore added an explicit reset to the Game's destructor to avoid being dependent on the order of destruction in the Game class. We also do this _after_ the audio engine has been explicitly suspended since XAudio2 is multi-threaded and references data directly from the application's objects.

To make use of wavebanks a bit more robust, calls to ``CreateInstance`` for 'missing sounds' can return nullptr rather than throwing an exception. This is why we guard the use of ``m_nightLoop`` elsewhere. This lets your audio developer mess around with the content of the wave banks without necessarily causing the game to crash.

# Generating wave bank entry headers

Wavebank entries can be referenced by a 0-based index or by an optional friendly name if present in the wave bank--this is why we used ``-f`` in our command-line to **xwbtool** above to have it include friendly names. You can also have **xwbtool** generate a C header with an enumeration for the indices by using ``-h``.

```
XWBTool -f -o sounds.xwb -y -h sounds.h Explo1.wav Explo2.wav Explo3.wav Explo4.wav NightAmbienceSimple_02.wav
```

which would generate a ``sounds.h`` file you could use instead of having 'magic' numbers in your code:

```cpp
#pragma once

enum XACT_WAVEBANK_SOUNDS : unsigned int
{
    XACT_WAVEBANK_SOUNDS_EXPLO1 = 0,
    XACT_WAVEBANK_SOUNDS_EXPLO2 = 1,
    XACT_WAVEBANK_SOUNDS_EXPLO3 = 2,
    XACT_WAVEBANK_SOUNDS_EXPLO4 = 3,
    XACT_WAVEBANK_SOUNDS_NIGHTAMBIENCESIMPLE_02 = 4,
};

#define XACT_WAVEBANK_SOUNDS_ENTRY_COUNT 5
```

# Streaming audio from wave banks

In the previous section we generated 'in-memory' wave banks where the data is all loaded into RAM. XACT wave banks also support 'streaming' wave banks which are arranged on disk to support non-buffered asynchronous I/O.

Download these media files to your project's folder [Electro_1.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Electro_1.wav). Run the following command-line:

```
xwbtool -o music.xwb -s Electro_1.wav
```

Then from the top menu in Visual Studio select **Project / Add Existing Item**.... Select [music.xwb](https://github.com/Microsoft/DirectXTK/wiki/media/music.xwb) and click "OK".

If you are using a Universal Windows Platform app or Xbox One project rather than a Windows desktop app, you need to manually edit the Visual Studio project properties on the ``music.xwb`` file and make sure "Content" is set to "Yes" so the data file will be included in your packaged build.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::WaveBank> m_music;
std::unique_ptr<DirectX::SoundStreamInstance> m_stream;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
m_music = std::make_unique<WaveBank>( m_audEngine.get(), L"music.xwb" );

m_stream = m_music->CreateStreamInstance(0u);
if (m_stream)
{
    m_stream->SetVolume(0.5f);
    m_stream->Play(true);
}
```

In **Game.cpp**, modify the destructor:

```cpp
Game::~Game()
{
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }

    m_stream.reset();
    m_nightLoop.reset();
}
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
      if (m_nightLoop)
          m_nightLoop->Play(true);
      if (m_stream)
          m_stream->Play(true);
    }
}
...
```

Build and run to hear a looping music track added to the other sounds.

## Technical notes

You can create both in-memory and streaming wave banks using ADPCM, xWMA, or XMA2. For example, download this xWMA compressed version of [Electro_1_xwma.wav](https://github.com/Microsoft/DirectXTK/wiki/media/Electro_1_xwma.wav) and create the music wave bank:

```
xwbtool -o music.xwb -y -s Electro_1_xwma.wav
```

> _Troubleshooting:_ This will fail to run if you are using XAudio 2.8 which only supports ADPCM, but not xWMA. It will run with XAudio 2.9, XAudio 2.7, or the XAudio2Redist.

**Next lesson:** [[Using positional audio]]

# Further reading

DirectX Tool Kit docs [[SoundEffectInstance]], [[SoundStreamInstance]], [[WaveBank]]
