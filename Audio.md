The _DirectXTK for Audio_ components implement a low-level audio API similar to XNA Game Studio's Microsoft.Xna.Framework.Audio. This consists of the following classes all declared in the Audio.h header (in the Inc folder of the distribution):

* AudioEngine - This class represents an XAudio2 audio graph, device, and mastering voice. 
* SoundEffect - A container class for sound resources which can be loaded from .wav files. 
* SoundEffectInstance - Provides a single playing, paused, or stopped instance of a sound 
* DynamicSoundEffectInstance - SoundEffectInstance where the application provides the audio data on demand 
* WaveBank - A container class for sound resources packaged into an XACT-style .xwb wave bank. 
* AudioListener,  AudioEmitter - Utility classes used with SoundEffectInstance::Apply3D.

_Note: DirectXTK for Audio uses XAudio 2.8 or XAudio 2.7. It does not make use of the legacy XACT Engine, XACT Cue, or XACT SoundBank._

# Platform support
Windows 8.x, Windows 10, Windows phone 8.x, and Xbox One all include XAudio 2.8 or later. Therefore, the standard DirectXTK.lib includes DirectXTK for Audio for all these platforms:
* _DirectXTK_Windows10_ 
* _DirectXTK_Windows81 _
* _DirectXTK_Windows8 _
* _DirectXTK_WindowsPhone81 _
* _DirectXTK_XAMLSilverlight_WindowsPhone81_ 
* _DirectXTK_WindowsPhone8 _
* _DirectXTK_XboxOneXDK_ 
* _DirectXTK_XboxOneADK_

For Windows desktop applications targeting Windows 8.x or later, you can make use of XAudio 2.8. The DirectXTKAudioWin8.lib contains the XAudio 2.8 version of DirectXTK for Audio, while DirectXTK.lib for Windows desktop contains only the math/graphics components. To support Windows desktop applications on Windows 7 and Windows Vista, we must make use XAudio 2.7, the legacy DirectX SDK, and the legacy DirectX End-User Runtime Redistribution packages (aka DirectSetup). The DirectXTKAudioDX.lib is the XAudio 2.7 version of DirectXTK for Audio.

DirectXTK_Desktop_2015, DirectXTK_Desktop_2013, DirectXTK_Desktop_2012, and DirectXTK_Desktop_2010 do not include DirectXTK for Audio. To add DirectXTK for Audio support for a Win32 desktop application, you must also add one of the following projects from the **Audio** folder of the distribution to your solution and Add a Reference to it (see [[DirectXTK]] for more details).

When targeting Windows 8.x or later:
* _DirectXTKAudio_Desktop_2012_Win8_ - DirectXTK for Audio using VS 2012 and XAudio 2.8 
* _DirectXTKAudio_Desktop_2013_Win8_ - DirectXTK for Audio using VS 2013 and XAudio 2.8
* _DirectXTKAudio_Desktop_2015_Win8_ - DirectXTK for Audio using VS 2015 and XAudio 2.8

When targeting Windows Vista, Windows 7, or Windows 8.x:
* _DirectXTKAudio_Desktop_2010_DXSDK_ - DirectXTK for Audio project for VS 2010 + Windows 8.1 SDK + legacy DirectXTK using XAudio 2.7 
* _DirectXTKAudio_Desktop_2012_DXSDK_ - DirectXTK for Audio project for VS 2012 + Windows 8.0 SDK + legacy DirectXTK using XAudio 2.7 
* _DirectXTKAudio_Desktop_2013_DXSDK_ - DirectXTK for Audio project for VS 2013 + Windows 8.1 SDK + legacy DirectXTK using XAudio 2.7

**VS 2010 Note:** We only support DirectXTK for Audio with the legacy DirectX SDK due to some issues with using the VS 2010 toolset and with Windows 8.x SDK WinRT headers.

[XAudio2 Versions](http://msdn.microsoft.com/en-us/library/windows/desktop/ee415802.aspx)
[KB2728613](http://support.microsoft.com/kb/2728613)
[Where is the DirectX SDK?](http://msdn.microsoft.com/en-us/library/windows/desktop/ee663275.aspx)

DirectXTK makes use of the latest Direct3D 11.1 headers available in the Windows 8.x SDK, and there are a number of file conflicts between the Windows 8.x SDK and the legacy DirectX SDK. Therefore, when building for down-level support with XAudio 2.7, Audio.h explicitly includes the DirectX SDK version of XAudio2 headers with a full path name. These reflect the default install locations, and if you have installed it elsewhere you will need to update this header. The *_DXSDK.vcxproj files use the DXSDK_DIR environment variable, so only the Audio.h references need updating for an alternative location.

    // Using XAudio 2.7 requires the DirectX SDK
    #include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\comdecl.h>
    #include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>
    #include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2fx.h>
    #pragma warning(push)
    #pragma warning( disable : 4005 )
    #include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\x3daudio.h>

# Content Pipeline
**Note:** When adding .xwb files to your Windows Store app or Windows phone app project, you need to manually set the file properties to "Content: Yes" for all configurations to have these files included in your AppX package. .wav files are automatically detected as a media file and are included as content by default.
