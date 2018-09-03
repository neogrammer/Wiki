This is the _Getting Started_ tutorial for _DirectX Tool Kit_ which introduces the reader to programming Direct3D 11 in C++.

> If you are looking to use DirectX 12 and are already familiar with Direct3D 11, the place to start is the _DirectX Tool Kit for DirectX 12_ [tutorials](https://github.com/Microsoft/DirectXTK12/wiki/Getting-Started). Keep in mind that DirectX 12 is an expert API which builds on knowing the ins & outs of DirectX 11, and is an unforgiving API to developers new to Direct3D or graphics in general.
15
# Background
This tutorial assumes the reader is familiar with the basics of C++ programming using Microsoft Visual C++, including writing code, building applications, and basic debugging. Coding conventions here will make use of C++11 language features such as ``nullptr``, ``auto``, simple _lambdas_  (aka anonymous functions), and the standard smart-pointer ``std::unique_ptr`` / ``std::make_unique<T>()``, but will generally be otherwise 'core' C++.

* [C++ Language and Standard Libraries](https://docs.microsoft.com/en-us/cpp/cpp/c-cpp-language-and-standard-libraries)
* [Tools for Visual C++ Development](https://docs.microsoft.com/en-us/cpp/ide/ide-and-tools-for-visual-cpp-development)
* [Smart Pointers](https://docs.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp)
* [[ComPtr]]

This tutorial does not assume prior experience with Direct3D, but the reader should be familiar with the basic graphics concepts for DirectX or OpenGL. That said, you can get a long way using _DirectX Tool Kit_ without much in the way of graphics experience.

* [Getting Started with Direct3D 11](http://blogs.msdn.com/b/chuckw/archive/2011/07/11/getting-started-with-direct3d-11.aspx)
* [Book Recommendations](http://blogs.msdn.com/b/chuckw/archive/2014/04/07/book-recommendations.aspx)

> Note that Direct3D 11 and _DirectX Tool Kit_ are not Windows Runtime (aka "WinRT") components, so we do not make use of the C++/CX language extensions or the ``/ZW`` switch. _DirectX Tool Kit_ is a 'pure' C++ library, which is why it's not directly usable by Visual Basic, C# or HTML+JavaScript applications.

# Error handling
One thing that many C++ developers, particularly game developers, may not be all that familiar with is "C++ Exception Handling". This is distinct from "Structured Exception Handling" (SEH) which some developers have seen in the past, and can leave an unfavorable impression of C++ EH. On both ARM and x64 native platforms, C++ EH is very efficient, although the x86 32-bit implementation does have some quirks. In any case, _DirectX Tool Kit_ uses C++ Exception Handling for most error conditions, just as the Standard Template Library (STL) does and the standard behavior of the operator ``new``.

* [Errors and Exception Handling](https://docs.microsoft.com/en-us/cpp/cpp/errors-and-exception-handling-modern-cpp)
* [C++ Exception Handling](https://docs.microsoft.com/en-us/cpp/cpp/cpp-exception-handling)
* [Exception Handling (Debugging)](https://docs.microsoft.com/en-us/visualstudio/debugger/managing-exceptions-with-the-debugger)
* [[ThrowIfFailed]]

For debugging exceptions thrown by the _DirectX Tool Kit_, you should enable the checkbox next to ``std::exception`` in the Visual Studio Exception Settings dialog (_Debug_ -> _Windows_ -> _Exception Settings_). It's not on by default. Then select the "C++ Exceptions" line, click the PLUS button, and type ``DirectX::com_exception`` to add that one. If working with the [Direct3D VS Game templates](https://github.com/walbourn/directx-vs-templates) you should do this once more typing ``DX::com_exception``.

![Exception Settings](https://github.com/Microsoft/DirectXTK/wiki/images/DebugExcept.PNG)

> For some more detail on how the Visual Studio debugger deals with exceptions, read [this](https://blogs.msdn.microsoft.com/visualstudioalm/2015/01/07/understanding-exceptions-while-debugging-with-visual-studio/) blog post.

> In Visual Studio 2015/2017, the exception debugging options changed compared to VS 2013; see [this](https://blogs.msdn.microsoft.com/visualstudioalm/2015/02/23/the-new-exception-settings-window-in-visual-studio-2015/) blog post.

# Audience
These tutorials are written with game development in mind as the target application since games are an excellent fit for the 'immersive DirectX app' model. Keep in mind, however, that the majority of the functionality in the _DirectX Tool Kit_ is applicable to DirectX graphics programming in general for both game and non-game applications.

# Software Setup
For learning purposes, these instructions are going to focus on the following setup:

* Visual Studio 2015 [Update 3](https://blogs.msdn.microsoft.com/chuckw/2016/06/27/visual-studio-2015-update-3/) or Visual Studio 2017 ([15.7 update](https://blogs.msdn.microsoft.com/chuckw/2018/05/07/vs-2017-15-7-update/) or [15.8 update](https://blogs.msdn.microsoft.com/chuckw/2018/08/16/vs-2017-15-8-update/)) Community, Professional, Premium, or Ultimate
* Windows 7 Service Pack 1, Windows 8.1, or Windows 10

We will be using a Win32 desktop application project template in order to support developers using Windows 7, but all these techniques and APIs apply to Universal Windows Platform apps, Windows Store apps, Windows phone 8.1, and Xbox One as well.

> There is no need to install the legacy DirectX SDK to use DirectX Tool Kit. The DirectX SDK is deprecated and you should not use it. The only time you would need to install the legacy DirectX SDK for this tutorial series is if you are using Windows 7 for the audio portions. See [MSDN](https://msdn.microsoft.com/en-us/library/windows/desktop/ee663275.aspx), [DirectX SDK Samples Catalog](http://blogs.msdn.com/b/chuckw/archive/2013/09/20/directx-sdk-samples-catalog.aspx), and [DirectX SDK Tools Catalog](http://blogs.msdn.com/b/chuckw/archive/2014/10/28/directx-sdk-tools-catalog.aspx).

## Visual Studio 2015
Be sure to install Visual C++ which requires using the _Custom_ install as it's not part of the _Typical_ install. See the [Visual C++ Team Blog](http://blogs.msdn.com/b/vcblog/archive/2015/07/24/setup-changes-in-visual-studio-2015-affecting-c-developers.aspx) for details.

## Visual Studio 2017
When using the [new lightweight installer](https://blogs.msdn.microsoft.com/vcblog/2016/08/22/the-lightweight-visual-studio-15-installer/) be sure to select the appropriate workloads. Here are the recommended options to select:

* Workload: _Game development with C++_
* Workload: _Desktop development with C++_
* Workload: _Universal Windows Platform development_
* Optional component: _C++ Universal Windows Platform tools_

> Note that Xbox One XDK developers also need to include the optional component _Windows 8.1 SDK and UCRT SDK_

# Samples

For a quick introduction to DirectX 11 programming, take a look at the [Introductory Graphics](https://github.com/Microsoft/Xbox-ATG-Samples#introductory-graphics) samples on [Xbox-ATG-Samples](https://github.com/Microsoft/Xbox-ATG-Samples).

* Basic drawing: SimpleTriangle
* Basic texturing: SimpleTexture
* Basic lighting: SimpleLighting
* DirectCompute: SimpleCompute
* Tessellation: SimpleBezier
* Instancing: SimpleInstancing
* Multisample Antialiasing: SimpleMSAA

# Tutorials

* [[The basic game loop]]
* [[Adding the DirectX Tool Kit]]

## Graphics

* [[Sprites and textures]]
* [[More tricks with sprites]]
* [[Drawing text]]
* [[Simple rendering]]
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
* [[Using positional audio]]

# Resources

[Dual-use Coding Techniques for Games](http://blogs.msdn.com/b/chuckw/archive/2012/09/17/dual-use-coding-techniques-for-games.aspx)  
[C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md)
