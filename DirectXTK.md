
# Headers
Public headers are in the **Inc** folder of the distribution package.

# Namespace
All the functions in the library are in the **DirectX** C++ namespace.

```cpp
using namespace DirectX;
```

> The Xbox One exclusive application *XBoxDDSTextureLoader* functions are in the **Xbox** C++ namespace.

# Modules
* [[Audio]] - low-level audio API using XAudio2
     * [[AudioEngine]], [[SoundEffect]], [[SoundEffectInstance]], [[SoundStreamInstance]], [[DynamicSoundEffectInstance]], [[WaveBank]], [[AudioListener]], [[AudioEmitter]]
* [[BufferHelpers]] - C++ helpers for creating D3D resources from CPU data
* [[CommonStates]] - factory providing commonly used D3D state objects
* [[DDSTextureLoader]] - light-weight DDS file texture loader
* [[DirectXHelpers]] - misc C++ helpers for D3D programming
* [[Effects]] - set of built-in shaders for common rendering tasks
     * [[IEffectFactory|EffectFactory]]
     * [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], [[IEffectFog]], [[IEffectSkinning]]
     * [[AlphaTestEffect]], [[BasicEffect]], [[DualTextureEffect]], [[EnvironmentMapEffect]], [[SkinnedEffect]]
     * [[NormalMapEffect]], [[PBREffect]], [[DebugEffect]]
     * [[DGSLEffect]]
* [[GamePad]] - gamepad controller helper using XInput
* [[GeometricPrimitive]] - draws basic shapes such as cubes and spheres
* [[GraphicsMemory]] - helper for managing dynamic graphics memory allocation
* [[Keyboard]] - keyboard state tracking helper
* [[Model]] - draws simple meshes loaded from .CMO, .SDKMESH, or .VBO files
     * [[ModelMesh]], [[ModelMeshPart]]
* [[Mouse]] - mouse helper
* [[PostProcess]] - set of built-in shaders for common post-processing operations
     * [[BasicPostProcess]], [[DualPostProcess]], [[ToneMapPostProcess]]
* [[PrimitiveBatch]] - simple and efficient way to draw user primitives
* [[ScreenGrab]] - light-weight screen shot saver
* [[SimpleMath]] - simplified C++ wrapper for DirectXMath
     * [[Rectangle]], [[Vector2]], [[Vector3]], [[Vector4]], [[Matrix]], [[Quaternion]], [[Plane]], [[Ray]], [[Color]], [[Viewport]]
* [[SpriteBatch]] - simple & efficient 2D sprite rendering
* [[SpriteFont]] - bitmap based text rendering
* [[VertexTypes]] - structures for commonly used vertex data formats
* [[WICTextureLoader]] - WIC-based image file texture loader
* [[XboxDDSTextureLoader]] - DDSTextureLoader variant for Xbox One exclusive app developers

# Tools
* [[MakeSpriteFont]] - builds ``.spritefont`` data files for use with SpriteFont class
* [[XWBTool]] - builds ``.xwb`` XACT-style wave banks for use with WaveBank class

# Building
This code is designed to build with Visual Studio 2017 or later. It requires the Windows 8.x / Windows 10 SDK for functionality such as the DirectXMath library and the DXGI 1.2 headers. Visual Studio 2017 or later already includes Windows 10 SDK.

These components are designed to work without requiring any content from the DirectX SDK. For details, see [Where is the DirectX SDK?](https://docs.microsoft.com/en-us/windows/desktop/directx-sdk--august-2009-), [Where is the DirectX SDK (2013 Edition)?](https://walbourn.github.io/where-is-the-directx-sdk-2013-edition/), [Where is the DirectX SDK (2015 Edition)?](https://aka.ms/dxsdk), and [The Zombie DirectX SDK](https://aka.ms/AA4gfea).

## HLSL shaders
The required Direct3D shaders are built as headers in `Src\Shader\Compiled\*.inc` and included into the DirectXTK static library. They are automatically built by the Visual Studio project if they are not present, and cleared by a Clean. They can also be manually rebuilt using the ``CompileShaders.cmd`` script (i.e. `...\DirectXTK\Src\Shaders`)

# Adding to a VS solution
## Using project-to-project references
In your application's solution, right-click on the Solution and use **Add \ Existing Project...** to add the appropriate ``.vcxproj`` file to your solution.

<table>
 <tr>
  <td>DirectXTK_Desktop_2019</td>
  <td>Windows desktop applications for Windows 7 Service Pack 1 or later building with VS 2019 Community, Professional or higher with the latest installed Windows 10 SDK.</td>
 </tr>
 <tr>
  <td>DirectXTK_Desktop_2019_Win10</td>
  <td>Windows desktop applications for Windows 10 building with VS 2019 Community, Professional or higher with the latest installed Windows 10 SDK. <I>Uses XAudio 2.9 and <code>Windows.Gaming.Input</code></I></td>
 </tr>
 <tr>
  <td>DirectXTK_Windows10_2019</td>
  <td>Universal Windows Platform (UWP) apps building with VS 2019 with the latest installed Windows 10 SDK.</td>
 </tr>
</table>

> For VS 2019, use of the latest update is recommended.

<table>
 <tr>
  <td>DirectXTK_Desktop_2017</td>
  <td>Windows desktop applications for Windows 7 Service Pack 1  or later building with VS 2017 Community, Professional or higher with the Windows 10 SDK (17763).</td>
 </tr>
 <tr>
  <td>DirectXTK_Desktop_2017_Win10</td>
  <td>Windows desktop applications for Windows 10 building with VS 2017 Community, Professional or higher with the Windows 10 SDK (17763). <I>Uses XAudio 2.9 and <code>Windows.Gaming.Input</code></I></td>
 </tr>
 <tr>
  <td>DirectXTK_Windows10_2017</td>
  <td>Universal Windows Platform (UWP) apps building with VS 2017 with the Windows 10 SDK (17763).</td>
 </tr>
 <tr>
  <td>DirectXTK_XboxOneXDK_2017</td>
  <td>Xbox One games building with VS 2017 using the Xbox One XDK (<I>June 2017 QFE 4 or later recommended</I>).</td>
 </tr>
</table>

> For VS 2017, use of the [15.9 update](https://walbourn.github.io/vs-2017-15-9-update/) is recommended.

In your application's project, right-click on the **Project** and use "References...", then "Add New Reference...", and then check the DirectXTK project name and click OK. For a Universal Windows Platform (UWP) app or Xbox One solution, you may need to set _Reference Assembly Output_ to false since DirectXTK is a static C++ library and not a WinRT component.

![Settings dialog](https://github.com/Microsoft/DirectXTK/wiki/images/settingsROA.PNG)

In your application's project settings, on the "C++ / General" page set Configuration to "All Configurations", set Platform to "All Platforms", and then add the relative path to `DirectXTK\inc;`--assuming you have the DirectXTK folder in the same directory as your ``sln`` file, it should be `$(SolutionDir)\DirectXTK\inc;`--to the _Additional Include Directories_ properties. Click Apply.

![Settings dialog](https://github.com/Microsoft/DirectXTK/wiki/images/settingsAID.PNG)

See [[Audio]] for additional information when setting up Windows desktop projects to use _DirectXTK for Audio_.

See also the [Visual C+ Team Blog](https://devblogs.microsoft.com/cppblog/flexible-project-to-project-references/)

## Using NuGet package manager
Alternatively you can use [NuGet](https://docs.microsoft.com/en-us/nuget/what-is-nuget) to install one of the DirectX Tool Kit packages. Use *Project / Manage NuGet Packages...* then select "Online" and search for "DirectXTK".

<table>
 <tr>
  <td><a href="https://www.nuget.org/packages/directxtk_desktop_2017/">directxtk_desktop_2017</a></td>
  <td>This NuGet is configured for Windows desktop C++ applications building with VS 2017 or VS 2019 Community/Professional or higher. <I>DirectXTK for Audio</I> in this package uses <a href="https://aka.ms/XAudio2Redist">XAudio2Redist</a> to support Windows 7 or later.</td>
 </tr>
 <tr>
  <td><a href="https://www.nuget.org/packages/directxtk_desktop_win10/">directxtk_desktop_win10</a></td>
  <td>This NuGet is configured for Windows desktop C++ applications building for Windows 10 with VS 2017 or VS 2019 Community/Professional or higher. <I>DirectXTK for Audio</I> in this package uses XAudio 2.9 in the Windows 10 SDK.</td>
 </tr>
 <tr>
  <td><a href="https://www.nuget.org/packages/directxtk_uwp/">directxtk_uwp</a></td>
  <td>This NuGet is configured for Universal Windows Platform apps for Windows 10 building with VS 2017 or VS 2019 Community/Professional or higher. <I>DirectXTK for Audio</I> in this package uses XAudio 2.9 in the Windows 10 SDK.</td>
 </tr>
</table>

You should use the NuGet interface to check for updates if you have an older version installed.

### Archived
These NuGet packages are no longer supported.

* [Windows desktop app using VS 2015](https://www.nuget.org/packages/directxtk_desktop_2015)
* [Windows desktop app using VS 2013](https://www.nuget.org/packages/directxtk_desktop_2013)
* [Windows 8.1 Store](https://www.nuget.org/packages/directxtk_windowsstore_8_1)
* [Windows phone 8.1](https://www.nuget.org/packages/directxtk_windowsphone_8_1)
* [Windows phone 8](https://www.nuget.org/packages/directxtk_windowsphone_8)

## Using the vcpkg C++ library manager
The _DirectX Tool Kit_ is also available through the [vcpkg](https://github.com/microsoft/vcpkg) C++ Library Manager.

```
.\vcpkg\vcpkg install directxtk
```

For the 64-bit version of the library, use:

```
.\vcpkg\vcpkg install directxtk:x64-windows
```

For the Universal Windows Platform (UWP) versions, use:

```
.\vcpkg\vcpkg install directxtk:x64-uwp
```

> ``arm``, ``arm64``, ``x86``, ``x64``, ``windows``, and ``uwp`` triplets are supported.

## Dependencies

The _DirectX Tool Kit_ library assumes your binary is linking with the following system libraries:

* ``d3d11.lib``: Provides the Direct3D device creation function ``D3D11CreateDevice``
* ``dxgi.lib``: Provides the DXGI factory creation function ``CreateDXGIFactory1`` / ``CreateDXGIFactory2``
* ``dxguid.lib``: Provides COM GUID values for ``IID_ID3D11Device``, etc.
* ``uuid.lib``: Provides COM GUID values for WIC usage such as ``CLSID_WICImagingFactory``, ``CLSID_WICImagingFactory1``, ``CLSID_WICImagingFactory2``, etc.

### DirectX Tool Kit for Audio
For "Windows 8.x" or "Windows 10" configurations of the AudioEngine class, you'll need to use ``xaudio2.lib``.

For [XAudio2Redist](https://docs.microsoft.com/en-us/windows/win32/xaudio2/xaudio2-redistributable), the [Microsoft.XAudio2.Redist](https://www.nuget.org/packages/Microsoft.XAudio2.Redist/) NuGet package provides the ``xaudio2_9redist.lib``.

### GamePad
For "Windows 10" configurations of the GamePad class that use Windows.Gaming.Input, you need ``runtimeobject.lib`` for Win32 desktop apps. For UWP apps, this is already handled with the umbrella lib ``windowsapp.lib``.

For "Windows 8.x" configurations of the GamePad class using XInput 1.4, you'll need ``xinput.lib``.

For Windows 7 configurations, use ``xinput9_1_0.lib``.
 
# Content pipeline
To use the Visual Studio graphics assets tools in the build system, be sure to [add them to your project](https://docs.microsoft.com/en-us/visualstudio/designers/using-3-d-assets-in-your-game-or-app).

**Note:** When adding ``.spritefont``, ``.sdkmesh``, or ``.xwb`` files to your Universal Windows Platform (UWP) or Xbox One project, you need to manually set the file properties to "Content: Yes" for all configurations to have these files included in your AppX package. ``.dds`` files, other image file formats, and ``.wav`` files are automatically detected as a media file and are included as content by default.

# Error handling
DirectXTK makes use of C++ exception handling which should be enabled by the application via the `/EHsc` compiler switch. In Visual Studio, this is set in the project settings under "C++ / Code Generation" with Enable C++ Exceptions set to "Yes (/EHsc)" for all configurations.

* [C++ Exception Handling](https://docs.microsoft.com/en-us/cpp/cpp/cpp-exception-handling)
* [How to: Break When an Exception is Thrown](https://docs.microsoft.com/en-us/visualstudio/debugger/managing-exceptions-with-the-debugger)
* [Dual-use Coding Techniques for Games](https://aka.ms/Fo3su4)
* [Resource Acquisition Is Initialization](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization)
* [[ThrowIfFailed]]

_Visual Studio Express editions have limited exception handling debugging support, so consider using VS Community or Pro+ instead._

# Smart-pointers and reference counting
DirectXTK encourages and makes use of a number of smart-pointers to simplify resource lifetime tracking.

* ``std::unique_ptr`` - A smart-pointer that has exactly one 'owner' of the memory
* ``std::shared_ptr`` - A smart-pointer that tracks memory use with reference counting
* ``Microsoft::WRL::ComPtr`` - A [[COM smart-pointer|ComPtr]] for reference count management very similar to ATL's CComPtr

* [Smart Pointers (Modern C++)](https://docs.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp)
* [Managing the Lifetime of an Object](https://docs.microsoft.com/en-us/windows/desktop/LearnWin32/managing-the-lifetime-of-an-object)
* [COM Coding Practices](https://docs.microsoft.com/en-us/windows/desktop/LearnWin32/com-coding-practices#smartptr)
* [Reference Counting (Direct3D 10)](https://docs.microsoft.com/en-us/windows/desktop/direct3d10/d3d10-graphics-programming-guide-api-features-reference-counting)

# Additional information
[[Implementation]]
