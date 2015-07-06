This lesson covers adding _DirectX Tool Kit for Audio_ to your project.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# NuGet package manager
If you used NuGet when [[Adding the DirectX Tool Kit]], then you already have support for _DirectX Tool Kit for Audio._. The id ``directxtk_desktop_2013`` package is configured for Windows 7 and Windows Vista support, so it is making use of **XAudio 2.7**.

XAudio 2.7 requires the legacy DirectX SDK (June 2010). Due to some technical issues, it must be installed to the 'default' location of ``C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)``. To install the legacy DirectX SDK on your development system, see [this post](http://blogs.msdn.com/b/chuckw/archive/2010/06/15/10023137.aspx) and be sure to read [this article](http://blogs.msdn.com/b/chuckw/archive/2013/09/24/10246203.aspx) for a known issue with the installer.

Complete the steps in **Adding the headers** below including the additional configuration for XAudio 2.7.

_The other platform NuGet packages do not require the legacy DirectX SDK as they always make use of XAudio 2.8 or later which is built into Windows 8.0 or later, Windows phone 8.x, and Xbox One._

# Project-to-project references
If you used project-to-project references when [[Adding the DirectX Tool Kit]], then you need to add an additional _DirectX Tool Kit for Audio_ project to your solution. There are two choices depending on your platform target.

## XAudio 2.7
XAudio 2.7 supports Windows Vista, Windows 7, or later. It is deployed by the legacy _DirectX End User Runtime_ package and requires the legacy DirectX SDK (June 2010) to develop with. Due to some technical issues, it must be installed to the 'default' location of ``C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)``. To install the legacy DirectX SDK on your development system, see [this post](http://blogs.msdn.com/b/chuckw/archive/2010/06/15/10023137.aspx) and be sure to read [this article](http://blogs.msdn.com/b/chuckw/archive/2013/09/24/10246203.aspx) for a known issue with the installer.

1. Right-click on your solution in the Solution Explorer, and select **Add** / **Existing Project...**
1. Browse into the "DirectXTK\Audio" folder and select ``DirectXTKAudio_Desktop_2013_DXSDK.vcxproj``, click "Open"
1. If Visual Studio presents a "Security Warning", select "OK". Optional: Uncheck "Ask me for every project in this solution" first.
1. Right-click on your project in the Solution Explorer, and select **Add** / **References...**
1. Select "Add New Reference..."
1. Check ``DirectXTKAudio_Desktop_2013_DXSDK.vcxproj`` and select "OK"
1. Select "OK"

![Add Reference (DirectX SDK)](https://github.com/Microsoft/DirectXTK/wiki/images/AddReferenceDX.png)

Complete the steps in **Adding the headers** below including the additional configuration for XAudio 2.7.

## XAudio 2.8
XAudio 2.8 is built into Windows 8.0 or later, and everything needed is include with the OS and the Windows 8.1 SDK which is deployed with VS 2013.

1. Right-click on your solution in the Solution Explorer, and select **Add** / **Existing Project...**
1. Browse into the "DirectXTK\Audio" folder and select ``DirectXTKAudio_Desktop_2013_Win8.vcxproj``, click "Open"
1. If Visual Studio presents a "Security Warning", select "OK". Optional: Uncheck "Ask me for every project in this solution" first.
1. Right-click on your project in the Solution Explorer, and select **Add** / **References...**
1. Select "Add New Reference..."
1. Check ``DirectXTKAudio_Desktop_2013_Win8.vcxproj`` and select "OK"
1. Select "OK"

![Add Reference (Windows 8)](https://github.com/Microsoft/DirectXTK/wiki/images/AddReferenceWin8.png)

Complete the steps in **Adding the headers** below including the additional configuration for XAudio 2.8.

_The other platform DirectX Tool Kit vcxproj files already include DirectX Tool Kit for Audio as they always make use of XAudio 2.8 or later which is built into Windows 8.0 or later, Windows phone 8.x, and Xbox One._

# Adding the headers
Now that we have the _DirectX Tool Kit for Audio_ usable in your project, the next step is to include the library header into your project.

    //
    // pch.h
    // Header for standard system include files.
    //

    #pragma once

    ...

    #include "Audio.h"

    ...

## XAudio 2.7
If you are using XAudio 2.7 for Windows 7 and Windows Vista compatibility, we also need to add the DirectX SDK include and library paths to your project. First go to **Project** / **Properties** and select "VC++ Directories" on the left. Then set Configuration to "All Configurations" and Platform to "Win32" (note this is called "x86" in VS 2015). Add to the _end_ of these paths:
* **Include Directories:** ``;$(DXSDK_DIR)Include``
* **Library Directories:** ``;$(DXSDK_DIR)Lib\x86``
* Click "Apply".

![VC++ Directories (x86)](https://github.com/Microsoft/DirectXTK/wiki/images/settingsDXx86.png)

Then set Configuration to "All Configurations" and Platform to "x64". Add to the _end_ of these paths:
* **Include Directories:** ``;$(DXSDK_DIR)Include``
* **Library Directories:** ``;$(DXSDK_DIR)Lib\x64``
* Click "Apply".

![VC++ Directories (x64)](https://github.com/Microsoft/DirectXTK/wiki/images/settingsDXx64.png)

It is important that the legacy DirectX SDK paths be after the existing path since are making use of the Windows 8.1 / VS 2013. See [Where is the DirectX SDK?](http://msdn.microsoft.com/en-us/library/windows/desktop/ee663275.aspx) for more details.

> _Troubleshooting:_ If you get a compilation error indicating you are missing ``comdecl.h``, then you have incorrectly configured your VC++ Directory include paths. If you get a link error indicating you are missing ``x3daudio.lib`` then you incorrectly configured your VC++ Directory library paths.

## XAudio 2.8

If you are using XAudio 2.8, then your application should be built to require Windows 8.0 or later. In *pch.h* modify the following section:

    #include <WinSDKVer.h>
    #define _WIN32_WINNT 0x0602
    #include <SDKDDKVer.h>

> _Troubleshooting:_ If you get a compilation error indicating you are missing ``comdecl.h``, then you have incorrectly configured the ``_WIN32_WINNT`` variable. See [http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745.aspx](http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745.aspx).

# Deployment
If you are using XAudio 2.7, then your application has a dependency on the _DirectX End-User Runtime_. See [Not So DirectSetup](http://blogs.msdn.com/b/chuckw/archive/2010/09/08/not-so-direct-setup.aspx) for information on using this package.

If you are using XAudio 2.8, then your application has a dependency on Windows 8.0 or later.

**Next lesson:** [[Adding audio to your project]]

# Further reading
DirectX Tool Kit docs [[Audio]]  
[DirectX Tool Kit for Audio](http://blogs.msdn.com/b/chuckw/archive/2013/12/25/directx-tool-kit-for-audio.aspx)  
[XAudio2 and Windows 8](http://blogs.msdn.com/b/chuckw/archive/2012/04/02/xaudio2-and-windows-8-consumer-preview.aspx)
