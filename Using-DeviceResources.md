In [[the basic game loop]] lesson, we made use of the **Direct3D Game** VS template which has all the relevant Direct3D code in the ``Game`` class including creating the device and swap chain. This makes it easy to teach with, and for the reader to see all the code in one place. This, however, does result in a lot of 'boiler-plate' code in the main ``Game`` class which could be distracting.

There is therefore a "DR" variant of each of the Direct3D Game VS template in the [VS 2015/2017/2019](https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DUWPGame.vsix) VSIX package which adds [[DeviceResources]].

> The standard Universal Windows Platform app ``DirectXApp`` VS templates make use of a similar ``DeviceResources`` abstraction.

# Creating a new project

# Running the application

# Tour of the code

# Tutorial series
**Next lesson**: [[Adding the DirectX Tool Kit]]

# Further reading
[Direct3D Win32 Game Visual Studio template](https://walbourn.github.io/direct3d-win32-game-visual-studio-template/)  
[Direct3D Game Visual Studio templates (Redux)](https://walbourn.github.io/direct3d-game-visual-studio-templates-redux/)  
[Manifest Madness](https://aka.ms/I6kdnw)  
[64-bit programming for Game Developers](https://docs.microsoft.com/en-us/windows/desktop/DxTechArts/sixty-four-bit-programming-for-game-developers)