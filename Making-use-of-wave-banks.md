This lesson covers the basics of playing sounds with _DirectX Tool Kit for Audio_ using of XACT-style wave banks.

# Setup

First create a new project using the instructions from these four lessons to enable the audio engine for _DirectX Tool Kit for Audio_: [[The basic game loop]], [[Adding the DirectX Tool Kit]], [[Adding the DirectX Tool Kit for Audio]], and [[Adding audio to your project]]. We'll be using the result of that process for this lesson.

# Background

In the previous tutorial we loaded the individual sound files as distinct **SoundEffect** objects, but games commonly have hundreds or thousands of distinct sounds, music, and voice recordings. A more efficient way to manage this data is to make of a 'wave bank' which is a single file that contains a collection of ``.wav ``files that can be loaded all at once.

_DirectX Tool Kit for Audio_ supports "wave banks" which were created for the Cross-platform Audio Creation Tool ([XACT](https://en.wikipedia.org/wiki/Cross-platform_Audio_Creation_Tool)) from the legacy DirectX SDK. It makes no use of "sound banks" or "cues".

# Creating wave bank files

1. Download the [XWBTool.exe](https://github.com/microsoft/DirectXTK/releases/latest/download/XWBTool.exe) from the _DirectX Tool Kit_ site and extract the EXE into your project's folder.
1. Open a [command-prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq) and then change to your project's folder.

Run the following command-line

```
XWBTool -f -o sounds.xwb Explo1.wav Explo2.wav Explo3.wav Explo4.wav NightAmbienceSimple_02.wav
```

Then from the top menu in Visual Studio select **Project / Add Existing Item**.... Select [sounds.xwb](https://github.com/Microsoft/DirectXTK/wiki/media/sounds.xwb) and click "OK".

If you are using a Universal Windows Platform app or Xbox One project rather than a Windows desktop app, you need to manually edit the Visual Studio project properties on the ``sounds.xwb`` file and make sure "Content" is set to "Yes" so the data file will be included in your packaged build.

> The ``xactbld`` tool from the legacy DirectX SDK can also create the same ``xwb`` files.

# Playing a sound

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

> _Troubleshooting:_ If you get a runtime exception, then you may have the ``sounds.xwb`` file in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``std::make_unique<WaveBank>`` line and step into the code to find the exact problem.

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

# Streaming audio from wave banks

> **UNDER CONSTRUCTION**

**Next lesson:** [[Using positional audio]]

# Further reading

DirectX Tool Kit docs [[SoundEffectInstance]], [[SoundStreamInstance]], [[WaveBank]]
