This is a version of [[DDSTextureLoader]] for use by Xbox One XDK developers. This loads pre-tiled texture resources for use with the ``CreatePlacement`` APIs available to Xbox One exclusive app developers.

These pre-tiled resources are stored in a variant of the ``.DDS`` file format marked with the "XBOX" FourCC pixel format. To generate these .DDS files, see the *xtexconv* sample on the [Microsoft Game Developer Network](https://developer.xboxlive.com/en-us/platform/development/education/Pages/Samples.aspx) site (_access required_).

_To load traditional or FourCC "DX10" variant DDS files, use [[DDSTextureLoader]]_

# Header
    #include <XboxDDSTextureLoader.h>

# Functions
These are in the **Xbox** namespace rather than **DirectX**

## CreateDDSTextureFromMemory. CreateDDSTextureFromFile*
These are equivalent to the same functions in _DDSTextureLoader_, but require a _ID3D11DeviceX_ interface and return a pointer to the video memory via _grfxMemory_ which the caller is responsible for releasing with *D3DFreeGraphicsMemory*

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

# Parameters
Either _texture_ or _textureView_ can be nullptr, but not both.

# Example

    using namespace Microsoft::WRL;

    ComPtr<ID3D11ShaderResourceView> srv;

    void *grfxMemory = nullptr;
    HRESULT hr = Xbox::CreateDDSTextureFromFile( m_device.Get(),
        L"XboxTiledTexturedds", nullptr,
        srv.GetAddressOf(), &grfxMemory );
    DX::ThrowIfFailed( hr );

# DDS File Format
This function expects the DDS file to contain a pixel format with FourCC "XBOX" which signifies a specific variant of the file.

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
    // Remainder of file is a tiled texture binary layout suitable for use with CreatePlacement APIs

Other ``DDS`` loaders and viewers will fail to recognize this pixel format, and other ``DDS`` creation tools do not support creating it. See the **xtexconv** sample for more information on creating this variant form of DDS files.

[DDS on MSDN](http://msdn.microsoft.com/en-us/library/windows/desktop/bb943990.aspx])
