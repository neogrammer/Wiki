A Direct3D 11 2D texture save routine for generating a "screenshot" from a render target texture. There is a function that will dump the 2D texture to a ``.DDS`` file, and another that will write using WIC (``BMP``, ``JPEG``, ``PNG``, ``TIFF``, ``GIF``, JPEG-XR / HD Photo, or other WIC supported file container).

These writers do not support array textures, 1D textures, 3D volume textures, or cubemaps. Mipmaps are also ignored. For those scenarios, use the _DirectXTex_ library functionality. The ScreenGrab module is really designed to make a quick and light-weight solution for capturing screenshots at runtime.

MSAA textures are resolved before being written.

Also part of the [DirectXTex](http://go.microsoft.com/fwlink/?LinkId=248926) package.

# Header
    #include <ScreenGrab.h>

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
## SaveDDSTextureToFile
Saves a texture to a ``DDS`` file on disk. It performs no format conversions, but will try to favor writing legacy ``.DDS`` files when possible for improved tool support.

    HRESULT SaveDDSTextureToFile(ID3D11DeviceContext* pContext,
        ID3D11Resource* pSource, LPCWSTR fileName );

## SaveWICTextureToFile
Saves a texture to a WIC-supported bitmap file on disk. The caller provides the desired WIC container format via _guidContainerFormat_ and can optionally specify a desired WIC pixel format via _targetFormat_ (which will result in ``E_FAIL`` if the requested pixel format is not supported by the WIC codec). If no WIC pixel format GUID is provided as the _targetFormat_ parameter, it will default to a non-alpha format since 'screenshots' usually ignore the alpha channel in render targets.

    HRESULT SaveWICTextureToFile( ID3D11DeviceContext* pContext,
        ID3D11Resource* pSource,
        REFGUID guidContainerFormat, 
        LPCWSTR fileName,
        const GUID* targetFormat = nullptr,
        std::function<void(IPropertyBag2*)> setCustomProps = nullptr );

# Examples

This example saves a JPEG screenshot given a content and swapchain.

    using namespace DirectX;
    using namespace Microsoft::WRL;

    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ),
       reinterpret_cast<LPVOID*>( backBuffer.GetAddressOf() ) );
    if ( SUCCEEDED(hr) )
    {
        hr = SaveWICTextureToFile( immContext.Get(), backBuffer.Get(),
                    GUID_ContainerFormatJpeg, L"SCREENSHOT.JPG" );
    }
    DX::ThrowIfFailed(hr);

Here is an example of explicitly writing a screenshot as a 16-bit (5:6:5) BMP.

    using namespace DirectX;
    using namespace Microsoft::WRL;

    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ),
       reinterpret_cast<LPVOID*>( backBuffer.GetAddressOf() ) );
    if ( SUCCEEDED(hr) )
    {
        hr = SaveWICTextureToFile( immContext.Get(), backBuffer.Get(),
                    GUID_ContainerFormatBmp, L"SCREENSHOT.BMP",
                    &GUID_WICPixelFormat16bppBGR565 );
    }
    DX::ThrowIfFailed(hr);

When writing WIC files, you can also provide a callback for setting specific encoding options.

    using namespace DirectX;
    using namespace Microsoft::WRL;

    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ),
       reinterpret_cast<LPVOID*>( backBuffer.GetAddressOf() ) );
    if ( SUCCEEDED(hr) )
    {
        hr = SaveWICTextureToFile( immContext.Get(), backBuffer.Get(),
                    GUID_ContainerFormatTiff, L"SCREENSHOT.TIF", nullptr,
                    [&](IPropertyBag2* props)
                    {
                        PROPBAG2 options[2] = { 0, 0 };
                        options[0].pstrName = L"CompressionQuality";
                        options[1].pstrName = L"TiffCompressionMethod";

                        VARIANT varValues[2];
                        varValues[0].vt = VT_R4;
                        varValues[0].fltVal = 0.75f;

                        varValues[1].vt = VT_UI1;
                        varValues[1].bVal = WICTiffCompressionNone;

                        (void)props->Write( 2, options, varValues ); 
                    });
    }
    DX::ThrowIfFailed(hr);

# Release Notes
* JPEG-XR / HD Photo supports nearly all WIC pixel formats including floating-point for both encoding and decoding.

* TIFF can contain floating-point data (128bpp or 96bpp), but the WIC built-in codec can only decode such images. It always converts floating-point data to unorm when encoding.

* Paletted WIC formats are not supported for writing by the ``SaveWICTextureToFile`` function.

# WIC2
WIC2 is available on Windows 10, Windows 8.x, and on Windows 7 Service Pack 1 with [KB 2670838](http://support.microsoft.com/kb/2670838) installed.

* If WIC2 is supported, then this function can make use of the new WIC pixel format ``GUID_WICPixelFormat96bppRGBFloat``.

See [Windows Imaging Component and Windows 8](http://blogs.msdn.com/b/chuckw/archive/2012/11/19/windows-imaging-component-and-windows-8.aspx)

# Windows Store apps
For _Save*TextureToFile_ to succeed, the application must have write access to the destination path. For Windows Store apps and Universal Windows Platform (UWP) app, the file access permissions are rather restricted so you'll need to make sure you use a fully qualified path to a valid write folder. A good location to use is the app data folder:

## C++/CX
    auto folder = Windows::Storage::ApplicationData::Current->LocalFolder;
    // use folder->Path->Data() as the path base

## C++/WinRT
    #include "winrt/Windows.Storage.h"
    auto tempFolder = winrt::Windows::Storage::ApplicationData::Current().TemporaryFolder();
    // use tempFile.Path().c_str() as the path base

If you are going to immediately copy it to another location via ``StorageFolder::MoveAndReplaceAsync``, then use the app's temporary folder:

## C++/CX
    #include <ppltasks.h>
    using namespace concurrency;

    using Windows::Storage;
    using Windows::Storage::Pickers;

    auto folder = ApplicationData::Current->TemporaryFolder;

    wchar_t fname[ _MAX_PATH ];
    wcscpy_s( fname, folder->Path->Data() );
    wcscat_s( fname, L"\\screenshot.jpg" );

    // note that context use is not thread-safe
    HRESULT hr = SaveWICTextureToFile( immContext.Get(), backBuffer.Get(),
        GUID_ContainerFormatJpeg, fname  );

    DX::ThrowIfFailed(hr);

    auto jpegExtensions = ref new Platform::Collections::Vector<String^>();
    jpegExtensions->Append(".jpg");

    auto pngExtensions = ref new Platform::Collections::Vector<String^>();
    pngExtensions->Append(".png");

    auto savePicker = ref new FileSavePicker();
    savePicker->SuggestedStartLocation = PickerLocationId.PicturesLibrary;
    savePicker->FileTypeChoices->Insert("Joint Photographic Experts Group", jpegExtensions);
    savePicker->FileTypeChoices->Insert("Portable Network Graphics", pngExtensions);
    savePicker->SuggestedFileName = "Screenshot";

    create_task(savePicker->PickSaveFileAsync()).then([](StorageFile^ file)
    {
        if ( file )
        {
            auto folder = Windows::Storage::ApplicationData::Current->TemporaryFolder;
            auto task = create_task( folder->GetFileAsync("screenshot.jpg") );
            task.then([file](StorageFile^ tempFile) ->IAsyncAction^
            {
                return tempFile->MoveAndReplaceAsync( file );
            });
        }
    });

## C++/WinRT
    #include "winrt/Windows.Storage.h"
    #include "winrt/Windows.Storage.Pickers.h"

    using namespace winrt::Windows::Storage;
    using namespace winrt::Windows::Storage::Pickers;

    auto folder = ApplicationData::Current().TemporaryFolder();

    wchar_t fname[_MAX_PATH];
    wcscpy_s(fname, folder.Path().c_str());
    wcscat_s(fname, L"\\screenshot.jpg");

    // note that context use is not thread-safe
    HRESULT hr = SaveWICTextureToFile(immContext.Get(), backBuffer.Get(),
        GUID_ContainerFormatJpeg, fname);

    DX::ThrowIfFailed(hr);

    FileSavePicker savePicker;
    savePicker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
    savePicker.FileTypeChoices(...);
    savePicker.SuggestedFileName(L"Screenshot");
    auto file = co_await savePicker.PickSaveFileAsync();
    if (file)
    {
        auto tempFolder = ApplicationData::Current().TemporaryFolder();
        auto tempFile = co_await file.CopyAsync(tempFolder, file.Name(), NameCollisionOption::GenerateUniqueName);
        if (tempFile)
        {
            co_await tempFile.MoveAndReplaceAsync(file);
        }
    }

See [File access and permissions (Windows Runtime apps)](https://msdn.microsoft.com/en-us/library/windows/apps/hh967755.aspx), 
[ApplicationData.TemporaryFolder property](http://msdn.microsoft.com/en-us/library/windows/apps/xaml/windows.storage.applicationdata.temporaryfolder.aspx)

# Xbox One
When using Direct3D 11.x fast semantics and trying to capture a Render Target, it must be decompressed with ``DecompressResource`` before calling ``SaveDDSTextureToFile`` or ``SaveWICTextureToFile``. The save functions will block until the resource copy is completed, so it should not be used with high frequency.