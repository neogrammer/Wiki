This is a native Direct3D 11 implementation of the SpriteBatch helper from XNA Game Studio 4 (``Microsoft.Xna.Framework.Graphics.SpriteBatch``), providing the same functionality and API.

![Sprite Batch Example](https://github.com/Microsoft/DirectXTK/wiki/images/SpriteBatchExample.png)

**Related tutorials:** [[Sprites and textures]], [[More tricks with sprites]], [[Writing custom shaders]]

# Header
```cpp
#include <SpriteBatch.h>
```

# Initialization
The SpriteBatch class requires a Direct3D 11 device context for drawing.

```cpp
std::unique_ptr<SpriteBatch> spriteBatch;
spriteBatch = std::make_unique<SpriteBatch>(deviceContext);
```

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Simple drawing

```cpp
spriteBatch->Begin();
spriteBatch->Draw(texture, XMFLOAT2(x, y));
spriteBatch->End();
```

The **Draw** method has many overloads with parameters controlling:

* Specify screen position as ``XMFLOAT2``, ``XMVECTOR`` or ``RECT`` (in units of pixels if using default transformation)
* Optional source rectangle (in units of pixels) for drawing just part of a texture (i.e. a sprite sheet)
* Tint color (defaults to 1,1,1,1 which means 'no tinting')
* Rotation (in radians)
* Origin point (in units of pixels if using the default transformation): position, scaling and rotation are relative to this
* Scale
* SpriteEffects enum (for horizontal or vertical mirroring)
* Layer depth (for sorting)

> The source rectangle is used to compute the texture coordinates for the sprite on the texture. The position, rotation, origin, and scale are used to compute the sprite triangle's drawing location.

# Sorting
The first parameter to ``SpriteBatch::Begin`` is a SpriteSortMode enum. For  most efficient rendering, use ``SpriteSortMode_Deferred`` (which batches up sprites, then submits them all to the GPU during the End call), and manually draw everything in texture order. If it is not possible to draw in texture order, the second most efficient approach is to use ``SpriteSortMode_Texture``, which will automatically sort on your behalf.

When drawing scenes with multiple depth layers, ``SpriteSortMode_BackToFront`` or ``SpriteSortMode_FrontToBack`` will sort by the layerDepth parameter specified to each ``Draw`` call.

``SpriteSortMode_Immediate`` disables all batching, submitting a separate Direct3D draw call for each sprite. This is expensive, but convenient in rare cases  when you need to set shader constants differently per sprite.

Multiple SpriteBatch instances are lightweight. It is reasonable to  create several, Begin them at the same time with different sort modes, submit sprites to different batches in arbitrary orders as you traverse a scene, then End the batches in whatever order you want these groups of sprites to be drawn.

[SpriteBatch and SpriteSortMode](http://www.shawnhargreaves.com/blog/spritebatch-and-spritesortmode.html)  
[SpriteBatch sorting part 2](http://www.shawnhargreaves.com/blog/spritebatch-sorting-part-2.html)  
[Return of the SpriteBatch: sorting part 3](http://www.shawnhargreaves.com/blog/return-of-the-spritebatch-sorting-part-3.html)  
[SpriteSortMode.Immediate in XNA Game Studio 4.0](http://www.shawnhargreaves.com/blog/spritesortmode-immediate-in-xna-game-studio-4-0.html)

# Alpha blending
Alpha blending defaults to using premultiplied alpha. To make use of 'straight' alpha textures, provide a blend state object to Begin.

```cpp
m_states = std::make_unique<CommonStates>(device);

spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied() );
```

> If you need to provide custom blend factors or sample mask, use the *setCustomShaders* callback to call the device context's ``OMSetBlendState`` directly instead.

> [[SpriteFont]] is designed to work with premultiplied alpha, so if you need to draw both text and straight-alpha content, you should submit them in two different ``Begin`` / ``End`` batches. Alternatively, you can use [[MakeSpriteFont]] to generate 'straight-alpha' ``.spritefont`` files using the ``/NoPremultiply`` switch.

# Custom render states
By default SpriteBatch uses premultiplied alpha blending, no depth buffer, counter clockwise culling, and linear filtering with clamp texture addressing. You can change this by passing custom state objects to ``SpriteBatch::Begin``.

```cpp
void Begin(SpriteSortMode sortMode,
    ID3D11BlendState* blendState,
    ID3D11SamplerState* samplerState,
    ID3D11DepthStencilState* depthStencilState,
    ID3D11RasterizerState* rasterizerState,
    std::function<void> setCustomShaders,
    FXMMATRIX transformMatrix);
```

Pass null for any parameters that should use their default value.

```cpp
spriteBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, nullptr, nullptr, [=]
{
   deviceContext->...
});
```

# Custom pixel shaders
To use SpriteBatch with a custom pixel shader (handy for 2D postprocessing effects such as bloom or blur), use the setCustomShaders parameter to specify a state setting callback function:

```cpp
spriteBatch->Begin(SpriteSortMode_Immediate,
    nullptr, nullptr, nullptr, nullptr, [=]
{
    deviceContext->PSSetShader(...);
    deviceContext->PSSetConstantBuffers(...);
    deviceContext->PSSetShaderResources(...);
});
```

SpriteBatch automatically sets pixel shader resource #0 and sampler slot 0 for the texture specified by each ``Draw`` call, so you only need to call ``PSSetResources`` for any additional textures required by your shader.

To write a custom sprite batch pixel shader in HLSL, make sure it matches the following signature

```cpp
Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);

float4 MySpritePixelShader(float4 color : COLOR0,
    float2 texCoord : TEXCOORD0) : SV_Target0
{
    // TODO
}
```

# Custom vertex shaders

To use a custom vertex shader with SpriteBatch, you'll need to create an input layout for your shader that uses VertexPositionColorTexture.

* The vertex position x, y coordinates are generated from position, rotation, origin, and scale ``SpriteBatch::Draw`` parameters.
* The vertex position z coordinate is the layerDepth ``SpriteBatch::Draw`` parameter, which defaults to 0.
* The vertex u, v coordinates are generated.
* The vertex color is the tint color ``SpriteBatch::Draw`` parameter, which defaults to White (1,1,1,1).

Here's an example using [[Effects]].

```cpp
void const* shaderByteCode;
size_t byteCodeLength;

effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

device->CreateInputLayout(VertexPositionColorTexture::InputElements,
                          VertexPositionColorTexture::InputElementCount,
                          shaderByteCode, byteCodeLength,
                          pInputLayout);

...

spriteBatch->Begin(SpriteSortMode_Deferred,
    nullptr, nullptr, nullptr, nullptr, [=]
{
    effect->Apply( deviceContext );
    deviceContext->IASetInputLayout( pInputLayout );
});
```

> Remember that the pixel shader input signature must match the vertex shader output signature, so it's easiest to provide both if you are going to override the standard sprite vertex shader.

# Custom transform

``SpriteBatch::Begin`` also has a _transformMatrix_ parameter, which can be used for global transforms such as scaling or translation of an entire scene. It otherwise defaults to matrix identity.

```cpp
XMMATRIX matrix = ...;
spriteBatch->Begin(SpriteSortMode_Deferred, nullptr,
    nullptr, nullptr, nullptr, nullptr, matrix );
```

This transformation parameter only works if you are using the standard sprite batch vertex shader.

The full transformation depends on the orientation setting and/or the viewport setting for the device. The viewport is obtained from the ``SpriteBatch::SetViewport`` method or by calling ``RSGetViewports`` on the current context, and then used to construct a view matrix based on the setting provided by the``SpriteBatch::SetRotation`` method.

```cpp
// DXGI_MODE_ROTATION_ROTATE90
finalMatrix =
    matrix * XMMATRIX(0, -yScale, 0, 0, -xScale, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1);

// DXGI_MODE_ROTATION_ROTATE270
finalMatrix =
    matrix * XMMATRIX(0, yScale, 0, 0, xScale, 0, 0, 0, 0, 0, 1, 0, -1, -1, 0, 1);

//DXGI_MODE_ROTATION_ROTATE180
finalMatrix =
    matrix * XMMATRIX(-xScale, 0, 0, 0, 0, yScale, 0, 0, 0, 0, 1, 0, 1, -1, 0, 1);

// DXGI_MODE_ROTATION_IDENTITY (the default)
finalMatrix =
    matrix * XMMATRIX(xScale, 0, 0, 0, 0, -yScale, 0, 0, 0, 0, 1, 0, -1, 1, 0, 1);

// DXGI_MODE_ROTATION_UNSPECIFIED
finalMatrix = matrix;
```

# Custom clipping
If custom clipping of the sprites is desired (such as clipping font strings), you can achieve this by first creating a custom raster state with scissor rectangle culling enabled:

```cpp
ID3D11RasterizerState* scissorState = nullptr;
CD3D11_RASTERIZER_DESC rsDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
    0, 0.f, 0.f, TRUE, TRUE, TRUE, FALSE);
if (FAILED(device->CreateRasterizerState(&rsDesc, &scissorState)))
    // error
```

Then when you call ``Begin`` provide a custom callback to set the scissor rectangle:

```cpp
spriteBatch->Begin(SpriteSortMode_Deferred,
    nullptr, nullptr, nullptr, scissorState,
    [&]()
    {
        CD3D11_RECT r( /* pixel rectangle coordinates here */);
        context->RSSetScissorRects(1, &r);
    });
```

# Applications
The same set of techniques that were used with XNA Game Studio's SpriteBatch can be adapted to the DirectXTK implementation.

## Animated sprites
The [[AnimatedTexture]] class demonstrates drawing an animated sprite.

## Scrolling background
The [[ScrollingBackground]] class demonstrates drawing a scrolling background.

## Sprite Sheet
The [[SpriteSheet]] class demonstrates drawing sprites from a sprite sheet (aka "texture atlas") to more efficiently use texture memory.

## Post processing effects
The techniques described for XNA Game Studio will work with the C++ SpriteBatch.

[Bloom on Windows Phone]( http://www.shawnhargreaves.com/blog/bloom-on-windows-phone.html)  
[Postprocessing effects on WP7, Part I](http://www.catalinzima.com/2012/10/postprocessing-effects-on-wp7-part-i/)  
[Postprocessing effects on WP7, Part II](http://www.catalinzima.com/2012/10/postprocessing-effects-on-wp7-part-ii/)  
[Postprocessing effects on WP7, Part III](http://www.catalinzima.com/2012/10/postprocessing-effects-on-wp7-part-iii/)  

> For Direct3D Feature Level 10.0 or better hardware, these are better done using [[PostProcess]]

## Normal-mapped sprites
The techniques described in the ``SpriteEffects`` sample for XNA Game Studio also work with the C++ SpriteBatch.

# Feature Level Notes
The implementation in SpriteBatch uses dynamic vertex buffers with 4 vertices per sprite, which works on all feature levels. The submission maximum batch size (2048) is within the limits of Feature Level 9.1's requirement to use 16-bit indices (``DXGI_FORMAT_R16_UINT``) and the maximum primitives per batch limit of 65535. This is robust and works well for the typical scale of usage for sprite batch.

For more extreme usage scenarios (large particle systems, star fields, etc.), writing a custom sprite implementation is likely the better solution over using SpriteBatch. With Feature Level 9.3, you can make use of instancing, and on Feature Level 10.0+ you can make use of the [geometry shader to implement point-sprites](http://code.msdn.microsoft.com/Direct3D-sprite-sample-2db07382)

# Threading model
Creation is fully asynchronous, so you can instantiate multiple SpriteBatch instances at the same time on different threads. Each SpriteBatch instance only supports drawing from one thread at a time, but you can simultaneously submit sprites on multiple threads if you create a separate SpriteBatch instance per Direct3D 11 deferred context.

[Immediate and Deferred Rendering](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-render-multi-thread-render)

# Orientation
For phones, laptops, and tablets the orientation of the display can be changed by the user. For Windows Store apps, DirectX applications are encouraged to handle the rotation internally rather than relying on DXGI's auto-rotation handling.

In older versions of DirectXTK, you had to handle orientation changes via the custom transform matrix on Begin(). In the latest version of DirectXTK, you can handle it via a rotation setting (which is applied after any custom transformation).

Using the DirectX starting template for Universal Windows Platform (UWP) apps, you will want to add to your ``CreateWindowSizeDependentResources`` method:

```cpp
spriteBatch->SetRotation( m_deviceResources->ComputeDisplayRotation() );
```

> In ``Common\DeviceResources.h``, you need to make ``ComputeDisplayRotation`` a public function instead of being private.

When using the [directx-vs-templates](https://github.com/walbourn/directx-vs-templates) DeviceResources for UWP, use:

```cpp
spriteBatch->SetRotation( m_deviceResources->GetRotation() );
```

# State management
Unlike ``ID3DXSprite``, SpriteBatch does not save and restore existing state. For efficiency, it simply sets the state it requires to render and assumes that any subsequent rendering after ``spriteBatch->End()`` will overwrite state that it needs.

SpriteBatch makes use of the following states:

* BlendState
* Constant buffer (Vertex Shader stage, slot 0)
* DepthStencilState
* Index buffer
* Input layout
* Pixel shader
* Primitive topology
* RasterizerState
* SamplerState (Pixel Shader stage, slot 0)
* Shader resources (Pixel Shader stage, slot 0)
* Vertex buffer (slot 0)
* Vertex shader

The SpriteBatch class assumes you've already set the Render Target view, Depth Stencil view, and Viewport. It will also read the first viewport set on the device unless you've explicitly called ``SetViewport``.

> Be sure that if you set any of the following shaders prior to using ``SpriteBatch`` that you clear them: Geometry Shader, Hull Shader, Domain Shader, Compute Shader.

# Xbox One
When using Direct3D 11.x fast semantics, you must make use of ``SetViewport`` which is otherwise optional as ``RSGetViewports`` is not supported in this mode.

```cpp
spriteBatch->SetViewport(viewPort);
```

# Further reading
[SpriteBatch and BasicEffect for C++ Direct3D 11](http://www.shawnhargreaves.com/blog/spritebatch-and-basiceffect-for-c-direct3d-11.html)

[Premultiplied alpha](http://www.shawnhargreaves.com/blog/premultiplied-alpha.html)  
[Premultiplied alpha and image composition](http://www.shawnhargreaves.com/blog/premultiplied-alpha-and-image-composition.html)  
[Premultiplied alpha in XNA Game Studio 4.0](http://www.shawnhargreaves.com/blog/premultiplied-alpha-in-xna-game-studio-4-0.html)

[SpriteBatch and custom blend modes](http://www.shawnhargreaves.com/blog/spritebatch-and-custom-blend-modes.html)

[Is SpriteBatch Turing complete?](http://www.shawnhargreaves.com/blog/is-spritebatch-turing-complete.html)
