This class represents an XAudio2 audio graph, device, and mastering voice

# Header
    #include <Audio.h>

# Initialization
This creates an XAudio2 interface, an XAudio2 mastering voice, and other global resources.

XAudio2 requires COM be initialized as a prerequisite using ``Windows::Foundation::Initialize``, ``CoInitialize``, or ``CoInitializeEx``.

    // This is only needed in Win32 desktop apps
    CoInitializeEx( nullptr, COINIT_MULTITHREADED );

All _DirectXTK for Audio_ components require an AudioEngine instance. For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr``.

    std::unique_ptr<AudioEngine> audEngine;
    audEngine = std::make_unique<AudioEngine>();

The constructor optionally takes a number of parameters:

    AudioEngine( AUDIO_ENGINE_FLAGS flags = AudioEngine_Default,
        const WAVEFORMATEX* wfx = nullptr,
        const wchar_t* deviceId = nullptr,
        AUDIO_STREAM_CATEGORY category = AudioCategory_GameEffects );

# Parameters

*flags*: A combination of audio engine flags. Defaults to ``AudioEngine_Default``. 

* ``AudioEngine_Debug`` - Enables debugging facilities. For XAudio 2.7 this requires the Developer Runtime.

* ``AudioEngine_EnvironmentalReverb`` - Enables I3DL2 environmental reverb effects for 3D positional audio.
* ``AudioEngine_ReverbUseFilters`` - An additional feature for 3D positional audio reverb. Requires ``AudioEngine_EnvironmentalReverb``.

* ``AudioEngine_UseMasteringLimiter`` - Enables a mastering volume limiter xAPO for the mastering voice. This is recommended for avoiding distortion or clipping, particularly when applying 3D positional audio.

* ``AudioEngine_DisableVoiceReuse`` - By default, _DirectXTK for Audio_ will reuse XAudio2 source voices for one-shots if possible. If using this flag, voice reuse is disabled and one-shot sounds will create and destroy source voices on every Play.

* ``AudioEngine_ThrowOnNoAudioHW`` - By default, if there is no audio hardware available _DirectXTK for Audio_ will automatically enter 'silent mode'. If this flag is provided, a C++ exception is generated instead.
: Note: ``AUDIO_ENGINE_FLAGS`` is used as a typed flag enum. Only operator| is overloaded to combine them, so operations like |= are not available without additional static_cast<> statements.

*wfx*: Specifies the output format for the XAudio2 mastering voice (primarily the channel count and sample rate). If null, it uses the device defaults.

*deviceId*: Specifies the output debug for the XAudio2 mastering voice.

> When using XAudio 2.8 or 2.9, this must be a WinRT device identifier, while on XAudio 2.7 this is a [WASAPI](http://msdn.microsoft.com/en-us/library/windows/desktop/dd371455.aspx) audio end-point identifier. If null, it uses the default audio device.

*category*: Specifies the audio end-point category for the XAudio2 mastering voice. On XAudio 2.7, this value is ignored.

# Debugging facilities
When creating the AudioEngine, you can specify enabling of XAudio2 debugging facilities by including the AudioEngine_Debug flag.

    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
    #ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
    #endif
    audEngine = std::make_unique<AudioEngine>( eflags );

[XAudio2 Debugging Facilities](http://msdn.microsoft.com/en-us/library/windows/desktop/ee415752.aspx)

> With XAudio 2.7, the developer runtime must be installed on the system for the AudioEngine_Debug case to
>  succeed. With XAudio 2.8 on Windows 8.x, the debug support is built into the OS.

# Device enumeration

AudioEngine provides a simple wrapper over the platform-specific audio device enumeration:

    auto enumList = AudioEngine::GetRendererDetails();

    if ( enumList.empty() )
    {
        // No audio devices
    }
    else
    {
       for( auto it = enumList.cbegin(); it != enumList.cend(); ++it )
       {
           // it->deviceId.c_str() - this is the device/end-point identifier you can
           //     pass as a parameter to AudioEngine
           // it->description.c_str() - this is a textual description
       }
    }

**Windows phone 8 and Xbox One**: The enumeration only returns the 'default' audio device identifier with the description "Default"

# Silent mode
If the initial device creation fails to find an audio device, by default AudioEngine creation will succeed and be in a 'silent' mode--if ``AudioEngine_ThrowOnNoAudioHW`` is given, then it will throw a C++ exception instead. This allows various DirectXTK for Audio objects to be created and methods called, but no audio processing will take place. This can be detected either by a 'false' return from **Update** or calling **IsAudioDevicePresent**.

At some later point (typically after detecting a new audio device is available on the system), you can retry by calling **Reset**. If this returns 'true', then the AudioEngine is no longer in 'silent' mode. After that, future calls to SoundEffect or WaveBank's Play() will result in one-shots being fired. All SoundEffectInstances are in a stopped state, but can be started after the successful call to ``Reset``.

    bool Reset( const WAVEFORMATEX* wfx = nullptr, const wchar_t* deviceId = nullptr );

Parameters to **Reset** are the similar as for the AudioEngine constructor. If the original object was created with ``AudioEngine_ThrowOnNoAudioHW``, then ``Reset`` will throw if no default audio device is found.

> For XAudio 2.7 and XAudio 2.8 (Windows 8.x), this logic also handles the 'lost endpoint' scenario that happens if you unplug speakers or headphones. With XAudio 2.9 (Windows 10), if you use a ``deviceId`` of ``nullptr`` (i.e. the default), then the new Windows 10 WASAPI feature Virtual Audio Client is used which prevents the 'lost endpoint' case from being triggered.

## Win32 desktop applications
For Win32 desktop applications, you can be informed of new audio devices in your application with [RegisterDeviceNotification](http://msdn.microsoft.com/en-us/library/windows/desktop/aa363431.aspx):

    #include <dbt.h>

    ...

    DEV_BROADCAST_DEVICEINTERFACE filter = {};
    filter.dbcc_size = sizeof( filter );
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter.dbcc_classguid = KSCATEGORY_AUDIO;

    HDEVNOTIFY hNewAudio = RegisterDeviceNotification( hwnd, &filter,
        DEVICE_NOTIFY_WINDOW_HANDLE );

    ...

    case WM_DEVICECHANGE:
        if ( wParam == DBT_DEVICEARRIVAL ) {
            auto pDev = reinterpret_cast<PDEV_BROADCAST_HDR>( lParam );
            if( pDev ) {
                if ( pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE ) {
                    auto pInter =
                        reinterpret_cast<const PDEV_BROADCAST_DEVICEINTERFACE>( pDev );
                    if ( pInter->dbcc_classguid == KSCATEGORY_AUDIO ) {
                        // New audio device

Or you can make use of [IMMNotificationClient](http://msdn.microsoft.com/en-us/library/windows/desktop/dd371417.aspx)

## Windows Store applications

You can be informed of new audio devices by using the WinRT class DeviceWatcher in the ``Windows.Device.Enumeration`` namespace.

[Device Enumeration Sample](http://code.msdn.microsoft.com/windowsapps/Device-Enumeration-Sample-a6e45169)

# Per-frame processing
**Update** should be called often, usually in a per-frame update. This can be done on the main rendering thread, or from a worker thread. This returns false if the audio engine is the 'silent' mode.

    if ( !audEngine->Update() )
    {
        // No audio device is active
        if ( audEngine->IsCriticalError() )
        {
            ...
        }    
    }

Note that if XAudio2 encounters a critical error (typically because the current audio device is disconnected or on the Windows platform if the speakers are unplugged from the current audio device), then the audio engine will automatically be in 'silent' mode. This can be detected either by a 'false' return from **Update** or calling **IsCriticalError**. If this occurs, you should try calling **Reset** to try the new 'default' device' if there is one. If that retry fails, you should wait until a new audio device is available (ala 'silent' mode above).

# Pausing
Typically games will suspend audio when paused or the game is 'tabbed away' from. **Suspend** and **Resume** can be used to globally halt/restart audio processing.

# Positional 3D audio
AudioEngine by default supports both standard and positional 3D audio voices, but without environmental reverb. If ``AudioEngine_EnvironmentalReverb`` is given, then a reverb effect is created and used for all 3D audio voices--it is not applied to standard voices. You can use **SetReverb** with I3DL2 presets or provide 'native' reverb settings.

    audEngine->SetReverb( Reverb_Quarry );

    ...

    audEngine->SetReverb( Reverb_Off );

Optionally ``AudioEngine_ReverbUseFilters`` can be used with ``AudioEngine_EnvironmentalReverb`` to provide some additional reverb/occlusion effects.

# Voice management

* **SetDefaultSampleRate**( sampleRate ) is used to control the sample rate for voices in the one-shot pool (see ``Play`` in [[SoundEffect]] and [[WaveBank]]). This should be the same rate as used by the majority of your content. It defaults to 44100 Hz. This value is never used if ``AudioEngine_DisableVoiceReuse``  was specified.

* **TrimVoicePool** can be used to free up any source voices in the 'idle' list for one-shots, and will cause all non-playing [[SoundEffectInstance]] objects to release their source voice. This is used for keeping the total source voice count under a limit for performance or when switching sections where audio content formats change dramatically.

> One-shots voices must have completely stopped playing before they are put into the idle pool. You should
> therefore ensure ``Update`` is called twice with some delay between them before calling ``TrimVoicePool`` for
> maximum effectiveness.

* **SetMaxVoicePool** ( maxOneShots, maxInstances ) can be used to set a limit on the number of one-shot source voices allocated and/or to put a limit on the source voices available for [[SoundEffectInstance]]. If there are insufficient one-shot voices, those sounds will not be heard. If there are insufficient voices for a SoundEffectInstance to play, then a C++ exception is thrown. These values default to 'unlimited'.

# Mastering volume limiter

The XAudio2 audio renderer makes use of single-precision floating-point values, which can exceed the range of the audio hardware, particularly after 3D positional audio computations, custom xAPOs, or volume settings greater than 1.0. This can produce distortion or other clipping artifacts in the final output.

Therefore, _DirectXTK for Audio_ supports attaching a _mastering volume limiter_ xAPO to the mastering voice by setting the ``AudioEngine_UseMasteringLimiter`` flag. It uses default settings, which can be modified by calling **SetMasteringLimit**( release, loudness ).

See [FXMASTERINGLIMITER_PARAMETERS](http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.xapofx.fxmasteringlimiter_parameters.aspx)

    AUDIO_ENGINE_FLAGS eflags = AudioEngine_UseMasteringLimiter;
    #ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
    #endif
    audEngine = std::make_unique<AudioEngine>( eflags );

# Statistics

The **GetStatistics** function returns information on the number of playing sounds, allocated instances, audio bytes in loaded [[SoundEffect]] and [[WaveBank]] objects, and XAudio2 source voices allocated for various purposes. These values do not rely on debug faculties to be enabled.

    auto stats = engine->GetStatistics();
    printf( "\nPlaying: %Iu / %Iu; Instances %Iu; Voices %Iu / %Iu / %Iu / %Iu;"
            "%Iu audio bytes\n",
        stats.playingOneShots, stats.playingInstances,
        stats.allocatedInstances, stats.allocatedVoices, stats.allocatedVoices3d,
        stats.allocatedVoicesOneShot, stats.allocatedVoicesIdle,
        stats.audioBytes );

# Properties

* **GetOutputFormat**: Returns the format consumed by the mastering voice (which is the same as the device output if defaults are used) as a ``WAVEFORMATEXTENSIBLE``.

* **GetChannelMask**: Returns the output channel mask

* **GetOutputChannels**: Returns the number of output channels

* **IsAudioDevicePresent**: Returns true if the audio graph is operating normally, false if in 'silent mode'

* **IsCriticalError**: Returns true if the audio graph is halted due to a critical error (which also places the engine into 'silent mode')

# Low-level interface access

The following methods can be used to obtain the low-level XAudio2 interface objects used by DirectXTK for Audio. Be sure to also use **RegisterNotify**, **UnregisterNotify**, and the ``IVoiceNotify`` interface to get proper notifications from the XAudio2 engine. You should prefer to use **AllocateVoice** to creating your own source voices if possible, and be sure to use **DestroyVoice** to free the source voice if it is not a 'oneshot' voice. One shot voices are managed and cleaned up by ``AudioEngine::Update``.

* **GetInterface**: Returns ``IXAudio2*`` 
* **GetMasterVoice**: Returns ``IXAudio2MasteringVoice*``
* **GetReverbVoice**: Returns ``IXAudio2SubmixVoice*``
* **Get3DHandle**: Returns ``X3DAUDIO_HANDLE&``

# Further reading

[XAudio2 APIs](http://msdn.microsoft.com/en-us/library/windows/desktop/hh405049.aspx)  
[Audio Effects](http://msdn.microsoft.com/en-us/library/windows/desktop/ee415754.aspx)  
[X3DAudio](http://msdn.microsoft.com/en-us/library/windows/desktop/ee415714.aspx)  

