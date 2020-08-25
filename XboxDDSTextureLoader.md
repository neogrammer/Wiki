This is a version of [[DDSTextureLoader]] for use by Xbox One XDK developers. This loads pre-tiled texture resources for use with the ``CreatePlacement`` APIs available to Xbox One exclusive app developers.

These pre-tiled resources are stored in a variant of the ``.DDS`` file format marked with the "XBOX" FourCC pixel format. To generate these ``.DDS`` files, see the [xtexconv](https://aka.ms/atgsplxtexconv).

> To load legacy ``DDPIXELFORMAT`` or FourCC "DX10" ``DXGI_FORMAT`` variant DDS files, use [[DDSTextureLoader]]

This function is only usable by [Xbox registered developers](https://www.xbox.com/en-US/developers) via the Xbox One XDK.

# Header
```cpp
#include <XboxDDSTextureLoader.h>
```

# Functions
These are in the **Xbox** namespace rather than **DirectX**

## CreateDDSTextureFromMemory, CreateDDSTextureFromFile
These are equivalent to the same functions in _DDSTextureLoader_.

```cpp
HRESULT CreateDDSTextureFromMemory( ID3D11DeviceX* d3dDevice,
    const uint8_t* ddsData, size_t ddsDataSize,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    void** grfxMemory, DDS_ALPHA_MODE* alphaMode = nullptr,
    bool forceSRGB = false );

HRESULT CreateDDSTextureFromFile( ID3D11DeviceX* d3dDevice,
    const wchar_t* szFileName,
    ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
    void** grfxMemory,
    DDS_ALPHA_MODE* alphaMode = nullptr,
    bool forceSRGB = false );
```

# Parameters
Either _texture_ or _textureView_ can be nullptr, but not both.

The _grfxMemory_ is returned as a pointer to the video memory which the caller is responsible for releasing with **FreeDDSTextureMemory**:

```cpp
void FreeDDSTextureMemory( _In_opt_ void* grfxMemory );
```

There is also a _forceSRGB_ option for working around gamma issues with content that is in the sRGB or similar color space but is not encoded explicitly as an SRGB format. This will force return format be one of the of ``DXGI_FORMAT_*_SRGB`` formats if it exist. Note that no pixel data conversion takes place.

# Remark
Auto-gen mipmaps are not supported with these functions.

# Example

```cpp
using namespace Microsoft::WRL;

ComPtr<ID3D11ShaderResourceView> srv;

void *grfxMemory = nullptr;
HRESULT hr = Xbox::CreateDDSTextureFromFile( m_device.Get(),
    L"XboxTiledTexturedds", nullptr,
    srv.GetAddressOf(), &grfxMemory );
DX::ThrowIfFailed( hr );
```

# DDS File Format
This function expects the DDS file to contain a pixel format with FourCC "XBOX" which signifies a specific variant of the file.

```cpp
DWORD dwMagic
DDS_HEADER header
DDS_HEADER_XBOX
{
    DXGI_FORMAT dxgiFormat;
    uint32_t    resourceDimension;
    uint32_t    miscFlag; // see DDS_RESOURCE_MISC_FLAG
    uint32_t    arraySize;
    uint32_t    miscFlags2; // see DDS_MISC_FLAGS2
    uint32_t    tileMode; // see XG_TILE_MODE
    uint32_t    baseAlignment;
    uint32_t    dataSize;
    uint32_t    xdkVer; // matching _XDK_VER
} headerXbox
// Remainder of file is a tiled texture binary layout suitable
// for use with CreatePlacement APIs
```

Other ``DDS`` loaders and viewers will fail to recognize this pixel format, and other ``DDS`` creation tools do not support creating it. See the **xtexconv** sample for more information on creating this variant form of DDS files.

[DDS on Microsoft Docs](https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dx-graphics-dds)
