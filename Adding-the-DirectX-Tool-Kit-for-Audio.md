This lesson covers adding _DirectX Tool Kit for Audio_ to your project.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# NuGet package manager
If you used NuGet when [[Adding the DirectX Tool Kit]], then you may already have support for _DirectX Tool Kit for Audio._.

<table>
 <tr>
  <td>directxtk_desktop_2015</td>
  <td>This package is configured for Windows 7 Service Pack 1 support for graphics & input, but make use of <b>Audio 2.8</b> for <I>DirectX Tool Kit for Audio</I>. Therefore audio support with NuGet requires Windows 8 or later.</td>
 </tr>
 <tr>
  <td>directxtk_uwp<br />directxtk12_uwp<br />directxtk12_desktop_2015</td>
  <td>These packages are configured for Windows 10 support for graphics, input, and make use of <b>XAudio 2.9</b> for <I>DirectX Tool Kit for Audio</I>.</td>
 </tr>
</table>

If you need Windows 7 Service Pack 1 support for audio, then use *Project-to-project references* instead of NuGet for graphics, input, and audio.

> See [this blog post](https://walbourn.github.io/github-nuget-and-vso/) for details on why it's set up this way.

Complete the steps in **Adding the headers** below.

# Project-to-project references
If you used project-to-project references when [[Adding the DirectX Tool Kit]], then you need to add an additional _DirectX Tool Kit for Audio_ project to your solution. There are four (4) choices depending on your platform target and deployment requirements:

## XAudio 2.9

XAudio 2.9 is built into Windows 10. Everything required is included in the operating system and the Windows 10 SDK. This version of _DirectX Tool Kit for Audio_ is already included in those projects (``DirectXTK*_Windows10_201x.vcxproj``, ``DirectXTK*_Desktop_201x_Win10.vcxproj``,) and Xbox One (``DirectXTK*_XboxOneXDK_201x.vcxproj``).

Complete the steps in **Adding the headers** below.

## XAudio 2.8

XAudio 2.8 is built into Windows 8.0 or later. Everything required is included in the operating system and the Windows 10 SDK.

1. Right-click on your solution in the Solution Explorer, and select **Add** / **Existing Project...**
1. Browse into the "DirectXTK\Audio" folder and select ``DirectXTKAudio_Desktop_201x_Win8.vcxproj``, click "Open"
1. If Visual Studio presents a "Security Warning", select "OK". Optional: Uncheck "Ask me for every project in this solution" first.
1. Right-click on your project in the Solution Explorer, and select **Add** / **References...**
1. Select "Add New Reference..."
1. Check ``DirectXTKAudio_Desktop_201x_Win8.vcxproj`` and select "OK"
1. Select "OK"

![Add Reference (Windows 8)](https://github.com/Microsoft/DirectXTK/wiki/images/AddReferenceWin8.png)

Complete the steps in **Adding the headers** below including the additional configuration for XAudio 2.8.

## XAudio2 Redistributable 
There is an XAudio 2.9 redistributable package available on [NuGet](https://www.nuget.org/packages/Microsoft.XAudio2.Redist/) that supports Windows 7 SP1, Windows 8, Windows 8.1, and Windows 10. The required runtime DLL is included side-by-side with your application, and avoids the need to include any legacy DirectX SDK redist package.

1. Right-click on your solution in the Solution Explorer, and select **Add** / **Existing Project...**
1. Browse into the "DirectXTK\Audio" folder and select ``DirectXTKAudio_Desktop_201x_Win7.vcxproj``, click "Open"
1. If Visual Studio presents a "Security Warning", select "OK". Optional: Uncheck "Ask me for every project in this solution" first.
1. Right-click on your project in the Solution Explorer, and select **Add** / **References...**
1. Select "Add New Reference..."
1. Check ``DirectXTKAudio_Desktop_201x_Win7.vcxproj`` and select "OK"
1. Select "OK"

In addition to the reference, you'll need to add the ``Microsoft.XAudio2.Redist`` NuGet package to your project(s) to ensure you have the correct XAudio2 headers and link libraries for this option.

Complete the steps in **Adding the headers** below.

> This is the preferred option for supporting XAudio2 on Windows 7, and is also a good choice if you want xWMA format support on Windows 8.x.

See [Microsoft Docs](https://aka.ms/xaudio2redist).

## XAudio 2.7
XAudio 2.7 supports Windows 7, Windows Vista, and Windows XP Service Pack 3. It is deployed by the legacy _DirectX End User Runtime_ package and requires the legacy DirectX SDK (June 2010) to develop with. Due to some technical issues, it must be installed to the 'default' location of ``C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)``. To install the legacy DirectX SDK on your development system, see [this post](https://walbourn.github.io/announcement-directx-sdk-june-2010-is-live/) and be sure to read [this article](https://walbourn.github.io/known-issue-directx-sdk-june-2010-setup-and-the-s1023-error/) for a known issue with the installer.

1. Right-click on your solution in the Solution Explorer, and select **Add** / **Existing Project...**
1. Browse into the "DirectXTK\Audio" folder and select ``DirectXTKAudio_Desktop_201x_DXSDK.vcxproj``, click "Open"
1. If Visual Studio presents a "Security Warning", select "OK". Optional: Uncheck "Ask me for every project in this solution" first.
1. Right-click on your project in the Solution Explorer, and select **Add** / **References...**
1. Select "Add New Reference..."
1. Check ``DirectXTKAudio_Desktop_201x_DXSDK.vcxproj`` and select "OK"
1. Select "OK"

![Add Reference (DirectX SDK)](https://github.com/Microsoft/DirectXTK/wiki/images/AddReferenceDX.png)

Complete the steps in **Adding the headers** below including the additional configuration for XAudio 2.7.

> This option is not recommended, but is included for completeness.

# Adding the headers
Now that we have the _DirectX Tool Kit for Audio_ usable in your project, the next step is to include the library header into your project.

```cpp
//
// pch.h
// Header for standard system include files.
//

#pragma once

...

#include "Audio.h"

...
```

## XAudio 2.9
For a desktop application that requires Windows 10, in your *pch.h* modify the following section:

```cpp
#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
```

> For Windows 10 platforms other than Windows desktop, you do not need to explicitly set ``_WIN32_WINNT`` as it's already set appropriately.

## XAudio 2.8
If you are using XAudio 2.8, then your application should be built to require Windows 8.0 or later. In *pch.h* modify the following section:

```cpp
#include <winsdkver.h>
#define _WIN32_WINNT 0x0602
#include <sdkddkver.h>
```

## XAudio 2.7
If you are using XAudio 2.7 for Windows 7 compatibility, we also need to add the legacy DirectX SDK include and library paths to your project. First go to **Project** / **Properties** and select "VC++ Directories" on the left. Then set Configuration to "All Configurations" and Platform to "Win32" (note this is called "x86" in VS 2015). Add to the _end_ of these paths:
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
If you are using XAudio 2.8, then your application has a dependency on Windows 8.0 or later. If using XAudio 2.9, your application will only run on Windows 10 devices.

If you are using the XAudio2 Redistributable, then you will need to include the ``XAUDIO2_9REDIST.DLL`` from the NuGet package as part of your application.

If you are using XAudio 2.7, then your application has a dependency on the _DirectX End-User Runtime_. See [Not So DirectSetup](https://aka.ms/dxsetup) for information on using this package. *Again, this is not recommended for use.*

**Next lesson:** [[Adding audio to your project]]

# Further reading
DirectX Tool Kit docs [[Audio]]  
[DirectX Tool Kit for Audio](https://walbourn.github.io/directx-tool-kit-for-audio/)  
[XAudio2 and Windows 8](https://walbourn.github.io/xaudio2-and-windows-8/)  
[Twilight for Windows 7](https://walbourn.github.io/twilight-for-windows-7/)

