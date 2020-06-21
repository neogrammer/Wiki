This lesson covers adding _DirectX Tool Kit for Audio_ to your project.

# Setup
After creating a new project in the [[The basic game loop]] or [[Using DeviceResources]], then [[Adding the DirectX Tool Kit]] for this lesson.

# NuGet package manager
If you used [NuGet](https://docs.microsoft.com/en-us/nuget/what-is-nuget) when [[Adding the DirectX Tool Kit]], then you already have support for _DirectX Tool Kit for Audio._.

<table>
 <tr>
  <td>directxtk_desktop_2017</td>
  <td>This package is configured for Windows 7 Service Pack 1 support for graphics & input. <I>DirectX Tool Kit for Audio</I> is configured to use the XAudio2Redist NuGet package to support Windows 7 or later.</td>
 </tr>
 <tr>
  <td>directxtk_desktop_win10<br />directxtk_uwp<br />directxtk12_uwp<br />directxtk12_desktop_2017</td>
  <td>These packages are configured for Windows 10 support for graphics, input, and make use of <b>XAudio 2.9</b> for <I>DirectX Tool Kit for Audio</I>.</td>
 </tr>
</table>

Complete the steps in **Adding the headers** below including the additional configuration for *XAudio2 Redistributable* or *XAudio 2.9*.

# Project-to-project references
If you used project-to-project references when [[Adding the DirectX Tool Kit]], then you need to add an additional _DirectX Tool Kit for Audio_ project to your solution. There are three (3) choices depending on your platform target and deployment requirements:

## XAudio 2.9
XAudio 2.9 is built into Windows 10. Everything required is included in the operating system and the Windows 10 SDK. This version of _DirectX Tool Kit for Audio_ is already included in those projects (``DirectXTK*_Windows10_201x.vcxproj``, ``DirectXTK*_Desktop_201x_Win10.vcxproj``) and Xbox One (``DirectXTK*_XboxOneXDK_201x.vcxproj``).

Complete the steps in **Adding the headers** below including the additional configuration for XAudio 2.9.

## XAudio 2.8
XAudio 2.8 is built into Windows 8.0 or later. Everything required is included in the operating system and the Windows 10 SDK.

1. Right-click on your solution in the Solution Explorer, and select **Add** / **Existing Project...**
1. Browse into the "DirectXTK\Audio" folder and select ``DirectXTKAudio_Desktop_201x_Win8.vcxproj``, click "Open"
1. If Visual Studio presents a "Security Warning", select "OK". Optional: Uncheck "Ask me for every project in this solution" first.
1. Right-click on your project in the Solution Explorer, and select **Add** / **References...**
1. Check ``DirectXTKAudio_Desktop_201x_Win8.vcxproj`` and select "OK"
1. Select "OK"

![Add Reference (Windows 8)](https://github.com/Microsoft/DirectXTK/wiki/images/AddReferenceWin8.png)

Complete the steps in **Adding the headers** below including the additional configuration for XAudio 2.8.

> XAudio 2.8 does not support xWMA

## XAudio2 Redistributable
There is an XAudio 2.9 redistributable package available on [NuGet](https://www.nuget.org/packages/Microsoft.XAudio2.Redist/) that supports Windows 7 SP1, Windows 8, Windows 8.1, and Windows 10 for Win32 desktop applications. The required runtime DLL is included side-by-side with your application, and avoids the need to include any legacy DirectX SDK redist package.

1. Right-click on your solution in the Solution Explorer, and select **Add** / **Existing Project...**
1. Browse into the "DirectXTK\Audio" folder and select ``DirectXTKAudio_Desktop_201x_Win7.vcxproj``, click "Open"
1. If Visual Studio presents a "Security Warning", select "OK". Optional: Uncheck "Ask me for every project in this solution" first.
1. Right-click on your project in the Solution Explorer, and select **Add** / **References...**
1. Check ``DirectXTKAudio_Desktop_201x_Win7.vcxproj`` and select "OK"
1. Select "OK"

![Add Reference (Windows 7)](https://github.com/Microsoft/DirectXTK/wiki/images/AddReferenceWin7.png)

In addition to the reference, you'll need to add the ``Microsoft.XAudio2.Redist`` NuGet package to your project(s) to ensure you have the correct XAudio2 headers and link libraries for this option.

* From the drop-down menu, select **Project** / **Manage NuGet Packages...**
* Select "Browse" on the top tab, and make sure the _Package source_ is set to "nuget.org"
* In the text search field type "XAudio2" and hit enter to search for the packages
* Select the package with the id  **[Microsoft.XAudio2.Redist](https://www.nuget.org/packages/Microsoft.XAudio2.Redist/)**
* Select "Install"
* When finished, close the NuGet Manager

![NuGet Package Manager (Microsoft.XAudio2.Redist)](https://github.com/Microsoft/DirectXTK/wiki/images/nugetxaudio2.png)

Complete the steps in **Adding the headers** below including the additional configuration for the XAudio2 Redistributable.

> This is the preferred option for supporting XAudio2 on Windows 7, and is also a good choice if you want xWMA format support on Windows 8.x.

See [Microsoft Docs](https://aka.ms/xaudio2redist).

# CMake projects

The *DirectX Tool Kit* CMake projects have a build option for XAudio 2.9 (``BUILD_XAUDIO_WIN10``). The *DirectX Tool Kit for DX11* CMake project also has a build option for XAudio 2.8 (``BUILD_XAUDIO_WIN8``).

To utilize the *XAudio2Redist* you will need to integrate the NuGet package for your project and the *DirectX Tool Kit for DX11* CMake projects. If you use the *Visual Studio* generator and CMake 3.15 or later, you can utilize [VS_PACKAGE_REFERENCES](https://cmake.org/cmake/help/latest/prop_tgt/VS_PACKAGE_REFERENCES.html).

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

## XAudio2 Redistributable
If you want to support Windows 7, be sure to verify ``_WIN32_WINT`` is set to ``0x0601``.

> _Troubleshooting:_ If you get a compilation error related to DirectX SDK headers, then you likely don't have the ``Microsoft.XAudio2.Redist`` NuGet package set up for the project that is using ``Audio.h``

> _Troubleshooting:_ If you get runtime errors related to not being able to find ``XAUDIO2_9REDIST.DLL``, then you likely don't have the ``Microsoft.XAudio2.Redist`` NuGet package set up for the project that generates the EXE, or you need to include that DLL from the NuGet package in your installer/deployment.

# Deployment
If you are using XAudio 2.8, then your application has a dependency on Windows 8.0 or later. If using XAudio 2.9, your application will only run on Windows 10 devices.

If you are using the XAudio2 Redistributable, then you will need to include the ``XAUDIO2_9REDIST.DLL`` from the NuGet package as part of your application.

**Next lesson:** [[Adding audio to your project]]

[[Legacy XAudio 2.7 Notes]]

# Further reading
DirectX Tool Kit docs [[Audio]]  
[DirectX Tool Kit for Audio](https://walbourn.github.io/directx-tool-kit-for-audio/)  
[XAudio2 and Windows 8](https://walbourn.github.io/xaudio2-and-windows-8/)  
[Twilight for Windows 7](https://walbourn.github.io/twilight-for-windows-7/)

