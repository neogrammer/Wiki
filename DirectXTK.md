# Headers
Public headers are in the **Inc** folder of the distribution package.

# Namespace
All the functions in the library are in the **DirectX** C++ namespace.

_Note: The Xbox One exclusive application XBoxDDSTextureLoader functions are in the Xbox C++ namespace_

# Modules
* [[Audio]] - low-level audio API using XAudio2 
* CommonStates - factory providing commonly used D3D state objects 
* DDSTextureLoader - light-weight DDS file texture loader 
* DirectXHelpers - misc C++ helpers for D3D programming 
* Effects - set of built-in shaders for common rendering tasks 
* GamePad - gamepad controller helper using XInput 
* GeometricPrimitive - draws basic shapes such as cubes and spheres 
* Model - draws simple meshes loaded from .CMO, .SDKMESH, or .VBO files 
* PrimitiveBatch - simple and efficient way to draw user primitives 
* ScreenGrab - light-weight screen shot saver 
* SimpleMath - simplified C++ wrapper for DirectXMath 
* SpriteBatch - simple & efficient 2D sprite rendering 
* SpriteFont - bitmap based text rendering 
* VertexTypes - structures for commonly used vertex data formats 
* WICTextureLoader - WIC-based image file texture loader 
* XboxDDSTextureLoader - DDSTextureLoader variant for Xbox One exclusive app developers

# Tools
* MakeSpriteFont - builds .spritefont data files for use with SpriteFont class
* XWBTool - builds .xwb XACT-style wave banks for use with WaveBank class

# Building
This code is designed to build with Visual Studio 2010 or later. It requires the Windows 8.x SDK for functionality such as the DirectXMath library and the DXGI 1.2 headers. Visual Studio 2012 or later already include the Windows 8.x SDK, but Visual Studio 2010 users must install the [standalone Windows 8.1 SDK](http://go.microsoft.com/fwlink/?LinkID=323507). Details on using the Windows 8.1 SDK with VS 2010 are described on the [Visual C++ Team Blog](http://blogs.msdn.com/b/vcblog/archive/2012/11/23/using-the-windows-8-sdk-with-visual-studio-2010-configuring-multiple-projects.aspx) and the required .props files are included in the DirectX Tool Kit package.

These components are designed to work without requiring any content from the DirectX SDK. For details, see [Where is the DirectX SDK?](http://msdn.microsoft.com/en-us/library/ee663275.aspx) and [Where is the DirectX SDK (2013 Edition)?](http://blogs.msdn.com/b/chuckw/archive/2013/07/01/where-is-the-directx-sdk-2013-edition.aspx)

## HLSL shaders
The distribution package comes with a set of `Src\Shader\*.inc` files containing the compiled HLSL shaders which are included into the DirectXTK static library. They can be rebuilt if using a newer HLSL FXC.EXE or if the .fx or .hlsl files in the library are modified.

Open a Developer [Command Prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq) (installed with Visual Studio), and change to the directory containing compileShaders.cmd (i.e. `...\DirectXTK\Src\Shaders`)

Enter the following command-line after changing to the appropriate directory:

    CompileShaders

Then rebuild the DirectXTK library to update with the latest version of the shaders.

## Xbox One XDK
Xbox One exclusive application developers using the Xbox One XDK need to generate the `Src\Shaders\Compiled\XboxOne*.inc` files to build the library as they are not included in the distribution package. They should be generated with the matching FXC compiler from the Xbox One XDK. While they will continue to work if outdated, a mismatch will cause runtime compilation overhead that would otherwise be avoided.

Open a Xbox One XDK [Command Prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq), and change to the directory containing CompileShaders.cmd (i.e. `...\DirectXTK\Src\Shaders`)

Enter the following command-line after changing to the appropriate directory:

    CompileShaders xbox

Then build the Xbox One XDK version of the DirectXTK library.

# Adding to a VS solution
## Using project-to-project references
In your application's solution, right-click on the Solution and use "Add \ Existing Project..." to add the appropriate .vcxproj file to your solution.

* _DirectXTK_Windows10_ is for universal Windows apps building with VS 2015 targeting Windows 10
* _DirectXTK_Windows81_ is for Windows Store C++ apps building with VS 2013 targeting Windows 8.1 / RT 8.1 
* _DirectXTK_Windows8_ is for Windows Store C++ apps building with VS 2012 targeting Windows 8 / RT 
* _DirectXTK_WindowsPhone81_ is for Windows phone 8.1 C++ apps building with VS 2013 Update 2 or later. 
* _DirectXTK_XAMLSilverlight_WindowsPhone81_ is for Windows phone Silverlight 8.1 C++ apps building with VS 2013 Update 2 or later. 
* _DirectXTK_WindowsPhone8_ is for Windows phone 8 C++ apps building with VS 2012 or VS 2013 and the Windows Phone 8.0 SDK 
* _DirectXTK_Desktop_2015_ is for Windows desktop C++ applications building with VS 2015 
* _DirectXTK_Desktop_2013_ is for Windows desktop C++ applications building with the VS 2013 Express for Desktop, VS 2013 Community, VS 2013 Professional or higher 
* _DirectXTK_Desktop_2012_ is for Windows desktop C++ applications building with VS 2012 Express for Desktop, VS 2012 Professional or higher 
* _DirectXTK_Desktop_2010_ is for Windows desktop C++ applications building with VS 2010 using the Windows 8.1 SDK 
* _DirectXTK_XboxOneXDK_ is for Xbox One exclusive C++ applications building with VS 2012 using the Xbox One XDK 
* _DirectXTK_XboxOneADK_ is for Xbox One hub apps C++ building with VS 2012 using the Xbox One ADK

In your application's project, right-click on the Project and use "References...", then "Add New Reference...", and then check the DirectXTK project name and click OK. For a Windows Store app, Windows phone, or Xbox One solution, you need to set Reference Assembly Output to false since DirectXTK is a static C++ library and not a WinRT component.

In your application's project settings, on the "C++ / General" page set Configuration to "All Configurations", set Platform to "All Platforms", and then add the relative path to `DirectXTK\inc;`--assuming you have the DirectXTK folder in the same directory as your sln file, it should be `$(SolutionDir)\DirectXTK\inc;`--to the Additional Include Directories properties. Click Apply.

When using VS 2010 with the Windows 8.x SDK, or when using VS 2012 with the Windows 8.1 SDK, you'll need to apply the [correct .props files](http://blogs.msdn.com/b/vcblog/archive/2012/11/23/using-the-windows-8-sdk-with-visual-studio-2010-configuring-multiple-projects.aspx
) to your projects as well as use the correct DirectXTK project.

See Audio for additional information when setting up Windows desktop projects to use _DirectXTK for Audio_.

See also the [Visual C+ Team Blog](http://blogs.msdn.com/b/vcblog/archive/2010/05/03/flexible-project-to-project-references.aspx)

## Using NuGet package manager
Alternatively you can use NuGet to install one of the DirectX Tool Kit packages. Use *Project / Manage NuGet Packages...* then select "Online" and search for "DirectX Tool Kit".

* Use Id: [directxtk_desktop_2013](https://www.nuget.org/packages/directxtk_desktop_2013/) for Windows desktop C++ applications building with the VS 2013 Express for Desktop, VS 2013 Community, VS 2013 Professional or higher
* Use Id: [directxtk_windowsstore_8_1](https://www.nuget.org/packages/directxtk_windowsstore_8_1/) for Windows Store C++ apps building with VS 2013 targeting Windows 8.1 / RT 8.1
* Use Id: [directxtk_windowsphone_8_1](https://www.nuget.org/packages/directxtk_windowsphone_8_1/) for Windows phone 8.1 C++ apps or Windows phone Silverlight 8.1 C++ apps building with VS 2013 Update 2 or later.
* Use Id: [directxtk_windowsphone_8](https://www.nuget.org/packages/directxtk_windowsphone_8/) for Windows phone 8 C++ apps building with VS 2012 or VS 2013 and the Windows Phone 8.0 SDK.

You should use the NuGet interface to check for updates if you have an older version installed.

# Content pipeline
To use the Visual Studio 2012 or later graphics assets tools in the build system, be sure to [add them to your project](http://msdn.microsoft.com/en-us/library/hh972446.aspx). 

_The graphics assets tools are not present in the Express editions of Visual Studio 2012. They are present in the Visual Studio 2013 Express for Windows and Community editions, but not in VS 2013 Express for Windows Desktop._

**Note:** When adding .spritefont, .sdkmesh, or .xwb files to your Windows Store app or Windows phone app project, you need to manually set the file properties to "Content: Yes" for all configurations to have these files included in your AppX package. .dds files and other image file formats are automatically detected as a media file and are included as content by default.

# Error handling
DirectXTK makes use of C++ exception handling which should be enabled by the application via the `/EHsc` compiler switch. In Visual Studio, this is set in the project settings under "C++ / Code Generation" with Enable C++ Exceptions set to "Yes (/EHsc)" for all configurations.

* [C++ Exception Handling](http://msdn.microsoft.com/en-us/library/4t3saedz.aspx)
* [How to: Break When an Exception is Thrown](http://msdn.microsoft.com/en-us/library/d14azbfh.aspx)
* [Dual-use Coding Techniques for Games](http://blogs.msdn.com/b/chuckw/archive/2012/09/17/dual-use-coding-techniques-for-games.aspx)
* [Resource Acquisition Is Initialization](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization)

# Smart-pointers and reference counting
DirectXTK encourages and makes use of a number of smart-pointers to simplify resource lifetime tracking.
* std::unique_ptr - A smart-pointer that has exactly one 'owner' of the memory
* std::shared_ptr - A smart-pointer that tracks memory use with reference counting
* Microsoft::WRL::ComPtr - A COM smart-pointer for reference count management very similar to ATL's CComPtr

* [Smart Pointers (Modern C++)](http://msdn.microsoft.com/en-us/library/hh279674.aspx)
* [Managing the Lifetime of an Object](http://msdn.microsoft.com/en-us/library/windows/desktop/ff485846.aspx)
* [COM Coding Practices](http://msdn.microsoft.com/en-us/library/windows/desktop/ff485839.aspx#smartptr)
* [Reference Counting (Direct3D 10)](http://msdn.microsoft.com/en-us/library/windows/desktop/bb205070.aspx)

# Implementation notes
DirectXTK's implementation makes extensive use of the [pImpl idiom](http://en.wikipedia.org/wiki/Opaque_pointer). This keeps the public headers slim and minimizes inter-module dependencies.

    // SpriteBatch.h public header
    class SpriteBatch
    {
    public:
        ...
    
    private:
        // Private implementation.
        class Impl;
    
        std::unique_ptr<Impl> pImpl;
    
        // Prevent copying.
        SpriteBatch(SpriteBatch const&);
        SpriteBatch& operator= (SpriteBatch const&);
    };

This also allows use to allocate the pImpl class internally using _aligned_malloc(x,16); so that we can use the DIrectXMath aligned XMVECTOR and XMMATRIX types directly in the implementation.
