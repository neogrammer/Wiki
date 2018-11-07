This is a helper for easily and efficiently drawing dynamically generated geometry using Direct3D 11 such as lines or trianges.

> Dynamic submission is a highly effective pattern for drawing procedural geometry, and convenient for debug rendering, but is not nearly as efficient as static buffers which is more suited to traditional meshes where the VBs and IBs do not change every frame. Excessive dynamic submission is a common source of performance problems in apps. Therefore, you should prefer to use [[Model]], [[GeometricPrimitive]], or your own VB/IB over PrimitiveBatch unless you really need the flexibility to regenerate the topology every frame.

PrimitiveBatch manages the vertex and index buffers for you, using *DISCARD* and *NO_OVERWRITE* hints to avoid stalling the GPU pipeline. It automatically merges adjacent draw requests, so if you call DrawLine 100 times in a row, only a single GPU draw call will be generated.

PrimitiveBatch is responsible for setting the vertex buffer, index buffer, and primitive topology, then issuing the final draw call. Unlike the higher level [[SpriteBatch]] helper, it does not provide shaders, set the input layout, or set any state objects. PrimitiveBatch is often used in conjunction with [[BasicEffect|Effects]] and the structures from [[VertexTypes]], but it can work with any other shader or vertex formats of your own.

> PrimitiveBatch fills the same role as the legacy Direct3D 9 APIs ``DrawPrimitiveUP`` and ``DrawIndexedPrimitiveUP``.

**Related tutorial:** [[Simple rendering]]

# Header
```cpp
#include <PrimitiveBatch.h>
```

# Initialization

Initialize a PrimitiveBatch for drawing ``VertexPositionColor`` data

```cpp
std::unique_ptr<PrimitiveBatch<VertexPositionColor>> primitiveBatch;
primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(deviceContext);
```

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

The default values assume that your maximum batch size is 2048 vertices arranged in triangles. If you want to use larger batches, you need to provide the additional constructor parameters (be sure to review the Feature Level limitations below).

```cpp
PrimitiveBatch<T>( ID3D11DeviceContext* deviceContext,
    size_t maxIndices = DefaultBatchSize * 3,
    size_t maxVertices = DefaultBatchSize)
```

# Effect and Input Layout

Setting up a suitable BasicEffect and input layout:

```cpp
std::unique_ptr<BasicEffect> basicEffect;
ComPtr<ID3D11InputLayout> inputLayout;

basicEffect = std::make_unique<BasicEffect>(device);

basicEffect->SetProjection(XMMatrixOrthographicOffCenterRH(0,
    screenWidth, screenHeight, 0, 0, 1));
basicEffect->SetVertexColorEnabled(true);

void const* shaderByteCode;
size_t byteCodeLength;

basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

device->CreateInputLayout(VertexPositionColor::InputElements,
                          VertexPositionColor::InputElementCount,
                          shaderByteCode, byteCodeLength,
                          inputLayout.GetAddressOf() );
```

# Drawing

```cpp
m_states = std::make_unique<CommonStates>(device);

deviceContext->OMSetBlendState( m_states->Opaque(), nullptr, 0xFFFFFFFF );
deviceContext->OMSetDepthStencilState( m_states->DepthNone(), 0 );
deviceContext->RSSetState( m_states->CullNone() );

basicEffect->Apply(deviceContext);
deviceContext->IASetInputLayout(inputLayout.Get());

primitiveBatch->Begin();
primitiveBatch->DrawLine(VertexPositionColor(...), VertexPositionColor(...));
primitiveBatch->End();
```

PrimitiveBatch provides five drawing methods:

* **DrawLine**(v1, 2): Draws a single-pixel line between two vertices
* **DrawTriangle**(v1, v2, v3): Draws a triangle between three vertices
* **DrawQuad**(v1, v2, v3, v4): draws a quad from four corner vertices (_submitted as two triangles_)
* **Draw**(topology, vertices, vertexCount): Draws an array of vertices with the given topology
* **DrawIndexed**(topology, indices, indexCount, vertices, vertexCount): Draws an indexed array of vertices with a given topology.

# Optimization

For best performance, draw as much as possible inside the fewest separate Begin/End blocks. This will reduce overhead and maximize potential for batching.

Ideally submit draws of the same topology to avoid flushing, and preferably use ``D3D_PRIMITIVE_TOPOLOGY_POINTLIST``, ``D3D_PRIMITIVE_TOPOLOGY_LINELIST``, or ``D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST``.

The PrimitiveBatch constructor allows you to specify what size index and vertex buffers to allocate. You may want to tweak these values to fit your workload, or if you only intend to draw non-indexed geometry, specify maxIndices = 0 to entirely skip creating the index buffer.

# Draw order

Until ``End`` is called on PrimitiveBatch, the various ``Draw`` statements are likely still buffered. They are always drawn in the order of the individual ``Draw`` statements, but if mixing PrimitiveBatch drawing with other drawing, you need to call ``End`` before they will all be submitted to Direct3D.

# Applications

Primitive batch is best used for dynamic submission, which is commonly needed for things like drawing debug shapes. See [[DebugDraw]].

# Feature Level Notes

In order to support [all feature levels](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-devices-downlevel-intro), PrimitiveBatch only supports 16-bit indices (``DXGI_FORMAT_R16_UINT``) which limits to a maximum of 65535 addressable vertices. This does not apply to non-indexed drawing when the PrimitiveBatch constructor is called with a maxIndices of 0.

Keep in mind there is a feature-level based limit on the maximum number of primitives in a single draw call, so the overall batch size needs to be under this limit. To support all feature levels, this should be 65535 or less lines or triangles in a single 'draw batch'.

[Direct3D Feature Levels](http://blogs.msdn.com/b/chuckw/archive/2012/06/20/direct3d-feature-levels.aspx)

# Threading model

Each PrimitiveBatch instance only supports drawing from one thread at a time, but you can simultaneously submit primitives on multiple threads if you create a separate PrimitiveBatch instance per Direct3D 11 deferred context.

[Immediate and Deferred Rendering](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-render-multi-thread-render)

# State management
PrimitiveBatch does not save and restore existing state. For efficiency, it simply sets the state it requires to render and assumes that any subsequent rendering after ``primitiveBatch->End()`` will overwrite state that it needs.

PrimitiveBatch makes use of the following states:

* Index buffer
* Primitive topology
* Vertex buffer (slot 0)

The PrimitiveBatch class assumes you've already set the Render Target view, Depth Stencil view, Viewport, BlendState, Constant buffers, DepthStencilState, Input layout, Pixel shader, RasterizerState, Vertex shader, and any required SamplerState or Shader resources.

> Be sure that if you set any of the following shaders prior to using ``PrimitiveBatch`` that you clear them: Geometry Shader, Hull Shader, Domain Shader, Compute Shader.

# Further Reading
[DirectXTK PrimitiveBatch helper makes it easy to draw user primitives with D3D11](http://blogs.msdn.com/b/shawnhar/archive/2012/10/12/directxtk-primitivebatch-helper-makes-it-easy-to-draw-user-primitives-with-d3d11.aspx)  
[How to: Use dynamic resources](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/how-to--use-dynamic-resources)  
