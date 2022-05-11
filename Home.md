http://go.microsoft.com/fwlink/?LinkId=248929

[![GitHub All Releases](https://img.shields.io/github/downloads/Microsoft/DirectXTK/total?style=for-the-badge) ![GitHub Release Date](https://img.shields.io/github/release-date/Microsoft/DirectXTK?style=for-the-badge)](https://github.com/microsoft/DirectXTK/releases/latest)

[![Nuget UWP](https://img.shields.io/nuget/dt/directxtk_uwp?label=NuGet%20%28UWP%29&style=for-the-badge)](https://www.nuget.org/packages/directxtk_uwp/) [![Nuget Win32](https://img.shields.io/nuget/dt/directxtk_desktop_2019?label=NuGet%20%28Win32%29&style=for-the-badge)](https://www.nuget.org/packages/directxtk_desktop_2019/) [![Nuget Win10](https://img.shields.io/nuget/dt/directxtk_desktop_win10?label=NuGet%20%28Win10%29&style=for-the-badge)](https://www.nuget.org/packages/directxtk_desktop_win10/) [![Nuget VS 2017](https://img.shields.io/nuget/dt/directxtk_desktop_2017?label=NuGet%20%28VS%202017%29&style=for-the-badge)](https://www.nuget.org/packages/directxtk_desktop_2017/)

![GitHub last commit](https://img.shields.io/github/last-commit/Microsoft/DirectXTK?style=for-the-badge) ![Lines of code](https://img.shields.io/tokei/lines/github/Microsoft/DirectXTK?style=for-the-badge) ![Code size](https://img.shields.io/github/languages/code-size/Microsoft/DirectXTK?style=for-the-badge)

![Azure DevOps VS 2019](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1144?label=BUILD%20%28VS%202019%29&style=for-the-badge)
![Azure DevOps VS 2022](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1848?label=BUILD%20%28VS%202022%29&style=for-the-badge)
![Azure DevOps GDK](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1993?label=BUILD%20%28GDK%29&style=for-the-badge)

![Azure DevOps CMake](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1664?label=BUILD%20%28CMake%29&style=for-the-badge)
![Azure DevOps MinGW](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/2015?label=BUILD%20%28MinGW%29&style=for-the-badge)

![Azure DevOps test suite build](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1155?label=BUILD%20%28TEST%20SUITE%29&style=for-the-badge)
![Azure DevOps test suite build VS 2019](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/2011?label=BUILD%20%28TEST%20SUITE%20VS%202022%29&style=for-the-badge)

![DirectX Logo](https://github.com/Microsoft/DirectXTK/wiki/X_jpg.jpg)The **DirectX Tool Kit** (aka **DirectXTK**) is a collection of helper classes for writing **DirectX 11.x** code in C++.

For an overview of the project, see these blog posts: [March 2012](https://walbourn.github.io/directxtk/), [January 2013](https://walbourn.github.io/directxtk-update/), [September 2014](https://walbourn.github.io/directx-tool-kit-now-with-gamepads/), [August 2015](https://walbourn.github.io/directx-tool-kit-keyboard-and-mouse-support/), [July 2016](https://walbourn.github.io/directx-tool-kit-for-directx-12/), [September 2021](https://walbourn.github.io/latest-news-on-directx-tool-kit/), and [October 2021](https://walbourn.github.io/directx-tool-kit-vertex-skinning-update/).

For an overview of *DirectX Tool Kit for Audio*, see these blog posts: [December 2013](https://walbourn.github.io/directx-tool-kit-for-audio/) and [May 2020](https://walbourn.github.io/directx-tool-kit-for-audio-updates-and-a-direct3d-9-footnote/).

[[Getting Started]]

[[DirectXTK]] library

[[Samples]]

[[Game Gallery]]

[[Resources]]

[[Utilities]]

# DirectX 11
DirectX 11 is supported on Windows  7, Windows 8.1, Windows 10, and Windows 11. Use of DirectX 11 for beginners is highly recommended.

# DirectX 12
A version of _DirectX Tool Kit for DirectX 12_ is available on [GitHub](https://github.com/Microsoft/DirectXTK12). DirectX 12 is an expert API for Windows 10 / Windows 11 which builds on knowing the ins & outs of DirectX 11. Both DirectX 11 and DirectX 12 provide access to the same graphics hardware features, but drive the hardware in different ways which can allow a well-optimized DirectX 12 engine to achieve much lower CPU overhead than in DirectX 11.

> Note that advanced hardware features like Shader Model 6, DirectX Raytracing, Amplification & Mesh Shaders, and other "DirectX Ultimate" features do in fact require DirectX 12 as they are not exposed in DirectX 11. The statement above is still 100% true for the core graphics functionality used by *DirectX Tool Kit*.

# NuGet

DirectX Tool Kit is also available from the NuGet Gallery:
- [Windows desktop app using VS 2019 or VS 2022](https://www.nuget.org/packages/directxtk_desktop_2019/)
- [Windows desktop app using VS 2019 or VS 2022 for Windows 10](https://www.nuget.org/packages/directxtk_desktop_win10/)
- [Universal Windows Platform apps using VS 2019 or VS 2022](https://www.nuget.org/packages/directxtk_uwp/)

# vcpkg C++ Package Manager

DirectX Tool Kit has a vcpkg installation [port](https://github.com/microsoft/vcpkg/tree/master/ports/directxtk) as well.

# Porting from D3DX
See [this post](https://aka.ms/Kfsdiu) for a complete listing of D3DX equivalents.

# Support

For questions, consider using [Stack Overflow](https://stackoverflow.com/questions/tagged/directxtk) with the *directxtk* tag, or the [DirectX Discord Server](https://discord.gg/directx) in the *dx9-dx11-developers* channel.

For bug reports and feature requests, please use GitHub [issues](https://github.com/microsoft/DirectXTK/issues) for this project.

# Contributing

This project welcomes contributions and suggestions. Most contributions require you to agree to a Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

# Trademarks

This project may contain trademarks or logos for projects, products, or services. Authorized use of Microsoft trademarks or logos is subject to and must follow [Microsoft's Trademark & Brand Guidelines](https://www.microsoft.com/en-us/legal/intellectualproperty/trademarks/usage/general). Use of Microsoft trademarks or logos in modified versions of this project must not cause confusion or imply Microsoft sponsorship. Any use of third-party trademarks or logos are subject to those third-party's policies.

# Credits

The _DirectX Tool Kit_ is the work of Shawn Hargreaves and Chuck Walbourn, with contributions from Aaron Rodriguez Hernandez, and Dani Roman.

Thanks to Shanon Drone for the SDKMESH file format.

Thanks to Adrian Tsai for the geodesic sphere implementation.

Thanks to Garrett Serack for his help in creating the NuGet packages for DirectX Tool Kit.

Thanks to Roberto Sonnino for his help with the ``CMO``, DGSL rendering, and the VS Starter Kit animation.

Thanks to Pete Lewis and Justin Saunders for the normal-mapped and PBR shaders implementation.
