http://go.microsoft.com/fwlink/?LinkId=248929

[![GitHub All Releases](https://img.shields.io/github/downloads/Microsoft/DirectXTK/total?style=for-the-badge) ![GitHub Release Date](https://img.shields.io/github/release-date/Microsoft/DirectXTK?style=for-the-badge)](https://github.com/microsoft/DirectXTK/releases/latest) [![Nuget](https://img.shields.io/nuget/dt/directxtk_uwp?label=NuGet%20%28UWP%29&style=for-the-badge)](https://www.nuget.org/packages/directxtk_uwp/) [![Nuget](https://img.shields.io/nuget/dt/directxtk_desktop_2015?label=NuGet%20%28Win32%29&style=for-the-badge)](https://www.nuget.org/packages/directxtk_desktop_2015/)

![GitHub last commit](https://img.shields.io/github/last-commit/Microsoft/DirectXTK?style=for-the-badge)
![Azure DevOps build](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1144?style=for-the-badge)
![Azure DevOps test suite build](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1155?label=BUILD%20%28TEST%20SUITE%29&style=for-the-badge)

![DirectX Logo](https://github.com/Microsoft/DirectXTK/wiki/X_jpg.jpg)The **DirectX Tool Kit** (aka **DirectXTK**) is a collection of helper classes for writing **DirectX 11.x** code in C++.

See this [blog post](https://walbourn.github.io/directxtk/).

[[Getting Started]]

[[DirectXTK]] library

[[Samples]]

[[Game Gallery]]

[[Resources]]

# DirectX 11
_DirectX Tool Kit for DirectX 11_ was formerly hosted on [CodePlex](https://directxtk.codeplex.com/). DirectX 11 is supported on Windows  7, Windows 8.x, and Windows 10. Use of DirectX 11 for beginners, indies, small projects, and small teams is highly recommended.

# DirectX 12
A version of _DirectX Tool Kit for DirectX 12_ is available on [GitHub](https://github.com/Microsoft/DirectXTK12). DirectX 12 is an expert API for Windows 10 which builds on knowing the ins & outs of DirectX 11. DirectX 12 is an extremely low-level API designed for graphic experts who have a solid understanding of the architecture of modern GPU hardware, and can essentially write the DirectX 11 Runtime from scratch. Both DirectX 11 and DirectX 12 provide access to the same hardware features on Windows 10, but drive the hardware in different ways which can allow a well-optimized DirectX 12 engine to achieve much lower CPU overhead than in DirectX 11.

# NuGet
DirectXTK is also available on NuGet
* [Universal Windows Platform apps](https://www.nuget.org/packages/directxtk_uwp)
* [Windows desktop app using VS 2015, VS 2017, or VS 2019](https://www.nuget.org/packages/directxtk_desktop_2015)

## Archived
* [Windows desktop app using VS 2013](https://www.nuget.org/packages/directxtk_desktop_2013)
* [Windows 8.1 Store](https://www.nuget.org/packages/directxtk_windowsstore_8_1)
* [Windows phone 8.1](https://www.nuget.org/packages/directxtk_windowsphone_8_1)

# Porting from D3DX
See [this post](https://aka.ms/Kfsdiu) for a complete listing of D3DX equivalents.

# Credits

The _DirectX Tool Kit_ is the work of Shawn Hargreaves and Chuck Walbourn, with contributions from Aaron Rodriguez Hernandez, and Daniel Roman.

Thanks to Adrian Tsai for the geodesic sphere implementation.

Thanks to Garrett Serack for his help in creating the NuGet packages for DirectX Tool Kit.

Thanks to Roberto Sonnino for his help with the ``CMO`` and DGSL rendering.

Thanks to Pete Lewis and Justin Saunders for the normal-mapped and PBR shaders implementation.
