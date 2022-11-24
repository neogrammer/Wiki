|[[DirectXTK]]|
|---|

Contains various C++ utilities that simplify Direct3D 11 programming.

# Header
```cpp
#include <DirectXHelpers.h>
```

# Direct3D 11 "core" helpers
The standard ``d3d11_1.h`` headers in the Windows 8.x / 10 SDK already include a number of helper classes intended to simplify usage of the API when using C++. These are all in the global C++ namespace.

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

See [Microsoft Docs](https://docs.microsoft.com/windows/win32/direct3d11/cd3d11-helper-classes) for more information on these helpers.

> These helpers are excluded for C language builds or if the preprocessor define ``D3D11_NO_HELPERS`` is set.

> There are some additional helpers related to DirectX 11 Video not covered here that are excluded by the preprocessor define ``D3D11_VIDEO_NO_HELPERS``.

> The MinGW compiler Direct3D 11 headers are missing `CD3D11_VIEWPORT` and `CD3D11_DEPTH_STENCIL_VIEW_DESC` along with `D3D11_MIN_DEPTH` and `D3D11_MAX_DEPTH`.

# Exception-safe Direct3D 11 resource locking
Modern C++ development strongly encourages use of the [RAII](http://wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern for exception-safe resource management, ensuring that resources are properly cleaned up if C++ exceptions are thrown. Even without exception handling, it's generally cleaner code to use RAII and rely on the C++ scope variables rules to handle cleanup. Most of these cases are handled by the STL classes such as ``std::unique_ptr``, ``std::shared_ptr``, etc. and the recommended COM smart pointer [[``Microsoft::WRL::ComPtr``|ComPtr]] for Direct3D reference counted objects. One case that isn't so easily managed with existing classes is when you are mapping staging/dynamic Direct3D 11 resources. ``MapGuard`` solves this problem, and is modeled after ``std::lock_mutex``.

```cpp
class MapGuard : public D3D11_MAPPED_SUBRESOURCE
{
public:
    MapGuard( ID3D11DeviceContext* context, ID3D11Resource* resource,
        unsigned int subresource, D3D11_MAP mapType,
        unsigned int mapFlags );
    ~MapGuard();
    uint8_t* get() const;
    uint8_t* get(size_t slice) const;
    uint8_t* scanline(size_t row) const;
    uint8_t* scanline(size_t slice, size_t row) const;
}
```

# Create input layout helper

The **CreateInputLayoutFromEffect** helper is used to create an input layout from the shader blob within an ``IEffect``.

```cpp
static const D3D11_INPUT_ELEMENT_DESC s_inputElementDesc[2] =
{
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA,  0 },
    { "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA , 0 },
};

ComPtr<ID3D11InputLayout> il;
DX::ThrowIfFailed(
  CreateInputLayoutFromEffect(device, effect, s_inputElementDesc, std::size(s_inputElementDesc), &il)
)
```

A template form is convenient when used in combination with [[VertexTypes]]:

```cpp
ComPtr<ID3D11InputLayout> il;
DX::ThrowIfFailed(
  CreateInputLayoutFromEffect<VertexPositionColor>(device, effect, &il)
)
```

# Misc helpers

* **IsPowerOf2**: Tests if a value a power of 2.
* **AlignDown**: Aligns a value down to the next power of 2.
* **AlignUp**: Aligns a value up to the next power of 2.

# Debug object naming
To help track down resource leaks, the Direct3D 11 debug layer allows you to provide ASCII debug names to Direct3D 11 objects. This is done with a specific GUID and the ``SetPrivateData`` method. Since you typically want to exclude this for release builds, it can get somewhat messy to add these to code. The ``SetDebugObjectName`` template greatly simplifies this for static debug name strings.

```cpp
SetDebugObjectName(ID3D11DeviceChild* resource, const char (&name)[TNameLength]);
```

For more information on the Direct3D 11 debug layer, see these blog posts:

[Object Naming](https://walbourn.github.io/object-naming/)  
[Direct3D SDK Debug Layer Tricks](https://walbourn.github.io/direct3d-sdk-debug-layer-tricks/)

# Example

```cpp
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

MapGuard map( context.Get(), tex.Get(), 0, MAP_WRITE_DISCARD, 0 );

for( size_t j = 0; j < 128; ++j )
{
    auto ptr = map.scanline(j);
    ...
}
```


# Xbox-specific

A number of DirectX APIs use the COM parameter pattern of ``(..., REFIID riid, LPVOID *ppv)``. In these cases, the ``IID_PPV_ARGS`` macro is often employed such as the following:

```cpp
using Microsoft::WRL::ComPtr;

ComPtr<ID3D11Device2> device2;

hr = device->QueryInterface(IID_PPV_ARGS(&device2));
```

The ``IID_PPV_ARGS`` macro works on any type that's derived from ``IUnknown``.

For Xbox One XDK development, however, the Direct3D 11.X Runtime variant does not make use of the standard ``IUnknown`` interface (this is referred to as the 'monolithic' runtime vs. the Windows-style 'stock' runtime). This provides significant software optimizations for the fixed-hardware nature of the console, and generally is client-code compatible. The one exception is that this breaks ``IID_PPV_ARGS``. Therefore, the Direct3D 11.X headers include a ``IID_GRAPHICS_PPV_ARGS`` macro which works on the variant DirectX APIs in the same way.

In order to simplify coding that is portable to both 'stock' PC and 'monolithic' Xbox, all usage of ``IID_PPV_ARGS`` within the library is replaced with ``IID_GRAPHICS_PPV_ARGS`` for graphics APIs. The ``DirectXHelpers.h`` provides an simple macro that maps ``IID_GRAPHICS_PPV_ARGS`` to ``IID_PPV_ARGS`` for non-Xbox platforms. This helper is available for client code as well.

> Note that ``IID_PPV_ARGS`` is still used on Xbox for COM APIs like the Windows Imaging Component (WIC).

