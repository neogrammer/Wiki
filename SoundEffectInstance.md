SoundEffectInstance is an instance of a sound from a [[SoundEffect]] or a in-memory [[WaveBank]]. It can be played with 3D positional audio effects, volume and panning control, looping, and pause/resume control.

Note that the SoundEffectInstance does not copy the wave data and instead refers to the data 'owned' by the SoundEffect / WaveBank. Therefore, the parent object must be kept "live" until all sounds playing from it are finished.

**Related tutorials:** [[Creating and playing sounds]], [[Making use of wave banks]], [[Using positional audio]]

# Header
```cpp
#include <Audio.h>
```

# Initialization

It can be created for an individual sound loaded as a SoundEffect (which is returned as a ``std::unique_ptr<SoundEffectInstance>``)

```cpp
auto effect = soundEffect->CreateInstance();
```

Or created for an entry in a in-memory WaveBank (which is returned as a ``std::unique_ptr<SoundEffectInstance>``):

```cpp
auto effect = wb->CreateInstance( 2 );
if ( !effect )
    // Index not found in wave bank
```

It can optionally support 3D positional audio:

```cpp
auto effect = soundEffect->CreateInstance( SoundEffectInstance_Use3D );

auto effect = wb->CreateInstance( 2, SoundEffectInstance_Use3D );
if ( !effect )
    // Index not found in wave bank
```

Or use 3D positional audio with reverb effects (if [[AudioEngine]] was created using ``AudioEngine_EnvironmentalReverb`` | ``AudioEngine_ReverbUseFilters``):

```cpp
auto effect = soundEffect->CreateInstance(
    SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);

auto effect = wb->CreateInstance( 2,
    SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);
if ( !effect )
    // Index not found in wave bank
```

# Instance flags

This is a combination of sound effect instance flags. It defaults to ``SoundEffectInstance_Default``.

* ``SoundEffectInstance_Use3D`` - Required to use **Apply3D**
* ``SoundEffectInstance_ReverbUseFilters`` - Enables additional effects if the audio engine was created with ``AudioEngine_EnvironmentalReverb`` and optionally ``AudioEngine_ReverbUseFilters``.
* ``SoundEffectInstance_NoSetPitch`` - If set, this instance cannot use **SetPitch**. This is a useful optimization for XAudio2 if you are not making use of pitch-shifting.
* There is also a ``SoundEffectInstance_UseRedirectLFE`` which is used internally by the library.

> ``SOUND_EFFECT_INSTANCE_FLAGS`` is used as a typed flag enum. Only ``operator|`` is overloaded to combine them,
> so operations like ``|=`` are not available without additional ``static_cast<>`` statements.

#  Playback control

* **Play** ( bool loop = false ): Starts the playback of the sound. If loops is set to true, it loops continuously either the entire buffer or using the authored loop points (if any). If paused, it resumes playback.

> Note if a source voice limit is in effect (see [[AudioEngine]]), then a C++ exception can be generated from
> this method if there are too many source voices already allocated.

* **Stop** ( bool immediate = true ): Stops the playback of the voice. If immediate is true, the sound is immediately halted. Otherwise the current loop is exited (if looping) and any 'tails' are played. The sound still not completely stop playing until a future point so the state will remain ``PLAYING`` for a while.

* **Resume**: Resumes playback if the sound is paused.

* **Pause**: Pauses the sound playback. Note that for a 'game' pause, you should use ``AudioEngine::Suspend`` / ``Resume`` instead of 'pausing' the sounds individually.

# Volume and panning

* **SetVolume** ( float volume ): Sets playback volume. Playback defaults to 1

* **SetPitch** ( float pitch ): Sets a pitch-shift factor. Ranges from -1 to ``+1``, playback defaults to 0 (which is no pitch-shifting). This will trigger a C++ exception if the object was created with ``SoundEffectInstance_NoSetPitch``.

* **SetPan** ( float pan ): Sets a pan settings: -1 is fully left, ``+1`` is fully right, and 0 is balanced.

> Panning is only supported for mono and stereo sources, and will overwrite any ``Apply3D`` settings.

# Positional 3D audio

DirectXTK for Audio uses [X3DAudio](https://docs.microsoft.com/en-us/windows/desktop/xaudio2/x3daudio) for positional audio computations. To apply a 3D effect to a sound instance, you call **Apply3D** with the listener location (i.e. where the player/camera is located) and the emitter (i.e. where the sound source is located in 3D dimensions):

```cpp
AudioListener listener;
listener.SetPosition( ... );

AudioEmitter emitter;
emitter.SetPosition( ... );

effect->Apply3D( listener, emitter );
```

Note if the instance was created without ``SoundEffectInstance_Use3D``, then calls to **Apply3D** will result in a C++ exception being thrown. **Apply3D** will overwrite any ``SetPan`` settings.

The **GetChannelCount** method is provided to simplify setting up ``AudioEmitter`` instances for multi-channel sources:

```cpp
emitter.EnableDefaultMultiChannel( m_effect->GetChannelCount() );
```

See [[AudioListener]], [[AudioEmitter]]


## Coordinate systems

The emitter and listener (based on the XNA Game Studio conventions) use right-handed coordinates. They can be used with left-handed coordinates by setting the _rhcoords_ parameter on the ``Apply3D`` method to 'false' (the parameter defaults to 'true').

```cpp
AudioListener listener;
listener.SetPosition( ... );
listener.SetOrientation( ... );

AudioEmitter emitter;
emitter.SetPosition( ... );
emitter.SetOrientation( ... );

effect->Apply3D( listener, emitter, false );
```

# Voice management

A SoundEffectInstance will allocate a XAudio2 source voice when played, and will keep that source voice for the life of the object. You can force all SoundEffectInstances that currently have source voices but are not currently playing to release them by calling ``AudioEngine::TrimVoicePool``.

If you want to have the same sound playing multiple times and be 3D positioned, you need to create multiple instances of SoundEffectInstance for the same sound and then clean then up or re-use them manually. For non-3D positioned sounds, this is easily done by using ``SoundEffect::Play()`` or ``WaveBank::Play()`` instead.

By default the number of XAudio2 source voices that can be allocated is 'unlimited'. You can set a specific limit using ``AudioEngine::SetMaxVoicePool`` which will be enforced by generating a C++ exception if there are too many allocated source voices when ``Play`` is called.

See [[AudioEngine]] for more details.

# Properties

* **IsLooped**: Returns true if the sound was played with looping enable.

* **GetState**: Returns ``STOPPED``, ``PLAYING``, or ``PAUSED``.
