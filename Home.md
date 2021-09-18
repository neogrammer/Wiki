http://go.microsoft.com/fwlink/?LinkId=248929

[![GitHub All Releases](https://img.shields.io/github/downloads/Microsoft/DirectXTK/total?style=for-the-badge) ![GitHub Release Date](https://img.shields.io/github/release-date/Microsoft/DirectXTK?style=for-the-badge)](https://github.com/microsoft/DirectXTK/releases/latest)

[![Nuget UWP](https://img.shields.io/nuget/dt/directxtk_uwp?label=NuGet%20%28UWP%29&style=for-the-badge)](https://www.nuget.org/packages/directxtk_uwp/) [![Nuget Win32](https://img.shields.io/nuget/dt/directxtk_desktop_2017?label=NuGet%20%28Win32%29&style=for-the-badge)](https://www.nuget.org/packages/directxtk_desktop_2017/) [![Nuget Win10](https://img.shields.io/nuget/dt/directxtk_desktop_win10?label=NuGet%20%28Win10%29&style=for-the-badge)](https://www.nuget.org/packages/directxtk_desktop_win10/)

![GitHub last commit](https://img.shields.io/github/last-commit/Microsoft/DirectXTK?style=for-the-badge)

![Azure DevOps Win32](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1144?label=BUILD%20%28Win32%29&style=for-the-badge)
![Azure DevOps CMake](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1664?label=BUILD%20%28CMake%29&style=for-the-badge)
![Azure DevOps test suite build](https://img.shields.io/azure-devops/build/mscodehub/ab27a052-7f0e-4cba-9bec-d298c5942ab9/1155?label=BUILD%20%28TEST%20SUITE%29&style=for-the-badge)

![DirectX Logo](https://github.com/Microsoft/DirectXTK/wiki/X_jpg.jpg)The **DirectX Tool Kit** (aka **DirectXTK**) is a collection of helper classes for writing **DirectX 11.x** code in C++.

See this [blog post](https://walbourn.github.io/directxtk/), [this one](https://walbourn.github.io/directxtk-update/), and [this update](https://walbourn.github.io/latest-news-on-directx-tool-kit/) for the *DirectX Tool Kit*.

See [this post](https://walbourn.github.io/directx-tool-kit-for-audio/) and [this update](https://walbourn.github.io/directx-tool-kit-for-audio-updates-and-a-direct3d-9-footnote/) for *DirectX Tool Kit for Audio*.

[[Getting Started]]

[[DirectXTK]] library

[[Samples]]

[[Game Gallery]]

[[Resources]]

[[Utilities]]

# DirectX 11
_DirectX Tool Kit for DirectX 11_ was formerly hosted on [CodePlex](https://directxtk.codeplex.com/). DirectX 11 is supported on Windows  7, Windows 8.x, and Windows 10. Use of DirectX 11 for beginners, indies, small projects, and small teams is highly recommended.

# DirectX 12
A version of _DirectX Tool Kit for DirectX 12_ is available on [GitHub](https://github.com/Microsoft/DirectXTK12). DirectX 12 is an expert API for Windows 10 which builds on knowing the ins & outs of DirectX 11. DirectX 12 is an extremely low-level API designed for graphic experts who have a solid understanding of the architecture of modern GPU hardware, and can essentially write the DirectX 11 Runtime from scratch. Both DirectX 11 and DirectX 12 provide access to the same hardware features on Windows 10, but drive the hardware in different ways which can allow a well-optimized DirectX 12 engine to achieve much lower CPU overhead than in DirectX 11.

# Porting from D3DX
See [this post](https://aka.ms/Kfsdiu) for a complete listing of D3DX equivalents.

# Support

For questions, consider using [Stack Overflow](https://stackoverflow.com/questions/tagged/directxtk)  with the *directxtk* tag, or the [DirectX Discord Server](https://discord.gg/directx) in the *dx9-dx11-developers* channel.

For bug reports and feature requests, please use GitHub [issues](https://github.com/microsoft/DirectXTK/issues) in this project.

# Credits

The _DirectX Tool Kit_ is the work of Shawn Hargreaves and Chuck Walbourn, with contributions from Aaron Rodriguez Hernandez, and Dani Roman.

Thanks to Adrian Tsai for the geodesic sphere implementation.

Thanks to Garrett Serack for his help in creating the NuGet packages for DirectX Tool Kit.

Thanks to Roberto Sonnino for his help with the ``CMO`` and DGSL rendering.

Thanks to Pete Lewis and Justin Saunders for the normal-mapped and PBR shaders implementation.
