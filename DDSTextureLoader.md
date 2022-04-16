|[[DirectXTK]]|
|---|

This is a simple light-weight DirectDraw Surface (``.dds``) file loader. This is the traditional texture file container for DirectX. This loader performs no pixel data conversions (see _Remarks_ for more details). This is ideal for runtime usage, and supports the full complement of Direct3D 11 texture resources (1D, 2D, volume maps, cubemaps, mipmap levels, texture arrays, cubemap arrays, Block Compressed formats, etc.). It supports both legacy ``DDPIXELFORMAT`` and 'DX10' ``DXGI_FORMAT`` extension header format ``.dds`` files.

> A standalone version is included in [DirectXTex](https://github.com/microsoft/DirectXTex/tree/main/DDSTextureLoader) for Direct3D 9, Direct3D 11, and Direct3D 12. Be sure to add both the ``h`` and ``cpp`` file to your project.

> To load legacy files that require runtime format conversion (i.e. 24bpp RGB files), use _DirectXTex_ or convert them with the [texconv](https://github.com/Microsoft/DirectXTex/wiki/Texconv) utility.

> To load FourCC "XBOX" variant ``.DDS`` files, use [[XboxDDSTextureLoader]], which is supported by the [xtexconv](https://aka.ms/atgsplxtexconv) utility.

**Related tutorial:** [[Sprites and textures]]

# Header
```cpp
#include <DDSTextureLoader.h>
```

# Functions
## CreateDDSTextureFromMemory
Loads a ``.DDS`` file assuming the image of the file is located in a memory buffer. Creates a Direct3D 11 resource and optionally a Direct3D 11 shader resource view. Providing the ``ID311DeviceContext`` supports auto-generation of mipmaps.

```cpp
HRESULT CreateDDSTextureFromMemory(ID3D11Device* d3dDevice,
    const uint8_t* ddsData, size_t ddsDataSize,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    size_t maxsize = 0, DDS_ALPHA_MODE* alphaMode = nullptr);

HRESULT CreateDDSTextureFromMemory(ID3D11Device* d3dDevice,
    ID3D11DeviceContext* d3dContext,
    const uint8_t* ddsData, size_t ddsDataSize,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    size_t maxsize = 0, DDS_ALPHA_MODE* alphaMode = nullptr);
```

## CreateDDSTextureFromFile
Loads a ``.DDS`` file from disk and creates a Direct3D 11 resource and optionally a Direct3D 11 shader resource view. Providing the ``ID311DeviceContext`` supports auto-generation of mipmaps.

```cpp
HRESULT CreateDDSTextureFromFile(ID3D11Device* d3dDevice,
    const wchar_t* szFileName,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    size_t maxsize = 0, DDS_ALPHA_MODE* alphaMode = nullptr);

HRESULT CreateDDSTextureFromFile(ID3D11Device* d3dDevice,
    ID3D11DeviceContext* d3dContext,
    const wchar_t* szFileName,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    size_t maxsize = 0, DDS_ALPHA_MODE* alphaMode = nullptr);
```

## CreateDDSTextureFromMemoryEx, CreateDDSTextureFromFileEx
These versions provide explicit control over the created resource's usage, binding flags, CPU access flags, and miscellaneous flags for advanced / expert scenarios.

* The standard routines default to ``D3D11_USAGE_DEFAULT``, ``D3D11_BIND_SHADER_RESOURCE``, 0, and 0 respectively.

* For cubemaps, the miscellaneous flags default to ``D3D11_RESOURCE_MISC_TEXTURECUBE``.

* For auto-gen mipmaps, the default binding flags are ``D3D11_BIND_SHADER_RESOURCE`` | ``D3D11_BIND_RENDER_TARGET`` and miscellaneous flags is set to ``D3D11_RESOURCE_MISC_GENERATE_MIPS``.

There is also a _forceSRGB_ option for working around gamma issues with content that is in the sRGB or similar color space but is not encoded explicitly as an SRGB format. This will force return format be one of the of ``DXGI_FORMAT_*_SRGB`` formats if it exist. Note that no pixel data conversion takes place.

Note that the _maxsize_ parameter is not at the end of the parameter list like it is in the non-Ex version.

```cpp
HRESULT CreateDDSTextureFromMemoryEx(ID3D11Device* d3dDevice,
    const uint8_t* ddsData, size_t ddsDataSize,
    size_t maxsize, D3D11_USAGE usage, unsigned int bindFlags,
    unsigned int cpuAccessFlags, unsigned int miscFlags,
    bool forceSRGB,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    DDS_ALPHA_MODE* alphaMode = nullptr);

HRESULT CreateDDSTextureFromMemoryEx(ID3D11Device* d3dDevice,
    ID3D11DeviceContext* d3dContext,
    const uint8_t* ddsData, size_t ddsDataSize,
    size_t maxsize, D3D11_USAGE usage, unsigned int bindFlags,
    unsigned int cpuAccessFlags, unsigned int miscFlags,
    bool forceSRGB,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    DDS_ALPHA_MODE* alphaMode = nullptr);

HRESULT CreateDDSTextureFromFileEx(ID3D11Device* d3dDevice,
    const wchar_t* szFileName,
    size_t maxsize, D3D11_USAGE usage, unsigned int bindFlags,
    unsigned int cpuAccessFlags, unsigned int miscFlags,
    bool forceSRGB,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    DDS_ALPHA_MODE* alphaMode = nullptr);

HRESULT CreateDDSTextureFromFileEx(ID3D11Device* d3dDevice,
    ID3D11DeviceContext* d3dContext,
    const wchar_t* szFileName,
    size_t maxsize, D3D11_USAGE usage, unsigned int bindFlags,
    unsigned int cpuAccessFlags, unsigned int miscFlags,
    bool forceSRGB,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    DDS_ALPHA_MODE* alphaMode = nullptr);
```

# Parameters
Either _texture_ or _textureView_ can be nullptr, but not both.

If _maxsize_ parameter non-zero, then all mipmap levels larger than the maxsize are ignored before creating the Direct3D 11 resource. This allows for load-time scaling. If '0', then if the attempt to create the Direct3D 11 resource fails and there are mipmaps present, it will retry assuming a maxsize based on the device's current feature level.

> The reason for the default _maxsize_ behavior with 0 is to automatically handle Direct3D feature level scaling.
> This means you can create a ``.dds`` file of a 16k by 16k texture _with all mipmaps_, and it will still load
> even on a feature level 9.1 device by skipping all the mipmap sizes that are too large for the hardware to
> support. The full file is still required in memory, so it would be more efficient in terms of load-time to
> provide multiple sizes of ``.dds`` texture file, but this provides a simple scaling solution by default.

If a _d3dContext_ is given to these functions, they will attempt to use the auto-generation of mipmaps features in the Direct3D 11 API if supported for the pixel format. Note the quality of auto-gen mipmaps is up to the driver, so can vary widely. Also if a context is passed, the function is not thread safe. If _d3dContext_ is nullptr, then it functions the same as the version which does not take a context.

> auto-gen mipmaps have limited utility because the quality of the mips can vary widely. Some formats
> are not supported by the driver which will default to a texture without mips. If loading a texture
> from a file without mipmaps you can get a quick & dirty filter solution by using auto-gen. The best
> quality is obtained by generating mipmaps offline and storing your textures as ``.dds`` files with mipmaps.

The last optional parameter _alphaMode_ is a pointer to return the _alpha mode_ of the ``.DDS`` file, or it can be nullptr. This can be one of the following values to return information about the alpha channel if present in the file:

* ``DDS_ALPHA_MODE_UNKNOWN`` (0) - This is the default for most ``.DDS`` files if the specific metadata isn't present, and it's up to the application to know if it's really something else. Viewers should assume the alpha channel is intended for 'normal' alpha blending.
* ``DDS_ALPHA_MODE_STRAIGHT`` (1) - This indicates that the alpha channel if present is assumed to be using 'straight' alpha. Viewers should use the alpha channel with 'normal' alpha blending.
* ``DDS_ALPHA_MODE_PREMULTIPLIED`` (2) - This indicates the alpha channel if present is premultiplied alpha. This information is only present if the file is written using the latest version of the "DX10" extended header, or if the file is BC2/BC3 with the "DXT2"/"DXT4" FourCC which are explicitly stored as premultiplied alpha. Viewers should use the alpha channel with premultiplied alpha blending.
* ``DDS_ALPHA_MODE_OPAQUE`` (3) - This indicates that the alpha channel if present is fully opaque for all pixels. Viewers can assume there is no alpha blending.
* ``DDS_ALPHA_MODE_CUSTOM`` (4) - This indicates the alpha channel if present does not contain transparency (neither straight or premultiplied alpha) and instead is encoding some other channel of information. Viewers should not use the alpha channel for blending, and should instead view it as a distinct image channel.

For the ``Ex`` versions, the _usage_ is a [D3D11_USAGE](https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage), typically ``D3D11_USAGE_DEFAULT``. The *bindFlags* parameter is one or more [D3D11_BIND_FLAG](https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_bind_flag) values, typically ``D3D11_BIND_SHADER_RESOURCE`` for textures. The _cpuAccessFlags_ parameter is ``D3D11_CPU_ACCESS_FLAG`` typically 0 for default usage textures. The _miscFlags_ parameter is a [D3D11_RESOURCE_MISC_FLAG](https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_resource_misc_flag) value, usually 0.

# Example

This example creates a shader resource view on the Direct3D 11 device which can be used for rendering.

```cpp
using namespace DirectX;
using namespace Microsoft::WRL;

ComPtr<ID3D11ShaderResourceView> srv;
HRESULT hr = CreateDDSTextureFromFile(d3dDevice.Get(), L"SEAFLOOR.DDS",
    nullptr, srv.GetAddressOf());
DX::ThrowIfFailed(hr);
```

# Feature Level Notes
In order to support all feature levels, you should make sure your ``.DDS`` textures are mip-mapped so that they contain a suitably sized image. Non-mipmapped textures will either need explicit feature level association, or be sized less than or equal to 2048 for 1D, 2048 x 2048 for 2D, 512 x 512 for cubemaps, and 256 x 256 x 256 for volume maps.

Texture arrays require Feature Level 10.0 or later. Cubemap arrays requires Feature Level 10.1 or later.

Be sure to review the various format limitations for the different feature levels. To support all feature levels, stick with textures in the following formats:

* ``DXGI_FORMAT_R8G8B8A8_UNORM``, ``DXGI_FORMAT_R8G8B8A8_UNORM_SRGB``
* ``DXGI_FORMAT_R8G8B8A8_SNORM``
* ``DXGI_FORMAT_B8G8R8A8_UNORM``
* ``DXGI_FORMAT_R16G16_SNORM``
* ``DXGI_FORMAT_R8G8_SNORM``
* ``DXGI_FORMAT_R8_UNORM``
* ``DXGI_FORMAT_BC1_UNORM``, ``DXGI_FORMAT_BC1_UNORM_SRGB``
* ``DXGI_FORMAT_BC2_UNORM``, ``DXGI_FORMAT_BC2_UNORM_SRGB``
* ``DXGI_FORMAT_BC3_UNORM``, ``DXGI_FORMAT_BC3_UNORM_SRGB``

On Windows 8 with WDDM 1.2 drivers or later, all feature levels support 16bpp formats as well ``DXGI_FORMAT_B5G6R5_UNORM``, ``DXGI_FORMAT_B5G5R5A1_UNORM``, and ``DXGI_FORMAT_B4G4R4A4_UNORM``.

# Remarks

This code is based on the legacy DirectX SDK sample [DDSWithoutD3DX](https://walbourn.github.io/ddswithoutd3dx-sample-update/) texture loading code for Direct3D 9 / Direct3D 11.

When applying _maxsize_ and 'stripping' mipmaps on a BC compressed texture, the function may fail if the appropriately sized mipchain is not a muliple-of-4 in width & height as required by Direct3D. The only way to ensure that any given mip meets this requirement is if the top-most level is both a multiple-of-4 and a power-of-2.

DDSTextureLoader performs no run-time conversions. Therefore, it only loads DDS content that 'directly-map' to DXGI formast:

| D3DFMT | DXGI |
|--------|------|
| | DXGI_FORMAT_R8G8B8A8_UNORM |
| | DXGI_FORMAT_B8G8R8A8_UNORM |
| | DXGI_FORMAT_B8G8R8X8_UNORM |
| | DXGI_FORMAT_R10G10B10A2_UNORM |
| | DXGI_FORMAT_R16G16_UNORM |
| | DXGI_FORMAT_R32_FLOAT |
| | DXGI_FORMAT_B5G5R5A1_UNORM |
| | DXGI_FORMAT_B5G6R5_UNORM |
| | DXGI_FORMAT_B4G4R4A4_UNORM |
| | DXGI_FORMAT_R8G8_UNORM |
| | DXGI_FORMAT_R16_UNORM |
| | DXGI_FORMAT_R8_UNORM |
| | DXGI_FORMAT_R16_UNORM |
| | DXGI_FORMAT_R8G8_UNORM |
| | DXGI_FORMAT_R8_UNORM | 
| | DXGI_FORMAT_R8G8_UNORM |
| | DXGI_FORMAT_A8_UNORM |
| | DXGI_FORMAT_R8G8B8A8_SNORM |
| | DXGI_FORMAT_R16G16_SNORM |
| | DXGI_FORMAT_R8G8_SNORM |
| "DXT1" | DXGI_FORMAT_BC1_UNORM |
| "DXT2"<br />"DXT3" | DXGI_FORMAT_BC2_UNORM |
| "DXT4"<br />"DXT5" | DXGI_FORMAT_BC3_UNORM |
| "ATI1"<br />"BC4U" | DXGI_FORMAT_BC4_UNORM |
| "BC4S" | DXGI_FORMAT_BC4_SNORM |
| "ATI2"<br />"BC5U" | DXGI_FORMAT_BC5_UNORM |
| "BC5S" | DXGI_FORMAT_BC5_SNORM |
| "RGBG" | DXGI_FORMAT_R8G8_B8G8_UNORM |
| "GRGB" | DXGI_FORMAT_G8R8_G8B8_UNORM |
| "YUY2" | DXGI_FORMAT_YUY2 |
| D3DFMT_A16B16G16R16 | DXGI_FORMAT_R16G16B16A16_UNORM |
| D3DFMT_Q16W16V16U16 | DXGI_FORMAT_R16G16B16A16_SNORM |
| D3DFMT_R16F | DXGI_FORMAT_R16_FLOAT |
| D3DFMT_G16R16F | DXGI_FORMAT_R16G16_FLOAT |
| D3DFMT_A16B16G16R16F | DXGI_FORMAT_R16G16B16A16_FLOAT |
| D3DFMT_R32F | DXGI_FORMAT_R32_FLOAT |
| D3DFMT_G32R32F | DXGI_FORMAT_R32G32_FLOAT |
| D3DFMT_A32B32G32R32F | DXGI_FORMAT_R32G32B32A32_FLOAT |


If there is not a direct mapping to a DXGI supported format, the function fails. You can make use of the [DirectXTex library](http://go.microsoft.com/fwlink/?LinkId=248926) or ``texconv`` tool to convert legacy Direct3D9 ``.DDS`` files to a supported format. Legacy formats which require conversion include:

| D3DFMT | Notes |
|--------|-------|
| D3DFMT_R8G8B8 | (24bpp RGB) - Use a 32bpp format |
| D3DFMT_X8B8G8R8 | (32bpp RGBX) - Use BGRX, BGRA, or RGBA |
| D3DFMT_A2R10G10B10 | (BGRA 10:10:10:2) - Use RGBA 10:10:10:2 |
| D3DFMT_X1R5G5B5 | (BGR 5:5:5) - Use BGRA 5:5:5:1 or BGR 5:6:5 |
| D3DFMT_A8R3G3B2<br />D3DFMT_R3G3B2 | (BGR 3:3:2) - Expand to a supported format |
| D3DFMT_P8<br />D3DFMT_A8P8 | (8-bit palette) - Expand to a supported format |
| D3DFMT_A4L4 | (Luminance 4:4) - Expand to a supported format |
| D3DFMT_UYVY | (YUV 4:2:2 16bpp) - Swizzle to YUY2 |

> There's no DXGI format equivalent or obvious conversion for ``D3DFMT_CxV8U8``, ``D3DFMT_L6V5U5``, ``D3DFMT_X8L8V8U8``, or ``D3DFMT_A2W10V10U10``.

On a system with the DirectX 11.0 Runtime or lacking WDDM 1.2 drivers, attempts to load 16bpp format files (BGR 5:6:5, BGRA 5:5:5:1, and BGRA 4:4:4:4) will fail.

Partial cubemaps (i.e. ``.DDS`` files without all six faces defined) are not supported by Direct3D 11.

# Windows Store apps
The texture loader function is typically used to load texture files from the application's install folder as they were included with the AppX package for Windows Store apps and Universal Windows Platform (UWP) apps. If you wish to create a texture from a file that is specified by the user from a WinRT file picker, you will need to copy the file locally to a temporary location before you can use DDSTextureLoader on it. This is because you either won't have file access rights to the user's file location, or the StorageFile is actually not a local file system path (i.e. it's a URL).

## C++/CX
```cpp
#include <ppltasks.h>
using namespace concurrency;

using Windows::Storage;
using Windows::Storage::Pickers;

auto openPicker = ref new FileOpenPicker();
openPicker->ViewMode = PickerViewMode::Thumbnail; 
openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary; 
openPicker->FileTypeFilter->Append(".dds"); 

create_task(openPicker->PickSingleFileAsync()).then([](StorageFile^ file)
{
    if (file)
    {
        auto tempFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder;
        create_task(file->CopyAsync(tempFolder, file->Name, NameCollisionOption::GenerateUniqueName)).then([](StorageFile^ tempFile)
        {
            if (tempFile)
            {
                HRESULT hr = CreateDDSTextureFromFile(..., tempFile->Path->Data(), ...);
                DeleteFile(tempFile->Path->Data());
                DX::ThrowIfFailed(hr);
            }
        });
    }
});
```

## C++/WinRT
```cpp
#include "winrt/Windows.Storage.h"
#include "winrt/Windows.Storage.Pickers.h"

using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;

FileOpenPicker openPicker;
openPicker.ViewMode(PickerViewMode::Thumbnail);
openPicker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
openPicker.FileTypeFilter().Append(L".dds");

auto file = co_await openPicker.PickSingleFileAsync();
if (file)
{
    auto tempFolder = ApplicationData::Current().TemporaryFolder();
    auto tempFile = co_await file.CopyAsync(tempFolder, file.Name(), NameCollisionOption::GenerateUniqueName);
    if (tempFile)
    {
        HRESULT hr = CreateDDSTextureFromFile(..., tempFile.Path().c_str(), ...);
        DeleteFile(tempFile.Path().c_str());
        DX::ThrowIfFailed(hr);
    }
}
```

See [File access and permissions (Windows Runtime apps)](https://docs.microsoft.com/en-us/windows/uwp/files/file-access-permissions)

# Debugging texture loading

If the ``HRESULT`` is a success, but you are still having problems using the texture, the next step in debugging is to examine details about the loaded texture. You can do this with the following code:

```cpp
ComPtr<ID3D11Resource> res;
ComPtr<ID3D11ShaderResourceView> srv;
HRESULT hr = CreateDDSTextureFromFile(d3dDevice.Get(), L"SEAFLOOR.DDS",
    res.GetAddressOf(), srv.GetAddressOf());
DX::ThrowIfFailed(hr);

D3D11_RESOURCE_DIMENSION resType = D3D11_RESOURCE_DIMENSION_UNKNOWN;
res->GetType( &resType );

switch( resType )
{
case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
    {
        ComPtr<ID3D11Texture1D> tex;
        hr = res.As(&tex);
        DX::ThrowIfFailed(hr);

        D3D11_TEXTURE1D_DESC desc;
        tex->GetDesc(&desc);

        // This is a 1D texture. Check values of desc here
    }
    break;

case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
    {
        ComPtr<ID3D11Texture2D> tex;
        hr = res.As(&tex);
        DX::ThrowIfFailed(hr);

        D3D11_TEXTURE2D_DESC desc;
        tex->GetDesc(&desc);

        // This is a 2D texture. Check values of desc here
    }
    break;

case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
    {
        ComPtr<ID3D11Texture3D> tex;
        hr = res.As(&tex);
        DX::ThrowIfFailed(hr);

        D3D11_TEXTURE3D_DESC desc;
        tex->GetDesc(&desc);

        // This is a 3D volume texture. Check values of desc here
    }
    break;

default:
    // Error!
    break;
}
```

``desc.Width`` is the texture width in pixels (of the top most mip level)  
``desc.Height`` is the texture height in pixels (of the top most mip level) for 2D and 3D textures  
``desc.Depth`` is the texture depth (of the top most level) for 3D textures  
``desc.MipLevels`` is the number of mip levels (or 1 if there are no mips)  
``desc.ArraySize`` is the number of textures in the array (or 1 if this not an array)  
``desc.Format`` is the DXGI format of the texture resource

# DDS Files
This function loads both traditional and FourCC "DX10" variant ``.DDS`` files.

[DDS on MSDN](https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dx-graphics-dds)

# Further Reading
[The DDS File Format Lives](https://walbourn.github.io/the-dds-file-format-lives/)

[Direct3D Feature Levels](https://aka.ms/Apsgrj)

[Programming Guide for DXGI](https://docs.microsoft.com/en-us/windows/desktop/direct3ddxgi/dx-graphics-dxgi-overviews)

[Linear-Space Lighting (i.e. Gamma)](http://filmicworlds.com/blog/linear-space-lighting-i-e-gamma/)

[Chapter 24. The Importance of Being Linear](https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-24-importance-being-linear), GPU Gems 3

[Gamma-correct rendering](https://blog.molecular-matters.com/2011/11/21/gamma-correct-rendering/)   
