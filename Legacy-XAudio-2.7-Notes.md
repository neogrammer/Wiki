> The release for XAudio2Redist has removed the need to use the legacy DirectX SDK, so May 2020 was the last release to support the option for using the legacy DirectX SDK's XAudio 2.7.

XAudio 2.7 supports Windows 7 and older operating systems. It is deployed by the legacy _DirectX End User Runtime_ package and requires the legacy DirectX SDK (June 2010) to develop with. Due to some technical issues, it must be installed to the 'default' location of ``C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)``. To install the legacy DirectX SDK on your development system, see [this post](https://walbourn.github.io/announcement-directx-sdk-june-2010-is-live/) and be sure to read [this article](https://walbourn.github.io/known-issue-directx-sdk-june-2010-setup-and-the-s1023-error/) for a known issue with the installer.

## Legacy DirectX SDK
DirectXTK makes use of the latest Direct3D 11.1 headers available in the Windows 8.x / 10 SDK, and there are a number of file conflicts between the Windows 8.x / 10 SDK and the legacy DirectX SDK. Therefore, when building for down-level support with XAudio 2.7, ``Audio.h`` explicitly includes the DirectX SDK version of XAudio2 headers with a full path name. These reflect the default install locations, and if you have installed it elsewhere you will need to update this header. The ``*_DXSDK.vcxproj`` files use the ``DXSDK_DIR`` environment variable, so only the ``Audio.h`` references need updating for an alternative location.

```cpp
// Using XAudio 2.7 requires the DirectX SDK
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\comdecl.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2fx.h>
#pragma warning(push)
#pragma warning( disable : 4005 )
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\x3daudio.h>
```

[KB2728613](http://support.microsoft.com/kb/2728613)

[Where is the DirectX SDK?](https://docs.microsoft.com/en-us/windows/desktop/directx-sdk--august-2009-)

> When using the legacy DirectX SDK you need to set up VC++ Directories paths in your project (particularly your EXE/DLL). For the Windows 8.1 SDK or Windows 10 SDK, you need to set up those paths in _reverse_ order from previous include orders. You really only need a small portion of the legacy DirectX SDK for XAudio 2.7, and want to be using the Windows 8.1 SDK / Windows 10 SDK for everything else. For more details see [The Zombie DirectX SDK](https://aka.ms/AA4gfea).

# XAudio 2.7
To use XAudio 2.7, you u se one of the Win32 desktop DirectX Tool Kit configurations for graphics, input, and math then add the *DirectX Tool Kit for Audio* project:

<table>
 <tr>
  <td>DirectXTKAudio_Desktop_2017_DXSDK</td>
  <td>When targeting Windows 7 with the legacy DirectX SDK, use <code>DirectXTKAudioDX.lib</code> which is implemented using XAudio 2.7. <I>Use of the legacy DirectX SDK is not recommended, and requires use the DirectSetup to deploy the XAudio 2.7 DLL to end-user machines</I>.</td>
 </tr>
</table>

1. Right-click on your solution in the Solution Explorer, and select **Add** / **Existing Project...**
1. Browse into the "DirectXTK\Audio" folder and select ``DirectXTKAudio_Desktop_201x_DXSDK.vcxproj``, click "Open"
1. If Visual Studio presents a "Security Warning", select "OK". Optional: Uncheck "Ask me for every project in this solution" first.
1. Right-click on your project in the Solution Explorer, and select **Add** / **References...**
1. Check ``DirectXTKAudio_Desktop_201x_DXSDK.vcxproj`` and select "OK"
1. Select "OK"

![Add Reference (DirectX SDK)](https://github.com/Microsoft/DirectXTK/wiki/images/AddReferenceDX.png)

> This option is not recommended, but is included for completeness. Xaudio 2.7 has a number of known bugs.

# Adding the headers

If you are using XAudio 2.7 for Windows 7 compatibility (``_WIN32_WINNT`` set to ``0x0601``), we also need to add the legacy DirectX SDK include and library paths to your project. First go to **Project** / **Properties** and select "VC++ Directories" on the left. Then set Configuration to "All Configurations" and Platform to "x86". Add to the _end_ of these paths:
* **Include Directories:** ``;$(DXSDK_DIR)Include``
* **Library Directories:** ``;$(DXSDK_DIR)Lib\x86``

Click "Apply".

![VC++ Directories (x86)](https://github.com/Microsoft/DirectXTK/wiki/images/settingsDXx86.png)

Then set Configuration to "All Configurations" and Platform to "x64". Add to the _end_ of these paths:
* **Include Directories:** ``;$(DXSDK_DIR)Include``
* **Library Directories:** ``;$(DXSDK_DIR)Lib\x64``

Click "Apply".

![VC++ Directories (x64)](https://github.com/Microsoft/DirectXTK/wiki/images/settingsDXx64.png)

It is important that the legacy DirectX SDK paths be after the existing path since are making use of the Windows 8.1 / 10 SDK. See [Where is the DirectX SDK?](https://docs.microsoft.com/en-us/windows/desktop/directx-sdk--august-2009-) for more details.

> _Troubleshooting:_ If you get a compilation error indicating you are missing ``comdecl.h``, then you have incorrectly configured your VC++ Directory include paths. If you get a link error indicating you are missing ``x3daudio.lib`` then you incorrectly configured your VC++ Directory library paths.

> _Troubleshooting:_ If you get a compilation error indicating you are missing ``comdecl.h``, then you have incorrectly configured the ``_WIN32_WINNT`` variable. See [Using the Windows Headers](https://docs.microsoft.com/en-us/windows/desktop/WinProg/using-the-windows-headers).

# Deployment

If you are using XAudio 2.7, then your application has a dependency on the _DirectX End-User Runtime_. See [Not So DirectSetup](https://aka.ms/dxsetup) for information on using this package. *Again, this is not recommended for use.*

# Troubleshooting
When using the debug version of XAudio 2.7 on Windows 7, you can get a break-point exception thrown even with a valid device. The debug output window message will be the following. You can safely ignore this and click "Continue".

```
################################################################################
### XAUDIO2: enginerendererconnection.cpp:334:
### ASSERT FAILED: FramesToLeapTime(uOutputBufferFramesObtained, m_pOutputFormat, RoundUp) >= rtRequestedBufferDuration
################################################################################
```

See [[Adding the DirectX Tool Kit for Audio]]
