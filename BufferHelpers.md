Helpers functions for creating Direct3D resources from CPU data.

# Header
```cpp
#include <BufferHelpers.h>
```

# Creating vertex/index buffers

The ``CreateStaticBuffer`` helper is used to create Direct3D buffer type resources such as vertex buffers or index buffers.


```cpp
HRESULT CreateStaticBuffer(ID3D11Device* device,
    const void* ptr, size_t count, size_t stride,
    D3D11_BIND_FLAG bindFlags,
    ID3D11Buffer** pBuffer);

template<typename T>
HRESULT CreateStaticBuffer(ID3D11Device* device,
    T const* data,
    size_t count,
    D3D11_BIND_FLAG bindFlags,
    ID3D11Buffer** pBuffer);

template<typename T>
HRESULT CreateStaticBuffer(ID3D11Device* device,
    T const& data,
    D3D11_BIND_FLAG bindFlags,
    ID3D11Buffer** pBuffer);
```

## Examples

```cpp
static const VertexPositionColor s_vertexData[3] =
{
    { XMFLOAT3{ 0.0f,   0.5f,  0.5f }, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f } },  // Top / Red
    { XMFLOAT3{ 0.5f,  -0.5f,  0.5f }, XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f } },  // Right / Green
    { XMFLOAT3{ -0.5f, -0.5f,  0.5f }, XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f } }   // Left / Blue
};

ComPtr<ID3D11Buffer> vb;
DX::ThrowIfFailed(
  CreateStaticBuffer(device,
    s_vertexData,                // const void *ptr
    _countof(s_vertexData),      // size_t count
    sizeof(VertexPositionColor), // size_t stride
    D3D11_BIND_VERTEX_BUFFER, &vb)
  );
```

When creating from an array using [[VertexTypes]], a template simplifies this to:

```cpp
ComPtr<ID3D11Buffer> vb;
DX::ThrowIfFailed(
  CreateStaticBuffer(device, s_vertexData, _countof(s_vertexData),
    D3D11_BIND_VERTEX_BUFFER, &vb)
  );
```

Finally, you can create from C++ containers like ``std::vector``:

```cpp
std::vector<uint16_t> indices;

...

ComPtr<ID3D11Buffer> ib;
DX::ThrowIfFailed(
  CreateStaticBuffer(device, indices, D3D11_BIND_INDEX_BUFFER, &ib)
  );
```

See [[Model]] for creating vertex buffers/index buffers from disk files.

# Creating textures

The ``CreateTextureFromMemory`` helpers can be used to create 1D, 2D, or 3D Direct3D texture resources. For the 2D version, you can enable auto-generation of mipmaps.

```cpp
HRESULT CreateTextureFromMemory(ID3D11Device* device,
    size_t width,
    DXGI_FORMAT format,
    const D3D11_SUBRESOURCE_DATA& initData,
    ID3D11Texture1D** texture,
    ID3D11ShaderResourceView** textureView,
    D3D11_BIND_FLAG bindFlags = D3D11_BIND_SHADER_RESOURCE);

HRESULT CreateTextureFromMemory(ID3D11Device* device,
    size_t width, size_t height,
    DXGI_FORMAT format,
    const D3D11_SUBRESOURCE_DATA& initData,
    ID3D11Texture2D** texture,
    ID3D11ShaderResourceView** textureView,
    D3D11_BIND_FLAG bindFlags = D3D11_BIND_SHADER_RESOURCE);

HRESULT  CreateTextureFromMemory(ID3D11Device* device,
    ID3D11DeviceContext* d3dContext,
    size_t width, size_t height,
    DXGI_FORMAT format,
    const D3D11_SUBRESOURCE_DATA& initData,
    ID3D11Texture2D** texture,
    ID3D11ShaderResourceView** textureView);

HRESULT CreateTextureFromMemory(ID3D11Device* device,
    size_t width, size_t height, size_t depth,
    DXGI_FORMAT format,
    const D3D11_SUBRESOURCE_DATA& initData,
    ID3D11Texture3D** texture,
    ID3D11ShaderResourceView** textureView,
    D3D11_BIND_FLAG bindFlags = D3D11_BIND_SHADER_RESOURCE);
```

## Examples

To create a 'default' texture (i.e. a 1x1 white pixel):

```cpp
const uint32_t s_pixel = 0xffffffff;

D3D11_SUBRESOURCE_DATA initData = { &s_pixels, sizeof(uint32_t), 0 };

ComPtr<ID3D11ShaderResourceView> defaultTex;
DX::ThrowIfFailed(
    CreateTextureFromMemory(device, 1u, 1u, DXGI_FORMAT_R8G8B8A8_UNORM, initData,
        nullptr, &defaultTex)
    );
```

See [[DDSTextureLoader]] and [[WICTextureLoader]] for creating textures from disk files.

# Constant buffers

The **ConstantBuffer** template class is a strongly-typed helper for managing constant buffers.

```cpp
struct MyCB
{
    XMMATRIX world;
};

static_assert((sizeof(MyCB) % 16) == 0, "CB size not padded correctly");

ConstantBuffer<MyCB> cbuffer;

...

cbuffer.Create(device);

...


MyCB updates = { ... };
cbuffer.SetData(deviceContext, updates);

auto buffer = cbuffer.GetBuffer();

deviceContext->VSSetConstantBuffers(0, 1, &buffer);
deviceContext->PSSetConstantBuffers(0, 1, &buffer);
```

For Xbox One XDK, the ``ConstantBuffer`` class supports "11.X fast semantics", requiring a slightly different calling pattern when rendering. Each time you call ``SetData`` the [[GraphicsMemory]] class is used to allocate video memory.

```cpp
void *grfxMemory;
cbuffer.SetData(deviceContext, constants, &grfxMemory);

auto buffer = cbuffer.GetBuffer();

deviceContextX->VSSetPlacementConstantBuffer(0, buffer, grfxMemory);
deviceContextX->PSSetPlacementConstantBuffer(0, buffer, grfxMemory);
```
