The _DirectXTK for Audio_ components implement a low-level audio API similar to XNA Game Studio's [Microsoft.Xna.Framework.Audio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.audio.aspx) design. This consists of the following classes all declared in the ``Audio.h`` header (in the _Inc_ folder of the distribution):

* [[AudioEngine]] - This class represents an XAudio2 audio graph, device, and mastering voice. 
* [[SoundEffect]] - A container class for sound resources which can be loaded from ``.wav`` files. 
* [[SoundEffectInstance]] - Provides a single playing, paused, or stopped instance of a sound 
* [[DynamicSoundEffectInstance]] - SoundEffectInstance where the application provides the audio data on demand 
* [[WaveBank]] - A container class for sound resources packaged into an XACT-style ``.xwb`` wave bank. 
* [[AudioListener]],  [[AudioEmitter]] - Utility classes used with ``SoundEffectInstance::Apply3D``.

_Note: DirectXTK for Audio uses XAudio 2. It does not make use of the legacy XACT Engine, XACT Cue, or XACT SoundBank._

**Related tutorials:** [[Adding the DirectX Tool Kit for Audio]], [[Adding audio to your project]], [[Creating and playing sounds]], [[Using positional audio]]

# Header

    #include <Audio.h>

# Initialization
The first step in using DirectXTK for Audio is to create the AudioEngine, which creates an XAudio2 interface, an XAudio2 mastering voice, and other global resources.

    // This is only needed in Windows desktop apps
    CoInitializeEx( nullptr, COINIT_MULTITHREADED );

    ...

    std::unique_ptr<AudioEngine> audEngine;

    ...
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
    #ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
    #endif
    audEngine = std::make_unique<AudioEngine>( eflags );

# Per-frame processing
The application should call ``Update`` every frame to allow for per-frame engine updates, such as one-shot voice management. This could also be done in a worker thread rather than on the main rendering thread.

    if ( !audEngine->Update() )
    {
        // No audio device is active
        if ( audEngine->IsCriticalError() )
        {
            ...
        }    
    }

``Update`` returns false if no audio is actually playing (either due to there being no audio device on the system at the time AudioEngine was created, or because XAudio2 encountered a Critical Error--typically due to speakers being unplugged). Calls to various DirectXTK for Audio methods can still be made in this state but no actual audio processing will take place. See [[AudioEngine]] for more information.

# Loading and a playing a looping sound
Creating SoundEffectInstances allows full control over the playback, and are provided with a  dedicated XAudio2 source voice. This allows control of playback, looping, volume control, panning,  and pitch-shifting.

    std::unique_ptr<SoundEffect> soundEffect( new SoundEffect( audEngine.get(),
        L"Sound.wav" ) );
    auto effect = soundEffect->CreateInstance();

    ...

    effect->Play( true );

# Playing one-shots
A common way to play sounds is to trigger them in a 'fire-and-forget' mode. This is done by calling ``SoundEffect::Play`` rather than creating a SoundEffectInstance. These use XAudio2 source voices managed by AudioEngine, are cleaned up automatically when they finish playing, and can overlap in time. One-shot sounds cannot be looped or have positional 3D effects.

    std::unique_ptr<SoundEffect> soundEffect( new SoundEffect( audEngine.get(),
        L"Explosion.wav" ) );
    soundEffect->Play();

    ...

    soundEffect->Play();

# Applying 3D audio effects to a sound
DirectXTK for Audio supports positional 3D audio with optional environmental reverb effects using X3DAudio.

    AUDIO_ENGINE_FLAGS eflags =  AudioEngine_EnvironmentalReverb
                | AudioEngine_ReverbUseFilters;
    #ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
    #endif
    std::unique_ptr<AudioEngine> audEngine( new AudioEngine( eflags ) );
    audEngine->SetReverb( Reverb_ConcertHall );

    ...

    std::unique_ptr<SoundEffect> soundEffect( new SoundEffect( audEngine.get(),
        L"Sound.wav" ) );
    auto effect = soundEffect->CreateInstance( SoundEffectInstance_Use3D
        | SoundEffectInstance_ReverbUseFilters );

    ...

    effect->Play(true);

    ...

    AudioListener listener;
    listener.SetPosition( ... );

    AudioEmitter emitter;
    emitter.SetPosition( ... );

    effect->Apply3D( listener, emitter );

**Note:** A C++ exception is thrown if you call Apply3D for a SoundEffectInstance that was not created with SoundEffectInstance_Use3D

> ``Apply3D`` assumes the emitter and listener are using right-handed coordinates. You can pass 'false' for the _rhcoords_ parameter which defaults to 'true' if using left-handed coordinates.

# Using wave banks
Rather than loading individual ``.wav`` files, a more efficient method is to package them into a  "wave bank". This allows for more efficient loading and memory organization. DirectXTK for Audio's WaveBank class can be used to play one-shots or to create SoundEffectInstances from 'in-memory' wave banks.

    std::unique_ptr<WaveBank> wb( new WaveBank( audEngine.get(),
        L"wavebank.xwb" ) );

A SoundEffectInstance can be created from a wavebank referencing a particular wave in the bank:

    auto effect = wb->CreateInstance( 10 );
    if ( !effect )
        // Error (invalid index for wave bank)

    ...

    effect->Play( true );

One-shot sounds can also be played directly from the wave bank.

    wb->Play( 2 );
    wb->Play( 6 );

XACT3-style "wave banks" can be created by using the [[XWBTool]] command-line tool, or they can be authored using XACT3 in the DirectX SDK. Note that the XWBTool will not perform any format conversions or compression, so more full-featured options are better handled with the XACT3 GUI or XACTBLD, or it can be used on ``.wav`` files already compressed by ``adpcmencode.exe``, ``xwmaencode.exe``, ``xma2encode.exe``, etc.

    xwbtool -o wavebank.xwb Sound.wav Explosion.wav Music.wav

DirectXTK for Audio does not make use of the XACT engine, nor does it make use of XACT "sound banks" ``.xsb`` or "cues". We only use ``.xwb`` wave banks as a method for packing ``.wav`` data.

# Voice management
Each instance of a SoundEffectInstance will allocate it's own source voice when played, which won't be released until it is destroyed. Each time a one-shot sound is played from a SoundEffect or a WaveBank, a voice will be created or a previously used one-shot voice will be reused if possible.

See [[AudioEngine]] for more information.

# Platform support
Windows 8.x, Windows 10, Windows phone 8.x, and Xbox One all include XAudio 2.8 or later. Therefore, the standard ``DirectXTK.lib`` includes _DirectXTK for Audio_ for all these platforms:
* _DirectXTK_Windows10_
* _DirectXTK_Windows81_
* _DirectXTK_WindowsPhone81_
* _DirectXTK_XAMLSilverlight_WindowsPhone81_ 
* _DirectXTK_WindowsPhone8_
* _DirectXTK_XboxOneXDK_2015_ 
* _DirectXTK_Desktop_2015_Win10_

For Windows desktop applications targeting Windows 8.x or later, you can make use of XAudio 2.8. The ``DirectXTKAudioWin8.lib`` contains the XAudio 2.8 version of DirectXTK for Audio, while ``DirectXTK.lib`` for Windows desktop contains only the math/graphics components. To support Windows desktop applications on Windows 7 and Windows Vista, we must make use XAudio 2.7, the legacy DirectX SDK, and the legacy DirectX End-User Runtime Redistribution packages (aka DirectSetup). The ``DirectXTKAudioDX.lib`` is the XAudio 2.7 version of DirectXTK for Audio.

DirectXTK_Desktop_2015 and DirectXTK_Desktop_2013 do not include _DirectXTK for Audio_. To add _DirectXTK for Audio_ support for a Win32 desktop application, you must also add one of the following projects from the ``Audio`` folder of the distribution to your solution and **Add a Reference** to it (see [[DirectXTK]] for more details).

When targeting Windows 8.x or later:
* _DirectXTKAudio_Desktop_2013_Win8_ - DirectXTK for Audio using VS 2013 and XAudio 2.8
* _DirectXTKAudio_Desktop_2015_Win8_ - DirectXTK for Audio using VS 2015 and XAudio 2.8

When targeting Windows Vista or Windows 7:
* _DirectXTKAudio_Desktop_2013_DXSDK_ - DirectXTK for Audio project for VS 2013 + Windows 8.1 SDK + legacy DirectXTK using XAudio 2.7
* _DirectXTKAudio_Desktop_2015_DXSDK_ - DirectXTK for Audio project for VS 2015 + Windows 8.1 SDK + legacy DirectXTK using XAudio 2.7

[XAudio2 Versions](http://msdn.microsoft.com/en-us/library/windows/desktop/ee415802.aspx)

[KB2728613](http://support.microsoft.com/kb/2728613)

[Where is the DirectX SDK?](http://msdn.microsoft.com/en-us/library/windows/desktop/ee663275.aspx)

DirectXTK makes use of the latest Direct3D 11.1 headers available in the Windows 8.x SDK, and there are a number of file conflicts between the Windows 8.x SDK and the legacy DirectX SDK. Therefore, when building for down-level support with XAudio 2.7, ``Audio.h`` explicitly includes the DirectX SDK version of XAudio2 headers with a full path name. These reflect the default install locations, and if you have installed it elsewhere you will need to update this header. The ``*_DXSDK.vcxproj`` files use the ``DXSDK_DIR`` environment variable, so only the ``Audio.h`` references need updating for an alternative location.

    // Using XAudio 2.7 requires the DirectX SDK
    #include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\comdecl.h>
    #include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>
    #include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2fx.h>
    #pragma warning(push)
    #pragma warning( disable : 4005 )
    #include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\x3daudio.h>

# Content Pipeline
**Note:** When adding ``.xwb`` files to your Windows Store app or Windows phone app project, you need to manually set the file properties to "Content: Yes" for all configurations to have these files included in your AppX package. ``.wav`` files are automatically detected as a media file and are included as content by default.

# Threading model
The DirectXTK for Audio methods assume it is always called from a single thread. This is generally either the main thread or a worker thread dedicated to audio processing.  The XAudio2 engine itself makes use of lock-free mechanism to make it 'thread-safe'.
    
Note that ``IVoiceNotify::OnBufferEnd`` is called from XAudio2's thread, so the callback must be very fast and use thread-safe operations.

# Further reading

[Learning XAudio2](http://blogs.msdn.com/b/chuckw/archive/2012/05/15/learning-xaudio2.aspx)

[XAudio2 and Windows 8](http://blogs.msdn.com/b/chuckw/archive/2012/04/02/xaudio2-and-windows-8-consumer-preview.aspx)

[SoundLab](http://xbox.create.msdn.com/en-US/education/catalog/utility/soundlab)