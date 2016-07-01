http://go.microsoft.com/fwlink/?LinkId=248929

![DirectX Logo](https://github.com/Microsoft/DirectXTK/wiki/X_jpg.jpg)The **DirectX Tool Kit** (aka **DirectXTK**) is a collection of helper classes for writing **DirectX 11.x** code in C++.

See this [blog post](http://blogs.msdn.com/b/chuckw/archive/2012/03/02/directxtk.aspx).

[[Getting Started]]

[[DirectXTK]] library

[[Samples]]

[[Game Gallery]]

[[Resources]]

# DirectX 11
_DirectX Tool Kit for DirectX 11_ was formerly hosted on [CodePlex](https://directxtk.codeplex.com/). DirectX 11 is supported on Windows Vista SP2, Windows 7, Windows 8.x, and Windows 10. Use of DirectX 11 for beginners, indies, small projects, and small teams is highly recommended.

# DirectX 12
A version of _DirectX Tool Kit for DirectX 12_ is available on [GitHub](https://github.com/Microsoft/DirectXTK12). DirectX 12 is an expert API for Windows 10 which builds on knowing the ins & outs of DirectX 11. DirectX 12 is an extremely low-level API designed for graphic experts who have a solid understanding of the architecture of modern GPU hardware, and can essentially write the DirectX 11 Runtime from scratch. Both DirectX 11 and DirectX 12 provide access to the same hardware features on Windows 10, but drive the hardware in different ways which can allow a well-optimized DirectX 12 engine to achieve much lower CPU overhead than in DirectX 11.

# NuGet
DirectXTK is also available on NuGet
* [Universal Windows Platform apps](https://www.nuget.org/packages/directxtk_uwp)
* [Windows desktop app using VS 2013](https://www.nuget.org/packages/directxtk_desktop_2013)
* [Windows desktop app using VS 2015](https://www.nuget.org/packages/directxtk_desktop_2015)
* [Windows 8.1 Store](https://www.nuget.org/packages/directxtk_windowsstore_8_1)
* [Windows phone 8.1](https://www.nuget.org/packages/directxtk_windowsphone_8_1)

# Porting from D3DX
See [this post](http://blogs.msdn.com/b/chuckw/archive/2013/08/21/living-without-d3dx.aspx) for a complete listing of D3DX equivalents.

# Credits

The DirectX Tool Kit is the work of Shawn Hargreaves and Chuck Walbourn, with contributions from Aaron Rodriguez Hernandez, and Daniel Roman.

Thanks to Adrian Tsai for the geodesic sphere implementation.

Thanks to Garrett Serack for his help in creating the NuGet packages for DirectX Tool Kit.

Thanks to Roberto Sonnino for his help with the CMO and DGSL rendering.
