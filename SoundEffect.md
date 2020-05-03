SoundEffect is a container class for wave sample data. This data can then be played back as a one-shot sound or via a [[SoundEffectInstance]].

**Related tutorial:** [[Creating and playing sounds]]

# Header
```cpp
#include <Audio.h>
```

# Initialization

The SoundEffect class can be created with two main forms of constructor. The first loads from a ``.wav`` file on disk.

```cpp
std::unique_ptr<SoundEffect> effect;
effect = std::make_unique<SoundEffect>( audEngine.get(), L"sound.wav" );
```

For exception safety, it is recommended you make use of the C++ C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

The second takes ownership of a memory buffer containing the wave data. The _wfx_ and _startAudio_ pointers are assumed to point into the same memory buffer owned by _wavData_ since they must remain valid for the life of the SoundEffect object.

```cpp
size_t audioSize = 44100 * 2;
std::unique_ptr<uint8_t[]> wavData( new uint8_t[
    audioSize + sizeof(WAVEFORMATEX) ] );

auto startAudio = wavData.get() + sizeof(WAVEFORMATEX);

GenerateSineWave( reinterpret_cast<int16_t*>( startAudio ), 44100, 440 );

auto wfx = reinterpret_cast<WAVEFORMATEX*>( wavData.get() );
wfx->wFormatTag = WAVE_FORMAT_PCM;
wfx->nChannels = 1;
wfx->nSamplesPerSec = 44100;
wfx->nAvgBytesPerSec = 2 * 44100;
wfx->nBlockAlign = 2;
wfx->wBitsPerSample = 16;
wfx->cbSize = 0;

effect = std::make_unique<SoundEffect>( audEngine.get(),
    wavData, wfx, startAudio, audioSize );
```

This example uses a simple helper routine which fills a buffer with 1 second of a pure sine wave at a given frequency:

```cpp
void GenerateSineWave( _Out_writes_(sampleRate) int16_t* data,
    int sampleRate, int frequency )
{
    const double timeStep = 1.0 / double(sampleRate);
    const double freq = double(frequency);

    int16_t* ptr = data;
    double time = 0.0;
    for( int j = 0; j < sampleRate; ++j, ++ptr )
    {
        double angle = ( 2.0 * XM_PI * freq ) * time;
        double factor = 0.5 * ( sin(angle) + 1.0 );
        *ptr = int16_t( 32768 * factor );
        time += timeStep;
    }
}
```

# Play one-shot sounds

To play the sound effect as a 'fire and forget' sound:

```cpp
soundEffect->Play();
```

To play the sound effect as a 'fire and forget' sound with volume, a pitch-shift, and/or pan setting:

```cpp
soundEffect->Play( volume, pitch, pan );
```

* _volume_ default is 1
* _pitch_ ranges from -1 to +1, playback defaults to 0 (which is no pitch-shifting)
* _pan_ -1 is fully left, +1 is fully right, and 0 is balanced.

# Playing the sound

To play the sound with full control, looping, and dynamic pitch-shifting/volume control/pan settings:

```cpp
auto effect = soundEffect->CreateInstance();
```

To play the sound with positional 3D audio:

```cpp
auto effect = soundEffect->CreateInstance(
    SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);
```

See [[SoundEffectInstance]].

# Properties

* **IsInUse**: Returns true if the SoundEffect is currently playing as a one-shot or there are SoundEffectInstances referencing it.

* **GetSampleSizeInBytes**: Returns the size of the wave data in bytes.

* **GetSampleDuration**: Returns the wave data duration in samples. This does not include any loops.

* **GetSampleDurationMS**: Returns the wave data duration in milliseconds. This does not include any loops.

* **GetFormat**: Returns ``WAVEFORMATEX`` structure that describes the wave data format. See [[Wave Formats]].

> At least 64 bytes is recommended as this is large enough to contain ``WAVEFORMAT``, ``PCMWAVEFORMAT``, ``WAVEFORMATEX``, ``ADPCMWAVEFORMAT`` with coefficients,  ``WAVEFORMATEXTENSIBLE``, or a ``XMA2WAVEFORMATEX``.

```cpp
char formatBuff[ 64 ] = {};
auto wfx = reinterpret_cast<WAVEFORMATEX*>( formatBuff );

effect->GetFormat( wfx, 64 );
```

# Low-level access

**FillSubmitBuffer** is used internally, but can also be used when implementing your own XAudio2 source voices using the low-level interface access. Note that _LoopCount_ is set to 0 by this function and should be set to a non-zero value by the caller. If looping is not desired, be sure to set _LoopBegin_ and _LoopLength_ to zero as well.

Care needs to be taken to ensure that any referenced SoundEffect is not deleted while a source voice is actively playing back content from it or has pending buffers referencing it.

# Content support

XAudio 2.9 on Windows 10 supports PCM, ADPCM, and xWMA.

XAudio 2.8 on Windows 8.x and Windows phone support PCM and ADPCM formats.

XAudio 2.7 on Windows 7 or later via the legacy DirectX End-User Runtime Redistribution (aka DirectSetup) supports PCM, ADPCM, and xWMA.

XAudio on Xbox One supports PCM, ADPCM, and xWMA. Xbox One exclusive app developers can also make use of XMA2.

To compress to ADPCM (a variant of MS-ADPCM) ``.wav`` files, use ``adpcmencode.exe`` from the Windows SDK, Xbox One XDK, or legacy DirectX SDK.

To compress to xWMA ``.wav`` files, use ``xwmaencode.exe`` from the  Xbox One XDK or legacy DirectX SDK.

To compress to XMA2 ``.wav`` files, use ``xma2encode.exe`` from the Xbox One XDK.

# WAV File Format

To aid in debugging, here is a [simple console program](https://github.com/Microsoft/DirectXTK/wiki/wavdump.cpp) for dumping out the content of a ``.wav`` in a human-readable form.

[Resource Interchange File Format (RIFF)](https://docs.microsoft.com/en-us/windows/desktop/xaudio2/resource-interchange-file-format--riff-)
