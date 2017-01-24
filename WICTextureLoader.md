A Direct3D 11 2D texture loader that uses WIC to load a bitmap (``BMP``, ``JPEG``, ``PNG``, ``TIFF``, ``GIF``, HD Photo, or other WIC supported file container), resize if needed based on the current feature level (or by explicit parameter), format convert to a standard DXGI format if required, and then create a 2D texture. Furthermore, if a Direct3D 11 device context is provided and the current device supports it for the given pixel format, it will auto-generate mipmaps.

This loader does not support array textures, 1D textures, 3D volume textures, or cubemaps. For these scenarios, use the ``.DDS`` file format and [[DDSTextureLoader]] instead.

[[DDSTextureLoader]] is recommended for fully "precooked" textures for maximum performance and image quality, but this loader can be useful for creating simple 2D texture from standard image files at runtime.

> On Windows 8.1 and Windows 10, you can load simple 2D textures in BC1, BC2, or BC3 pixel format ``DDS`` files using WIC as there is a basic DDS built-in codec present. All other pixel formats and resource types will fail, and you'll get ``WINCODEC_ERR_COMPONENTNOTFOUND`` on older versions of Windows trying to use WICTextureLoader to load ``DDS`` files. Use [[DDSTextureLoader]] instead.

Also part of the [DirectXTex](http://go.microsoft.com/fwlink/?LinkId=248926) package.

**Related tutorial:** [[Sprites and textures]]

# Header
    #include <WICTextureLoader.h>

# Initialization
The library assumes that the client code will have already called ``CoInitialize``, ``CoInitializeEx``, or ``Windows::Foundation::Initialize`` as needed by the application before calling any [Windows Imaging Component](https://msdn.microsoft.com/en-us/library/windows/desktop/ee719654.aspx) functionality.

For a Universal Windows Platform (UWP) app, the Windows Runtime and COM is initialized by the C/C++ Run-Time. For a classic Windows desktop application you have to do this explicitly:

    #if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
        Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
        if (FAILED(initialize))
            // error
    #else
        HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
        if (FAILED(hr))
            // error
    #endif

# Functions

## CreateWICTextureFromMemory
Loads a WIC-supported bitmap file from a memory buffer. It creates a Direct3D 11 resource from it, and optionally a Direct3D 11 shader resource view.

    HRESULT CreateWICTextureFromMemory( ID3D11Device* d3dDevice,
       const uint8_t* wicData, size_t wicDataSize,
       ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
       size_t maxsize = 0 );

    HRESULT CreateWICTextureFromMemory( ID3D11Device* d3dDevice,
       ID3D11DeviceContext* d3dContext,
       const uint8_t* wicData, size_t wicDataSize,
       ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
       size_t maxsize = 0 );

## CreateWICTextureFromFile
Loads a WIC-supported bitmap file from disk, creates a Direct3D 11 resource from it, and optionally a Direct3D 11 shader resource view.

    HRESULT CreateWICTextureFromFile( ID3D11Device* d3dDevice,
       const wchar_t* szFileName,
       ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
       size_t maxsize = 0 );

    HRESULT CreateWICTextureFromFile( ID3D11Device* d3dDevice,
       ID3D11DeviceContext* d3dContext,
       const wchar_t* szFileName,
       ID3D11Resource** texture, ID3D11ShaderResourceView** textureView,
       size_t maxsize = 0 );

## CreateWICTextureFromMemoryEx, CreateWICTextureFromFileEx
These versions provide explicit control over the created resource's usage, binding flags, CPU access flags, and miscellaneous flags for advanced / expert scenarios.

The standard routines default to ``D3D11_USAGE_DEFAULT``, ``D3D11_BIND_SHADER_RESOURCE``, 0, and 0 respectively.

For auto-gen mipmaps, the default binding flags are ``D3D11_BIND_SHADER_RESOURCE`` | ``D3D11_BIND_RENDER_TARGET`` and miscellaneous flags is set to ``D3D11_RESOURCE_MISC_GENERATE_MIPS``.

There is also a _loadFlags_ parameter. The flags are ``WIC_LOADER_DEFAULT``, ``WIC_LOADER_FORCE_SRGB``, and ``WIC_LOADER_IGNORE_SRGB``.

    HRESULT CreateWICTextureFromMemoryEx( ID3D11Device* d3dDevice,
       const uint8_t* wicData, size_t wicDataSize,
       size_t maxsize, D3D11_USAGE usage, unsigned int bindFlags,
       unsigned int cpuAccessFlags, unsigned int miscFlags,
       unsigned int loadFlags,
       ID3D11Resource** texture, ID3D11ShaderResourceView** textureView );

    HRESULT CreateWICTextureFromMemoryEx( ID3D11Device* d3dDevice,
       ID3D11DeviceContext* d3dContext,
       const uint8_t* wicData, size_t wicDataSize,
       size_t maxsize, D3D11_USAGE usage, unsigned int bindFlags,
       unsigned int cpuAccessFlags, unsigned int miscFlags,
       unsigned int loadFlags,
       ID3D11Resource** texture, ID3D11ShaderResourceView** textureView );

    HRESULT CreateWICTextureFromFileEx( ID3D11Device* d3dDevice,
       const wchar_t* szFileName,
       size_t maxsize, D3D11_USAGE usage, unsigned int bindFlags,
       unsigned int cpuAccessFlags, unsigned int miscFlags,
       unsigned int loadFlags,
       ID3D11Resource** texture, ID3D11ShaderResourceView** textureView );

    HRESULT CreateWICTextureFromFileEx( ID3D11Device* d3dDevice,
       ID3D11DeviceContext* d3dContext,
       const wchar_t* szFileName,
       size_t maxsize, D3D11_USAGE usage, unsigned int bindFlags,
       unsigned int cpuAccessFlags, unsigned int miscFlags,
       unsigned int loadFlags,
       ID3D11Resource** texture, ID3D11ShaderResourceView** textureView );

> The ``loadFlags`` was previously a ``bool forceSRGB``. ``false`` will map to ``WIC_LOADER_DEAULT`` and ``true`` maps to ``WIC_LOADER_FORCE_SRGB``.

# Parameters
Either _texture_ or _textureView_ can be nullptr, but not both.

For all these functions above, the _maxsize_ parameter provides an upper limit on the size of the resulting texture. If given a 0, the functions assume a maximum size determined from the device's current feature level. If the bitmap file contains a larger image, it will be resized using WIC at load-time to provide scaling.

If a _d3dContext_ is given to these functions, they will attempt to use the auto-generation of mipmaps features in the Direct3D 11 API if supported for the pixel format. Note the quality of auto-gen mipmaps is up to the driver, so can vary widely. Also if a context is passed, the function is not thread safe. If _d3dContext_ is nullptr, then it functions the same as the version which does not take a context.

# Example

This example creates a shader resource view on the Direct3D device which can be used for rendering. It also makes use of the immediate device context to auto-gen mipmaps if supported.

    using namespace DirectX;
    using namespace Microsoft::WRL;

    ComPtr<ID3D11ShaderResourceView> srv;
    HRESULT hr = CreateWICTextureFromFile( d3dDevice.Get(), immContext.Get(),
         L"LOGO.BMP", nullptr, srv.GetAddressOf() );
    DX::ThrowIfFailed(hr);

# Release Notes
* On a system with the DirectX 11.0 Runtime or lacking WDDM 1.2 drivers, 16bpp pixel formats will be converted to a RGBA 32-bit format.

* WICTextureLoader cannot load ``.TGA``/``.HDR`` files unless the system has a 3rd party WIC codec installed. You must use the [DirectXTex library](http://go.microsoft.com/fwlink/?LinkId=248926) for ``TGA``/``HDR`` file format support without relying on an add-on WIC codec.

* While there is no explicit 'sRGB' pixel format defined for WIC, the load function will check for known metadata tags and may return ``DXGI_FORMAT_*_SRGB`` formats if there are equivalents of the same size and channel configuration available. For PNG, this is indicated by ``/sRGB/RenderingIntent`` set to 1. For JPG this is ``/app1/ifd/exif/{ushort=40961}`` set to 1. For TIFF this is ``/ifd/exif/{ushort=40961}`` set to 1. Setting _loadFlags_ to ``WIC_LOADER_IGNORE_SRGB`` will ignore this metadata.

* ``gAMA`` chunks in PNGs are ignored. If the ``sRGB`` chunk is found, it is assumed to be gamma 2.2.

# Implementation Details
* The conversion tables are designed so that they prefer to convert to RGB if a conversion is required as a general preferance for DXGI 1.0 supporting formats supported by WDDM 1.0 drivers. The majority of Direct3D 11 devices actually support BGR DXGI 1.1 formats so we use them when they are the best match. For example, ``GUID_WICPixelFormat32bppBGRA`` loads directly as ``DXGI_FORMAT_B8G8R8A8_UNORM``, but ``GUID_WICPixelFormat32bppPBGRA`` converts to ``DXGI_FORMAT_R8G8B8A8_UNORM``.

* ``GUID_WICPixelFormatBlackWhite`` is always converted to a greyscale ``DXGI_FORMAT_R8_UNORM`` since ``DXGI_FORMAT_R1_UNORM`` is not supported by Direct3D 10.x/11.x.

* ``GUID_WICPixelFormat32bppRGBE`` is an 8:8:8:8 format, which does not match ``DXGI_FORMAT_R9G9B9E5_SHAREDEXP``. This WIC pixel format is therefore converted to ``GUID_WICPixelFormat128bppRGBAFloat`` and returns as ``DXGI_FORMAT_R32G32B32A32_FLOAT``.

# WIC2
WIC2 is available on Windows 10, Windows 8.x, and on Windows 7 Service Pack 1 with [KB 2670838](http://support.microsoft.com/kb/2670838) installed.

* If WIC2 is supported, then it will load the new WIC pixel format ``GUID_WICPixelFormat96bppRGBFloat`` directly as ``DXGI_FORMAT_R32G32B32_FLOAT``. Otherwise the module converts this to ``DXGI_FORMAT_R32G32B32A32_FLOAT``.

* If WIC2 is supported, then it will include conversions cases for the new WIC pixel formats ``GUID_WICPixelFormat32bppRGB``, ``GUID_WICPixelFormat64bppRGB``, and ``GUID_WICPixelFormat64bppPRGBAHalf``.

* If WIC2 is supported, then it will convert the WIC pixel format ``GUID_WICPixelFormat96bppRGBFixedPoint`` to ``DXGI_FORMAT_R32G32B32_FLOAT``. There is special-case handling so that if auto-gen mips fails for this format (this is optional support for Feature Level 10.0 or later devices), it will use ``DXGI_FORMAT_R32G32B32A32_FLOAT`` instead (which has required support for Feature Level 10.0 or later devices).

See [Windows Imaging Component and Windows 8](http://blogs.msdn.com/b/chuckw/archive/2012/11/19/windows-imaging-component-and-windows-8.aspx)

# Windows Store apps
The texture loader function is typically used to load texture files from the application's install folder as they were included with the AppX package for Windows Store apps and universal Windows apps. If you wish to create a texture from a file that is specified by the user from a WinRT picker, you will need to copy the file locally to a temporary location before you can use WICTextureLoader on it. This is because you either won't have file access rights to the user's file location, or the StorageFile is actually not a local file system path (i.e. it's a URL).

    include <ppltasks.h>
    using namespace concurrency;

    using Windows::Storage;
    using Windows::Storage::Pickers;

    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file)
        {
            auto tempFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder;
            create_task(file->CopyAsync( tempFolder, file->Name, NameCollisionOption::GenerateUniqueName )).then([this](StorageFile^ tempFile)
            {
                if ( tempFile )
                {
                    HRESULT hr = CreateWICTextureFromFile( ..., tempFile->Path->Data(), ... );
                    DeleteFile(tempFile->Path->Data());
                    DX::ThrowIfFailed(hr);
                }
            });
        });
    
[File access and permissions (Windows Runtime apps)](https://msdn.microsoft.com/en-us/library/windows/apps/hh967755.aspx)

# Debugging texture loading

If the ``HRESULT`` is a success, but you are still having problems using the texture, the next step in debugging is to examine details about the loaded texture. You can do this easily with the following code:

    ComPtr<ID3D11Resource> res;
    ComPtr<ID3D11ShaderResourceView> srv;
    HRESULT hr = CreateWICTextureFromFile( d3dDevice.Get(), immContext.Get(),
         L"LOGO.BMP", res.GetAddressOf(), srv.GetAddressOf() );
    DX::ThrowIfFailed(hr);

    ComPtr<ID3D11Texture2D> tex;
    hr = res.As(&tex);
    DX::ThrowIfFailed(hr);

    D3D11_TEXTURE2D_DESC desc;
    tex->GetDesc(&desc);

``desc.Width`` is the texture width in pixels (of the top most mip level)  
``desc.Height`` is the texture height in pixels (of the top most mip level)  
``desc.MipLevels`` is the number of mip levels (or 1 if there are no mips)  
``desc.ArraySize`` is the number of textures in the array (or 1 if this not an array)  
``desc.Format`` is the DXGI format of the texture resource

# Further reading

[Linear-Space Lighting (i.e. Gamma)](http://filmicgames.com/archives/299)

[Chapter 24. The Importance of Being Linear](http://http.developer.nvidia.com/GPUGems3/gpugems3_ch24.html), GPU Gems 3

[Gamma-correct rendering](https://blog.molecular-matters.com/2011/11/21/gamma-correct-rendering/)   
