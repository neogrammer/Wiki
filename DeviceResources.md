In [[the basic game loop]] lesson and other tutorials, we are making use of the **D3D11Game** VS template which has all the relevant in the ``Game`` class. This makes it easy to teach with, and for the reader to see all the code in one place. This, however, does result in a lot of 'boiler-plate' code in the main ``Game`` class which could be distracting in larger projects & samples.

There is therefore a "DR" variant of each of the D3DGame VS templates in the [VS 2013](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DWin32Game.vsix) / [VS 2015](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DUWPGame.vsix) VSIX package.

# Creating a new project
* From the drop-down menu, select **File** and then **New** -> **Project...**
* Select "Visual C++" on the left-hand tree-view.
* Select "Direct3D Win32 Game DR".
* Fill in the "Name" and "Location" fields as desired.
* Optional: Uncheck "Create directory for solution" to keep the directory structure as bit flatter.
* Select "OK".

![Create New Project](https://github.com/Microsoft/DirectXTK/wiki/images/CreateNewProjectDR.png)

![Open Game Cpp](https://github.com/Microsoft/DirectXTK/wiki/images/OpenGameCppDR.png)

* Precompiled header files
 * pch.h 
 * pch.cpp
* Main application entry-point and classic Windows procedure function
 * Main.cpp
* Device resources abstraction
 * DeviceResources.h
 * DeviceResources.cpp
* Timer helper class
 * StepTimer.h
* Resources
 * directx.ico
 * resource.rc
 * settings.manifest
* The Game class
 * Game.h
 * Game.cpp


# Further reading

[Direct3D Win32 Game Visual Studio template (Redux)](http://blogs.msdn.com/b/chuckw/archive/2015/12/17/direct3d-game-visual-studio-templates-redux.aspx)  
