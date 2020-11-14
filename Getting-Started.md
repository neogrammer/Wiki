This is the _Getting Started_ tutorial for _DirectX Tool Kit_ which introduces the reader to using the DirectX Tool Kit with DirectX 11 in C++.

> If you are looking to use DirectX 12 and are already familiar with Direct3D 11, the place to start is the _DirectX Tool Kit for DirectX 12_ [tutorials](https://github.com/Microsoft/DirectXTK12/wiki/Getting-Started). Keep in mind that DirectX 12 is an expert API which builds on knowing the ins & outs of DirectX 11, and can be an unforgiving API to developers new to Direct3D or graphics in general.

# Background
This tutorial assumes the reader is familiar with the basics of C++ programming using Microsoft Visual C++, including writing code, building applications, and basic debugging. Coding conventions here will make use of C++11 language features such as ``nullptr``, ``auto``, simple _lambdas_  (aka anonymous functions), and the standard smart-pointer ``std::unique_ptr`` / ``std::make_unique<T>()``, but will generally be otherwise 'core' C++.

* [Welcome back to C++ - Modern C++](https://docs.microsoft.com/en-us/cpp/cpp/welcome-back-to-cpp-modern-cpp)
* [C++ Language and Standard Libraries](https://docs.microsoft.com/en-us/cpp/cpp/c-cpp-language-and-standard-libraries)
* [Tools for Visual C++ Development](https://docs.microsoft.com/en-us/cpp/ide/ide-and-tools-for-visual-cpp-development)
* [Smart Pointers](https://docs.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp)
* [[ComPtr]]

> If you are more comfortable with C#, then you should consider [SharpDX](http://sharpdx.org/).

This tutorial does not assume prior experience with Direct3D, but the reader should be familiar with the basic graphics concepts for DirectX or OpenGL. That said, you can get a long way using _DirectX Tool Kit_ without much in the way of graphics experience. You may also find it useful to review the _Programming Guide for Direct3D 11_ on [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/direct3d11/dx-graphics-overviews).

* [Getting Started with Direct3D 11](https://walbourn.github.io/getting-started-with-direct3d-11/)
* [Migrating to Direct3D 11](https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-programming-guide-migrating)

> Note that Direct3D 11 and _DirectX Tool Kit_ are not Windows Runtime (aka "WinRT") components, so we do not make use of the C++/CX language extensions (a.k.a. ``/ZW`` switch) or C++/WinRT language projections, although it is compatible with both of those programming models. _DirectX Tool Kit_ is a 'pure' C++ library, which is why it's not directly usable by Visual Basic, C# or HTML+JavaScript applications.

# Error handling
One thing that many C++ developers, particularly game developers, may not be all that familiar with is "C++ Exception Handling". This is distinct from "Structured Exception Handling" (SEH) which some developers have seen in the past, and can leave an unfavorable impression of C++ EH. On both ARM and x64 native platforms, C++ EH is very efficient, although the x86 32-bit implementation does have some quirks. In any case, _DirectX Tool Kit_ uses C++ Exception Handling for most error conditions, just as the Standard Template Library (STL) does and the standard behavior of the operator ``new``.

* [Errors and Exception Handling](https://docs.microsoft.com/en-us/cpp/cpp/errors-and-exception-handling-modern-cpp)
* [C++ Exception Handling](https://docs.microsoft.com/en-us/cpp/cpp/cpp-exception-handling)
* [Exception Handling (Debugging)](https://docs.microsoft.com/en-us/visualstudio/debugger/managing-exceptions-with-the-debugger)
* [[ThrowIfFailed]]

For debugging exceptions thrown by the _DirectX Tool Kit_, you should enable the checkbox next to ``std::exception`` in the Visual Studio Exception Settings dialog (_Debug_ -> _Windows_ -> _Exception Settings_). It's not on by default. Then select the "C++ Exceptions" line, click the PLUS button, and type ``DirectX::com_exception`` to add that one. If working with the [Direct3D VS Game templates](https://github.com/walbourn/directx-vs-templates) you should do this once more typing ``DX::com_exception``.

![Exception Settings](https://github.com/Microsoft/DirectXTK/wiki/images/DebugExcept.PNG)

> For some more detail on how the Visual Studio debugger deals with exceptions, read [this](https://devblogs.microsoft.com/devops/understanding-exceptions-while-debugging-with-visual-studio/) blog post.

# Audience
These tutorials are written with game development in mind as the target application since games are an excellent fit for the 'immersive DirectX app' model. Keep in mind, however, that the majority of the functionality in the _DirectX Tool Kit_ is applicable to DirectX graphics programming in general for both game and non-game applications.

# System requirements
The DirectX Tool Kit for DirectX 11 tutorials assume you are using Windows 7 Service Pack 1, Windows 8.1, or Windows 10. The *DirectX Tool Kit* supports ``D3D_FEATURE_LEVEL_9_1`` Direct3D hardware feature level or greater. Any use of 32-bit index buffers (``DXGI_FORMAT_R32_UINT``) requires at least ``D3D_FEATURE_LEVEL_9_2``. A few features such as [[DGSLEffect]], [[DebugEffect]], [[NormalMapEffect]], [[PBREffect]], [[PostProcess]], and dual-parabolic environment maps require ``D3D_FEATURE_LEVEL_10_0`` or better. 

> Older versions of the tool kit supported Windows phone 8 and Windows Vista.

# Software setup
For learning purposes, these instructions are going to focus on the following setup:

* Visual Studio 2017 ([15.9 update](https://walbourn.github.io/vs-2017-15-9-update/)) or VS 2019 Community, Professional, Premium, or Ultimate
* Windows 10 SDK ([19041](https://walbourn.github.io/windows-10-may-2020-update-sdk/) is recommended)
* Windows 7 Service Pack 1, Windows 8.1, or Windows 10 (May 2020 Update a.k.a. Version 2004 is recommended)

We will be using a Win32 desktop application project template in order to support developers using Windows 7, but all these techniques and APIs apply to Universal Windows Platform apps and Xbox as well.

> There is no need to install the legacy DirectX SDK to use DirectX Tool Kit. The DirectX SDK is deprecated and you should not use it per [Microsoft Docs](https://docs.microsoft.com/en-us/windows/desktop/directx-sdk--august-2009-). See [this blog post](http://aka.ms/dxsdk), [The Zombie DirectX SDK](https://walbourn.github.io/the-zombie-directx-sdk/), [DirectX SDK Samples Catalog](https://walbourn.github.io/directx-sdk-samples-catalog/), and [DirectX SDK Tools Catalog](https://walbourn.github.io/directx-sdk-tools-catalog/).

## Visual Studio 2017 / 2019
When using the [new lightweight installer](https://devblogs.microsoft.com/cppblog/the-lightweight-visual-studio-15-installer/) be sure to select the appropriate workloads. Here are the recommended options to select:

* Workload: _Game development with C++_
* Workload: _Desktop development with C++_
* Workload: _Universal Windows Platform development_
* Optional component: _C++ Universal Windows Platform tools_

> Note that Xbox One XDK developers also need to include the optional component _Windows 8.1 SDK and UCRT SDK_

# Samples

For a quick introduction to DirectX 11 programming, take a look at the [Introductory Graphics](https://github.com/Microsoft/Xbox-ATG-Samples#introductory-graphics) samples on [Xbox-ATG-Samples](https://github.com/Microsoft/Xbox-ATG-Samples).

* Basic drawing: *SimpleTriangle*
* Basic texturing: *SimpleTexture*
* Basic lighting: *SimpleLighting*
* DirectCompute: *SimpleCompute*
* Tessellation: *SimpleBezier*
* Instancing: *SimpleInstancing*
* Multisample Antialiasing: *SimpleMSAA*

The official samples for DirectX 11 were originally shipped in the legacy *DirectX SDK*. There is an unofficial repository of these samples available on [GitHub](https://github.com/walbourn/directx-sdk-samples/wiki) that have been updated to remove most of their legacy dependencies.

# Tutorials

* [[The basic game loop]]
* [[Using DeviceResources]]
* [[Adding the DirectX Tool Kit]]

## Graphics

* [[Sprites and textures]]
* [[More tricks with sprites]]
* [[Drawing text]]
* [[Simple rendering]]
* [[Line drawing and anti-aliasing]]
* [[3D shapes]]
* [[Rendering a model]]
* [[Using skinned models]]
* [[Using advanced shaders]]
* [[Creating custom shaders with DGSL]]
* [[Writing custom shaders]]
* [[Using HDR rendering]]

## Input

* [[Game controller input]]
* [[Mouse and keyboard input]]

## Math

* [[Using the SimpleMath library]]
* [[Basic game math]]
* [[Collision detection]]
* [[Picking]]
* [[Mixing SimpleMath and DirectXMath]]

## Audio

* [[Adding the DirectX Tool Kit for Audio]]
* [[Adding audio to your project]]
* [[Creating and playing sounds]]
* [[Making use of wave banks]]
* [[Using positional audio]]

# Resources

[Dual-use Coding Techniques for Games](https://aka.ms/Fo3su4)  
[C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md)

# Further reading

Luna, *Introduction to 3D Game Programming with DirectX 11*, Mercury Learning & Information (March 2, 2012)

Meyers, *Effective Modern C++: 42 Specific Ways to Improve Your Use of C++11 and C++14*, O'Reilly Media (November 2014)

Varcholik, *Real-Time 3D Rendering with DirectX and HLSL: A Practical Guide to Graphics Programming*, Addison-Wesley (May 31, 2014)

Zink et al, *Practical Rendering and Computation with Direct3D 11*, A K Peters/CRC Press (July 27, 2011)
