After creating a new project in the [[The basic game loop]], the next step is to add the _DirectX Tool Kit_ to the project so we can make use of it in our code.

# NuGet package manager
The easiest way to achieve this is to use the [NuGet package manager](https://www.nuget.org/) built into Visual Studio.

* From the drop-down menu, select **Project** / **Manage NuGet Packages...**
* *VS2013:* Select "Online/All" in the left-hand tree view
* *VS2015:* Select "Browse" on the top tab, and make sure the _Package source_ is set to "nuget.org"
* In the text search field type "DirectXTK" and hit enter to search for the packages
* Select the package with the id **[directxtk_desktop_2013](https://www.nuget.org/packages/directxtk_desktop_2013/)** or **[directxtk_desktop_2015](https://www.nuget.org/packages/directxtk_desktop_2015/)**
* Select "Install"
* When finished, close the NuGet Manager

![Manage NuGet Packages (VS 2013)](https://github.com/Microsoft/DirectXTK/wiki/images/nuget2013.png)

![Manage NuGet Packages (VS 2015)](https://github.com/Microsoft/DirectXTK/wiki/images/nuget2015.png)

You can check for updates to the NuGet package by selecting "Updates/All" in the left-hand tree view. If there are updates, select the "DirectX Tool Kit" package, and click on "Update".

> If you are using the Direct3DUWPGame template, you would use package id **[directxtk_uwp](https://www.nuget.org/packages/directxtk_uwp)**. Note both **directxtk_desktop_2015** and **directxtk_uwp** support VS 2017 as well.

# Project-to-project references
Another option rather than using NuGet is to use Visual Studio's [project-to-project references](http://blogs.msdn.com/b/vcblog/archive/2010/05/03/flexible-project-to-project-references.aspx). This approach is a little more setup, but it does ensure that you are building the full DirectX Tool Kit library as part of your solution, and allows you to make changes directly to the tool kit if desired as well.

* Extract the [release .zip file](https://github.com/Microsoft/DirectXTK/releases) into a directory relative to the new project you created. For this tutorial, we will assume the ``DirectXTK`` folder is in the same folder as your new project's Visual Studio Solution (``.sln``) file.
* Right-click on your solution in the Solution Explorer, and select **Add** / **Existing Project...**
* Browse into the "DirectXTK" folder and select ``DirectXTK_Desktop_2013.vcxproj`` or ``DirectXTK_Desktop_2015.vcxproj``, click "Open"
* If Visual Studio presents a "Security Warning", select "OK". Optional: Uncheck "Ask me for every project in this solution" first.
* Right-click on your project in the Solution Explorer, and select **Add** / **References...**
* Select "Add New Reference..."
* Check ``DirectXTK_Desktop_2013`` or ``DirectXTK_Desktop_2015`` and select "OK"
* Click on "Configuration Properties" in the left-hand tree view, then **C/C++** / **General**
* Select "All Configurations" and "All Platforms"
* Edit _Additional Include Directories_ to the relative path to the ``DirectXTK\Inc`` folder. With our original assumption, you can use ``$(SolutionDir)\DirectXTK\Inc``.
* Select "OK"

![Add Reference](https://github.com/Microsoft/DirectXTK/wiki/images/AddReference.png)

![Additional Includes](https://github.com/Microsoft/DirectXTK/wiki/images/AdditionalIncludes.png)

For more information see [[DirectXTK]] under _Adding to a VS solution_.

> If you are using the Direct3DUWPGame template, you would use ``DirectXTK_Windows10.vcxproj``.

# Adding the headers
Now that we have the _DirectX Tool Kit_ usable in your project, the next step is to include the library headers into your project.

General advice for C++ projects is that you should only add the headers you actually use to your project, but to simplify the tutorial we will go ahead and add them all to your new project's **pch.h** header:

    //
    // pch.h
    // Header for standard system include files.
    //

    #pragma once

    ...

    #include "CommonStates.h"
    #include "DDSTextureLoader.h"
    #include "DirectXHelpers.h"
    #include "Effects.h"
    #include "GamePad.h"
    #include "GeometricPrimitive.h"
    #include "GraphicsMemory.h"
    #include "Keyboard.h"
    #include "Model.h"
    #include "Mouse.h"
    #include "PostProcess.h"
    #include "PrimitiveBatch.h"
    #include "ScreenGrab.h"
    #include "SimpleMath.h"
    #include "SpriteBatch.h"
    #include "SpriteFont.h"
    #include "VertexTypes.h"
    #include "WICTextureLoader.h"

> This does not include the _DirectX Tool Kit for Audio_ header ``Audio.h`` which is covered by another [[tutorial|Adding the DirectX Tool Kit for Audio]].

Then finish off the setup by adding a C++ namespace using statement to your **Game.cpp** file to make it a bit easier to use the SimpleMath types:

    //
    // Game.cpp -
    //

    #include "pch.h"
    #include "Game.h"

    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    using Microsoft::WRL::ComPtr;

    ...

# Platform notes

If you are using the DirectX Tool Kit with the Xbox One XDK, then you have two additional steps:

1. You need to build the Xbox One shaders that match your XDK release via ``CompileShaders.cmd xbox``

1. You need to add creation of the [[GraphicsMemory]] singleton to your application and call ``m_graphicsMemory->Commit();`` after calling ``Present();`` / ``m_deviceResources->Present();``

**Next lessons:** [[Sprites and textures]], [[Game controller input]], [[Using the SimpleMath library]], [[Adding the DirectX Tool Kit for Audio]]
