Contains various C++ utilities that simply Direct3D 11 programming.

# Header
    #include <DirectXHelpers.h>

# Direct3D 11 "core" helpers
The standard ``d3d11_1.h`` headers in the Windows 8.x SDK already include a number of helper classes intended to simplify usage of the API when using C++. These are all in the global C++ namespace.

* ``CD3D11_RECT``
* ``CD3D11_BOX``
* ``CD3D11_DEPTH_STENCIL_DESC``
* ``CD3D11_BLEND_DESC``
* ``CD3D11_BLEND_DESC1``
* ``CD3D11_RASTERIZER_DESC``
* ``CD3D11_RASTERIZER_DESC1``
* ``CD3D11_BUFFER_DESC``
* ``CD3D11_TEXTURE1D_DESC``
* ``CD3D11_TEXTURE2D_DESC``
* ``CD3D11_TEXTURE3D_DESC``
* ``CD3D11_SHADER_RESOURCE_VIEW_DESC``
* ``CD3D11_RENDER_TARGET_VIEW_DESC``
* ``CD3D11_VIEWPORT``
* ``CD3D11_DEPTH_STENCIL_VIEW_DESC``
* ``CD3D11_UNORDERED_ACCESS_VIEW_DESC``
* ``CD3D11_SAMPLER_DESC``
* ``CD3D11_QUERY_DESC``
* ``CD3D11_COUNTER_DESC``

See [MSDN](http://msdn.microsoft.com/en-us/library/windows/desktop/jj151647.aspx) for more information on these helpers.

_Note: These helpers are excluded for C language builds or if the preprocessor define ``D3D11_NO_HELPERS`` is set._

# Exception-safe Direct3D 11 resource locking
Modern C++ development strongly encourages use of the [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern for exception-safe resource management, ensuring that resources are properly cleaned up if C++ exceptions are thrown. Even without exception handling, it's generally cleaner code to use RAII and rely on the C++ scope variables rules to handle cleanup. Most of these cases are handled by the STL classes such as ``std::unique_ptr``, ``std::shared_ptr``, etc. and the recommended COM smart pointer [[``Microsoft::WRL::ComPtr``|ComPtr]] for Direct3D reference counted objects. One case that isn't so easily managed with existing classes is when you are mapping staging/dynamic Direct3D 11 resources. ``MapGuard`` solves this problem, and is modeled after ``std::lock_mutex``.

    class MapGuard : public D3D11_MAPPED_SUBRESOURCE
    {
    public:
        MapGuard( _In_ ID3D11DeviceContext* context, _In_ ID3D11Resource *resource,
             _In_ UINT subresource, _In_ D3D11_MAP mapType, _In_ UINT mapFlags );
        ~MapGuard();
        uint8_t* get() const;
        uint8_t* get(size_t slice) const;
        uint8_t* scanline(size_t row) const;
        uint8_t* scanline(size_t slice, size_t row) const;
    }

# Debug object naming
To help track down resource leaks, the Direct3D 11 debug layer allows you to provide ASCII debug names to Direct3D 11 objects. This is done with a specific GUID and the ``SetPrivateData`` method. Since you typically want to exclude this for release builds, it can get somewhat messy to add these to code. The ``SetDebugObjectName`` template greatly simplifies this for static debug name strings.

    SetDebugObjectName(_In_ ID3D11DeviceChild* resource,
        _In_z_ const char (&name)[TNameLength])

For more information on the Direct3D 11 debug layer, see these blog posts:

[Object Naming](http://blogs.msdn.com/b/chuckw/archive/2010/04/15/object-naming.aspx)  
[Direct3D SDK Debug Layer Tricks](http://blogs.msdn.com/b/chuckw/archive/2012/11/30/direct3d-sdk-debug-layer-tricks.aspx)

# Example

    #include <d3d11.h>
    #include <wrl.h>
    #include "DirectXHelpers.h"

    using namespace DirectX;
    using namespace Microsoft::WRL;

    ComPtr<ID3D11Device> device;
    D3D_FEATURE_LEVEL lvl;
    ComPtr<ID3D11DeviceContext> context;
    HRESULT hr = D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_NULL,
        nullptr, 0/*D3D11_CREATE_DEVICE_DEBUG*/, nullptr, 0,
        D3D11_SDK_VERSION, device.GetAddressOf(),
        &lvl, context.GetAddressOf() );
    if ( FAILED(hr) )
       ...

    CD3D11_TEXTURE2D_DESC desc( DXGI_FORMAT_R8G8B8A8_UNORM,
        128, 64, 1, 1, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_WRITE );

    ComPtr<ID3D11Texture2D> tex;
    hr = device->CreateTexture2D( &desc, nullptr, tex.GetAddressOf() );
    if ( FAILED(hr) )
       ...

    SetDebugObjectName( tex.Get(), "MyTexture" );

    MapGuard map( context.Get(), tex.Get(), 0, D3D11_MAP_WRITE, 0 );

    for( size_t j = 0; j < 128; ++j )
    {
        auto ptr = map.scanline(j);
        ...
    }
