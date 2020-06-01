*DirectX Tool Kit for Audio* support for XAudio 2.7 is designed for use with the legacy DirectX SDK.

> The release for XAudio2Redist has removed the need to use the legacy DirectX SDK, so May 2020 was the last release to support this option.

XAudio 2.7 supports Windows 7 and older operating systems. It is deployed by the legacy _DirectX End User Runtime_ package and requires the legacy DirectX SDK (June 2010) to develop with. Due to some technical issues, it must be installed to the 'default' location of ``C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)``. To install the legacy DirectX SDK on your development system, see [this post](https://walbourn.github.io/announcement-directx-sdk-june-2010-is-live/) and be sure to read [this article](https://walbourn.github.io/known-issue-directx-sdk-june-2010-setup-and-the-s1023-error/) for a known issue with the installer.

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

See [[Adding the DirectX Tool Kit for Audio]]
