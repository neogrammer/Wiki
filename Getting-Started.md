This is the _Getting Started_ tutorial for _DirectX Tool Kit_ which introduces the reader to programming Direct3D 11 in C++.

# Background
This tutorial assumes the reader is familiar with the basics of C++ programming using Microsoft Visual C++, including writing code, building applications, and basic debugging. Coding conventions here will make use of C++11 language features such as ``auto``, simple _lambdas_  (aka anonymous functions), and the standard smart-pointer ``std::unique_ptr`` / ``std::make_unique<T>()``, but will generally be otherwise 'core' C++.

* [C++ Language and Standard Libraries](http://msdn.microsoft.com/en-us/library/hh875057.aspx)
* [Tools for Visual C++ Development](http://msdn.microsoft.com/en-us/library/hh967574.aspx)
* [Smart Pointers](http://msdn.microsoft.com/en-us/library/hh279674.aspx)
* [[ComPtr]]

This tutorial does not assume prior experience with Direct3D, but the reader should be familiar with the basic graphics concepts for DirectX or OpenGL. That said, you can get a long way using _DirectX Tool Kit_ without much in the way of graphics experience.

* [Getting Started with Direct3D 11](http://blogs.msdn.com/b/chuckw/archive/2011/07/11/getting-started-with-direct3d-11.aspx)
* [Book Recommendations](http://blogs.msdn.com/b/chuckw/archive/2014/04/07/book-recommendations.aspx)

One thing that many C++ developers, particularly game developers, may not be all that familiar with is "C++ Exception Handling". This is distinct from "Structured Exception Handling" (SEH) which some developers have seen in the past, and can leave an unfavorable impression of C++ EH. On both ARM and x64 native platforms, C++ EH is very efficient, although the x86 32-bit implementation does have some quirks. In any case, _DirectX Tool Kit_ uses C++ Exception Handling for most error conditions, just as the Standard Template Library (STL) does and the standard behavior of the operator ``new``.

* [Errors and Exception Handling](http://msdn.microsoft.com/en-us/library/hh279678.aspx)
* [C++ Exception Handling](http://msdn.microsoft.com/en-us/library/4t3saedz.aspx)
* [Exception Handling (Debugging)](http://msdn.microsoft.com/en-us/library/x85tt0dd.aspx)
* [[ThrowIfFailed]]

> Note that Direct3D 11 and _DirectX Tool Kit_ are not Windows Runtime (aka "WinRT") components, so we do not make use of the C++/CX language extensions or the ``/ZW`` switch. _DirectX Tool Kit_ is a 'pure' C++ library, which is why it's not directly usable by Visual Basic, C# or HTML+JavaScript applications.

# Audience
These tutorials are written with game development in mind as the target application since games are an excellent fit for the 'immersive DirectX app' model. Keep in mind, however, that the majority of the functionality in the _DirectX Tool Kit_ is applicable to DirectX graphics programming in general for both game and non-game applications.

# Software Setup
For learning purposes, these instructions are going to focus on the following setup:

* Visual Studio 2013 [Update 5](http://blogs.msdn.com/b/chuckw/archive/2015/07/20/visual-studio-2013-update-5.aspx) or 2015 [Update 1](http://blogs.msdn.com/b/chuckw/archive/2015/11/30/visual-studio-2015-update-1.aspx) Community, Professional, Premium, or Ultimate
* Windows 7 Service Pack 1, Windows 8.1, or Windows 10

We will be using a Win32 desktop application project template in order to support developers using Windows 7, but all these techniques and APIs apply to universal Windows apps, Windows Store apps, Windows phone 8.x, and Xbox One as well.

If using VS 2015, be sure to install Visual C++ which requires using the _Custom_ install as it's not part of the _Typical_ install. See the [Visual C++ Team Blog](http://blogs.msdn.com/b/vcblog/archive/2015/07/24/setup-changes-in-visual-studio-2015-affecting-c-developers.aspx) for details.

> There is no need to install the legacy DirectX SDK to use DirectX Tool Kit. The DirectX SDK is deprecated and you should not use it. The only time you would need to install the legacy DirectX SDK for this tutorial series is if you are using Windows 7 for the audio portions. See [MSDN](https://msdn.microsoft.com/en-us/library/windows/desktop/ee663275.aspx), [DirectX SDK Samples Catalog](http://blogs.msdn.com/b/chuckw/archive/2013/09/20/directx-sdk-samples-catalog.aspx), and [DirectX SDK Tools Catalog](http://blogs.msdn.com/b/chuckw/archive/2014/10/28/directx-sdk-tools-catalog.aspx).

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
* [[Applying lightmaps]]
* [[Creating custom shaders with DGSL]]
* [[Writing custom shaders]]

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
