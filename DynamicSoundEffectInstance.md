DynamicSoundEffectInstance is an instance of a sound which plays buffers submitted from the application. This can be used to implement procedural audio generation or to implement streaming. It can be played with 3D positional audio effects, volume and panning control, and pause/resume control. Looping is not supported.

Note that the DynamicSoundEffectInstance does not copy the wave data and instead refers to the data 'owned' by the application. Therefore, the buffers should be kept "live" until all sounds playing from it are finished.

_You can use direct low-level XAudio2 interfaces to implement your own streaming or procedural sound submission rather than make use of this class._

# Header
```cpp
#include <Audio.h>
```

# Initialization

This class supports integer PCM 8-bit or 16-bit data (defaults to 16-bit) with 1 - 8 interleaved channels.

```cpp
std::unique_ptr<DynamicSoundEffectInstance> effect;

// PCM 44100 Hz, 16-bit, 1 channel
effect = std::make_unique<DynamicSoundEffectInstance>( audEngine.get(),
    [](DynamicSoundEffectInstance*)
    {
        // 'Buffer needed' event handler
    },
    44100, 1 );
```

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr``.

It can optionally support 3D positional audio:

```cpp
effect = std::make_unique<DynamicSoundEffectInstance>( audEngine.get(),
    [](DynamicSoundEffectInstance*)
    {
        // 'Buffer needed' event handler
    },
    44100, 1, 16,
    SoundEffectInstance_Use3D );
```

Or use 3D positional audio with reverb effects (if [[AudioEngine]] was created using ``AudioEngine_EnvironmentalReverb`` | ``AudioEngine_ReverbUseFilters``):

```cpp
effect = std::make_unique<DynamicSoundEffectInstance>( audEngine.get(),
    [](DynamicSoundEffectInstance*)
    {
        // 'Buffer needed' event handler
    },
    44100, 1, 16,
    SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters );
```

# Instance flags

This is a combination of sound effect instance flags. It defaults to ``SoundEffectInstance_Default``.

See [[Instance flags|SoundEffectInstance#instance-flags]]

#  Playback control

* **Play**: Starts the playback of the sound. If paused, it resumes playback.

* **Stop** ( bool immediate = true ): Stops the playback of the voice. If immediate is true, the sound is immediately halted. Otherwise any 'tails' are played.

* **Resume**: Resumes playback if the sound is paused.

* **Pause**: Pauses the sound playback. Note that for a 'game' pause, you should use ``AudioEngine::Suspend`` / ``Resume`` instead of 'pausing' the sounds individually.

# Audio data submission
Unlike [[SoundEffectInstance]] which obtains the audio data from a [[SoundEffect]] or a [[WaveBank]], this class only plays data provided directly to the class via **SubmitBuffer**.

```cpp
std::vector<uint8_t> audioBytes;
audioBytes.resize( 44100 * 2 );

GenerateSineWave( reinterpret_cast<int16_t*>( &audioBytes.front() ),
    44100, 440 );

std::unique_ptr<DynamicSoundEffectInstance> effect(
    new DynamicSoundEffectInstance( audEngine.get(),
    [&audioBytes](DynamicSoundEffectInstance* effect)
    {
        int count = effect->PendingBufferCount();

        while( count < 3 )
        {
            effect->SubmitBuffer( &audioBytes.front(), audioBytes.size() );
            ++count;
        }
    }, 44100, 1, 16 ) );

effect->Play();

...

// The 'Buffer needed' event function is called during some later call
// to Update() as needed
```

The 'Buffer needed' callback function is invoked whenever there are <= 2 buffers pending or whenever a buffer completes playback. You should submit sufficient data to avoid starving the voice.

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

> This sample is somewhat contrived and is only for illustrative purposes. If the audio data is static, it's more
> efficient to use a [[SoundEffect]] and then create a [[SoundEffectInstance]] from it to play it as a looped
> sound. Dynamic cases will have different audio data in the buffer on each call to ``SubmitBuffer``. This
> requires some additional complexity to ensure that each buffer is freed or reused only after it has
> completed playing.

# Volume and panning

* **SetVolume** ( float volume ): Sets playback volume. Playback defaults to 1

* **SetPitch** ( float pitch ): Sets a pitch-shift factor. Ranges from -1 to ``+1``, playback defaults to 0 (which is no pitch-shifting). This will trigger a C++ exception if the object was created with ``SoundEffectInstance_NoSetPitch``.

* **SetPan** ( float pan ): Sets a pan settings: -1 is fully left, ``+1`` is fully right, and 0 is balanced.

> Panning is only supported for mono and stereo sources, and will overwrite any ``Apply3D`` settings.

# Positional 3D audio

See [[Positional 3D audio|SoundEffectInstance#positional-3d-audio]].

# Voice management

A SoundStreamInstance will allocate a XAudio2 source voice when played, and will keep that source voice for the life of the object. You can force all SoundStreamInstance that currently have source voices but are not currently playing to release them by calling ``AudioEngine::TrimVoicePool``.

By default the number of XAudio2 source voices that can be allocated is 'unlimited'. You can set a specific limit using ``AudioEngine::SetMaxVoicePool`` which will be enforced by generating a C++ exception if there are too many allocated source voices when ``Play`` is called.

See [[AudioEngine]] for more details.

# Properties

* **GetState**: Returns ``STOPPED``, ``PLAYING``, or ``PAUSED``.

* **GetPendingBufferCount**: Returns the number of pending buffers submitted to the instance that are either playing or waiting to play.

* **GetFormat**: Returns a ``WAVEFORMATEX`` that describes the audio format consumed by the sound instance. This is always integer PCM data.

# Helper methods

* **GetSampleDuration** ( bytes ): Returns duration in samples of a buffer of a given size

* **GetSampleDurationMS** ( bytes ): Returns duration in milliseconds of a buffer of a given size

* **GetSampleSizeInBytes** ( duration ): Returns size of a buffer for a duration given in milliseconds

# Remarks

Be sure to call ``AudioEngine::Update`` frequently as this class relies on this to manage requesting new buffers and playback submission.
