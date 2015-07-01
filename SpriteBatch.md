This is a native Direct3D 11 implementation of the SpriteBatch helper from [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.spritebatch.aspx), providing the same functionality and API.

![Sprite Batch Example](https://github.com/Microsoft/DirectXTK/wiki/images/SpriteBatchExample.png)

**Related tutorials:** [[Sprites and textures]], [[More tricks with sprites]]

# Header
    #include <SpriteBatch.h>

# Initialization
The SpriteBatch class requires a Direct3D 11 device context for drawing. 

    std::unique_ptr<SpriteBatch> spriteBatch(new SpriteBatch(deviceContext));

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Simple drawing

    spriteBatch->Begin();
    spriteBatch->Draw(texture, XMFLOAT2(x, y));
    spriteBatch->End();

The **Draw** method has many overloads with parameters controlling:

* Specify screen position as XMFLOAT2, XMVECTOR or RECT
* Optional source rectangle for drawing just part of a sprite sheet
* Tint color
* Rotation (in radians)
* Origin point (position, scaling and rotation are relative to this)
* Scale
* SpriteEffects enum (for horizontal or vertical mirroring)
* Layer depth (for sorting)

# Sorting
The first parameter to ``SpriteBatch::Begin`` is a SpriteSortMode enum. For  most efficient rendering, use ``SpriteSortMode_Deferred`` (which batches up sprites, then submits them all to the GPU during the End call), and manually draw everything in texture order. If it is not possible to draw in texture order, the second most efficient approach is to use ``SpriteSortMode_Texture``, which will automatically sort on your behalf.

When drawing scenes with multiple depth layers, ``SpriteSortMode_BackToFront`` or ``SpriteSortMode_FrontToBack`` will sort by the layerDepth parameter specified to each ``Draw`` call.

``SpriteSortMode_Immediate`` disables all batching, submitting a separate Direct3D draw call for each sprite. This is expensive, but convenient in rare cases  when you need to set shader constants differently per sprite.

Multiple SpriteBatch instances are lightweight. It is reasonable to  create several, Begin them at the same time with different sort modes, submit sprites to different batches in arbitrary orders as you traverse a scene, then End the batches in whatever order you want these groups of sprites to be drawn.

[SpriteBatch and SpriteSortMode](http://blogs.msdn.com/b/shawnhar/archive/2006/12/13/spritebatch-and-spritesortmode.aspx)  
[SpriteBatch sorting part 2](http://blogs.msdn.com/b/shawnhar/archive/2006/12/14/spritebatch-sorting-part-2.aspx)  
[Return of the SpriteBatch: sorting part 3](http://blogs.msdn.com/b/shawnhar/archive/2006/12/14/return-of-the-spritebatch-sorting-part-3.aspx)  
[SpriteSortMode.Immediate in XNA Game Studio 4.0](http://blogs.msdn.com/b/shawnhar/archive/2010/04/05/spritesortmode-immediate-in-xna-game-studio-4-0.aspx)

# Alpha blending
Alpha blending defaults to using premultiplied alpha. To make use of 'straight' alpha textures, provide a blend state object to Begin.

    CommonStates states(device);

    spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied() );

*Note:* If you need to provide custom blend factors or sample mask, use the setCustomShaders callback to call the device context's ``OMSetBlendState`` directly instead.

# Custom render states
By default SpriteBatch uses premultiplied alpha blending, no depth buffer, counter clockwise culling, and linear filtering with clamp texture addressing. You can change this by passing custom state objects to ``SpriteBatch::Begin``. Pass null for any parameters that should use their default value.

    spriteBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, nullptr, nullptr, [=]
    {
       deviceContext->...
    });

# Custom pixel shaders
To use SpriteBatch with a custom pixel shader (handy for 2D postprocessing effects such as bloom or blur), use the setCustomShaders parameter to specify a state setting callback function:

    spriteBatch->Begin(SpriteSortMode_Immediate,
        nullptr, nullptr, nullptr, nullptr, [=]
    {
        deviceContext->PSSetShader(...);
        deviceContext->PSSetConstantBuffers(...);
        deviceContext->PSSetShaderResources(...);
    });

SpriteBatch automatically sets pixel shader resource #0 and sampler slot 0 for the texture specified by each ``Draw`` call, so you only need to call ``PSSetResources`` for any additional textures required by your shader.

To write a custom sprite batch pixel shader in HLSL, make sure it matches the following signature

    Texture2D<float4> Texture : register(t0);
    sampler TextureSampler : register(s0);

    float4 MySpritePixelShader(float4 color : COLOR0,
        float2 texCoord : TEXCOORD0) : SV_Target0
    {
        // TODO
    }

# Custom vertex shaders

To use a custom vertex shader with SpriteBatch, you'll need to create an input layout for your shader that uses VertexPositionColorTexture.

* The vertex position x, y coordinates are generated from position, rotation, origin, and scale ``SpriteBatch::Draw`` parameters.
* The vertex position z coordinate is the layerDepth ``SpriteBatch::Draw`` parameter, which defaults to 0.
* The vertex u, v coordinates are generated.
* The vertex color is the tint color ``SpriteBatch::Draw`` parameter, which defaults to White (1,1,1,1).

Here's an example using [[Effects]].

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

_Remember that the pixel shader input signature must match the vertex shader output signature, so it's easiest to provide both if you are going to override the standard sprite vertex shader._

# Custom transform

``SpriteBatch::Begin`` also has a _transformMatrix_ parameter, which can be used for global transforms such as scaling or translation of an entire scene. It otherwise defaults to matrix identity.

    XMMATRIX matrix = ...;
    spriteBatch->Begin(SpriteSortMode_Deferred, nullptr,
        nullptr, nullptr, nullptr, nullptr, matrix );

This transformation parameter only works if you are using the standard sprite batch vertex shader.

The full transformation depends on the orientation setting and/or the viewport setting for the device. The viewport is obtained from the ``SpriteBatch::SetViewport`` method or by calling ``RSGetViewports`` on the current context, and then used to construct a view matrix based on the setting provided by the``SpriteBatch::SetRotation`` method.

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
    {code:c++}

# Custom clipping
If custom clipping of the sprites is desired (such as clipping font strings), you can achieve this by first creating a custom raster state with scissor rectangle culling enabled:

    ID3D11RasterizerState* scissorState = nullptr;
    CD3D11_RASTERIZER_DESC rsDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
        0, 0.f, 0.f, TRUE, TRUE, TRUE, FALSE);
    if (FAILED(device->CreateRasterizerState(&rsDesc, &scissorState)))
        // error

Then when you call ``Begin`` provide a custom callback to set the scissor rectangle:

    spriteBatch->Begin(SpriteSortMode_Deferred,
        nullptr, nullptr, nullptr, scissorState,
        [&]()
        {
            CD3D11_RECT r( /* pixel rectangle coordinates here */);
            context->RSSetScissorRects(1, &r);
        });
 
# Applications
The same set of techniques that were used with XNA Game Studio's SpriteBatch can be adapted to the DirectXTK implementation.

[Rendering 2D with Sprites and Text](http://msdn.microsoft.com/en-us/library/ff634505.aspx)

## Animated sprites
The [[AnimatedTexture]] class demonstrates drawing an animated sprite as discussed in [this C# tutorial](http://msdn.microsoft.com/en-us/library/bb203866.aspx)

## Scrolling background
The [[ScrollingBackground]] class demonstrates drawing a scrolling background as discussed in [this C# tutorial](http://msdn.microsoft.com/en-us/library/bb203868.aspx)

## Sprite Sheet
The [[SpriteSheet]] class demonstrates drawing sprites from a sprite sheet (aka "texture atlas") to more efficiently use texture memory.

## Post processing effects
The techniques described for XNA Game Studio will work with the C++ SpriteBatch.

[Bloom on Windows Phone](http://blogs.msdn.com/b/shawnhar/archive/2012/01/19/bloom-on-windows-phone.aspx)  
[Postprocessing effects on WP7, Part I](http://www.catalinzima.com/2012/10/postprocessing-effects-on-wp7-part-i/)  
[Postprocessing effects on WP7, Part II](http://www.catalinzima.com/2012/10/postprocessing-effects-on-wp7-part-ii/)  
[Postprocessing effects on WP7, Part III](http://www.catalinzima.com/2012/10/postprocessing-effects-on-wp7-part-iii/)  

## Normal-mapped sprites
The techniques described in the [SpriteEffects](http://xbox.create.msdn.com/en-US/education/catalog/sample/sprite_effects) sample for XNA Game Studio also work with the C++ SpriteBatch.

# Feature Level Notes
The implementation in SpriteBatch uses dynamic vertex buffers with 4 vertices per sprite, which works on all feature levels. The submission maximum batch size (2048) is within the limits of Feature Level 9.1's requirement to use 16-bit indices (``DXGI_FORMAT_R16_UINT``) and the maximum primitives per batch limit of 65535. This is robust and works well for the typical scale of usage for sprite batch.

For more extreme usage scenarios (large particle systems, star fields, etc.), writing a custom sprite implementation is likely the better solution over using SpriteBatch. With Feature Level 9.3, you can make use of instancing, and on Feature Level 10.0+ you can make use of the [geometry shader to implement point-sprites](http://code.msdn.microsoft.com/Direct3D-sprite-sample-2db07382)

# Threading model
Creation is fully asynchronous, so you can instantiate multiple SpriteBatch instances at the same time on different threads. Each SpriteBatch instance only supports drawing from one thread at a time, but you can simultaneously submit sprites on multiple threads if you create a separate SpriteBatch instance per Direct3D 11 deferred context.

[Immediate and Deferred Rendering](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476892.aspx)

# Orientation
For phones, laptops, and tablets the orientation of the display can be changed by the user. For Windows Store apps, DirectX applications are encouraged to handle the rotation internally rather than relying on DXGI's auto-rotation handling.

In older versions of DirectXTK, you had to handle orientation changes via the custom transform matrix on Begin(). In the latest version of DirectXTK, you can handle it via a rotation setting (which is applied after any custom transformation).

## Windows Store apps for Windows 8
Using the DirectX starting template, you will want to add to your ``CreateWindowSizeDependentResources`` method:

    DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;
    switch (m_orientation)
    {
    case DisplayOrientations::Landscape:
        rotation = DXGI_MODE_ROTATION_IDENTITY;
        break;
    case DisplayOrientations::Portrait:
        rotation = DXGI_MODE_ROTATION_ROTATE270;
        break;
    case DisplayOrientations::LandscapeFlipped:
        rotation = DXGI_MODE_ROTATION_ROTATE180;
        break;
    case DisplayOrientations::PortraitFlipped:
        rotation = DXGI_MODE_ROTATION_ROTATE90;
        break;
    }
    spriteBatch->SetRotation( rotation );

## Windows phone 8
For Windows phone 8.0 applications, you do something very similar to Windows Store apps for Windows 8, although you will need to add the support to the DirectX template as described in [this post](http://www.catalinzima.com/2012/12/handling-orientation-in-a-windows-phone-8-game/).

    DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;
    switch (m_orientation)
    {
    case DisplayOrientations::Portrait:
        rotation = DXGI_MODE_ROTATION_IDENTITY;
        break;
    case DisplayOrientations::Landscape:
        rotation = DXGI_MODE_ROTATION_ROTATE90;
        break;
    case DisplayOrientations::PortraitFlipped:
        rotation = DXGI_MODE_ROTATION_ROTATE180;
        break;
    case DisplayOrientations::LandscapeFlipped:
        rotation = DXGI_MODE_ROTATION_ROTATE270;
        break;
    }
    spriteBatch->SetRotation( rotation );

## Windows Store apps for Windows 8.1
Using the DirectX starting template, you will want to add to your ``CreateWindowSizeDependentResources`` method:

    spriteBatch->SetRotation( m_deviceResources->ComputeDisplayRotation() );

_In ``Common\DeviceResources.h``, you need to make ``ComputeDisplayRotation`` a public function instead of being private._

# Further reading
[ShawnHar's blog on SpriteBatch](http://www.shawnhargreaves.com/blogindex.html#spritebatch)  
[ShawnHar's blog on Premultiplied Alpha](http://www.shawnhargreaves.com/blogindex.html#premultipliedalpha)    
[SpriteBatch and renderstates in XNA Game Studio 4.0](http://blogs.msdn.com/b/shawnhar/archive/2010/06/18/spritebatch-and-renderstates-in-xna-game-studio-4-0.aspx)  

