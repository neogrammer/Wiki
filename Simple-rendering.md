Here we learn how to render a triangle and a grid in 3D.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Background
In order to do a draw operation with Direct3D 11, we need to provide the following objects and settings:

* A *vertex buffer* containing the vertices of the elements to draw.
* The *input layout* that [describes](https://msdn.microsoft.com/en-us/library/windows/desktop/ff476180.aspx) the memory layout of the vertices in the _vertex buffer_.
* A *primitive topology* setting that indicates how to interpret the individual vertices (as a point, a line, a triangle, etc.)
* A compiled *vertex shader* program
* A compiled *pixel shader* program
* Set any required *state objects* for [rasterizer state](https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198.aspx), [depth/stencil state](https://msdn.microsoft.com/en-us/library/windows/desktop/ff476110.aspx), [blend state](https://msdn.microsoft.com/en-us/library/windows/desktop/ff476087.aspx), and [sampler state](https://msdn.microsoft.com/en-us/library/windows/desktop/ff476207.aspx) (if using textures)

For this lesson, the [[BasicEffect]] object will provide the vertex and pixel shader programs, [[VertexPositionColor|VertexTypes]] will provide the input layout, [[CommonStates]] will provide the 'stock' state objects, and **PrimitiveBatch** will provide the vertex buffer and primitive topology.

Note that since the _input layout_ is the bridge between the _vertex buffer_ data and the _vertex shader program_, this Direct3D object is created with information about both.

# Drawing a triangle
In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

    std::unique_ptr<DirectX::CommonStates> m_states;
    std::unique_ptr<DirectX::BasicEffect> m_effect;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

    m_effect = std::make_unique<BasicEffect>(m_d3dDevice.Get());
    m_effect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;

    m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    DX::ThrowIfFailed(
            m_d3dDevice->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_inputLayout.ReleaseAndGetAddressOf()));

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(m_d3dContext.Get());

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_states.reset();
    m_effect.reset();
    m_batch.reset();
    m_inputLayout.Reset();

In **Game.cpp**, add to the TODO of **Render**:

    m_d3dContext->OMSetBlendState( m_states->Opaque(), nullptr, 0xFFFFFFFF );
    m_d3dContext->OMSetDepthStencilState( m_states->DepthNone(), 0 );
    m_d3dContext->RSSetState( m_states->CullNone() );

    m_effect->Apply(m_d3dContext.Get());

    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_batch->Begin();

    VertexPositionColor v1(Vector3(0.f, 0.5f, 0.5f), Colors::Yellow);
    VertexPositionColor v2(Vector3(0.5f, -0.5f, 0.5f), Colors::Yellow);
    VertexPositionColor v3(Vector3(-0.5f, -0.5f, 0.5f), Colors::Yellow);

    m_batch->DrawTriangle(v1, v2, v3);

    m_batch->End();

Build and run to see a simple yellow triangle rendered in 2D.

![Screenshot of triangle](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTriangle.PNG)

## Pixel vs. normalized coordinates
The image above is drawn using coordinates that are independent of the screen resolution and range from ``-1`` to ``+1``.  Resizing the window will result in the same image scaled to the new window. If instead you want to draw using screen pixel coordinates (which match the coordinate system used by [[SpriteBatch]]), then:

In **Game.cpp**, add to the TODO of **CreateResources**:

    Matrix proj = Matrix::CreateScale( 2.f/float(backBufferWidth),
       -2.f/float(backBufferHeight), 1.f)
       * Matrix::CreateTranslation( -1.f, 1.f, 0.f );
    m_effect->SetProjection(proj);

> The projection matrix can also be created with ``Matrix::CreateOrthographicOffCenter(0.f, float(backBufferWidth), float(backBufferHeight), 0.f, 0.f, 1.f);``

In **Game.cpp**, modify the TODO of **Render**:

    m_d3dContext->OMSetBlendState( m_states->Opaque(), nullptr, 0xFFFFFFFF );
    m_d3dContext->OMSetDepthStencilState( m_states->DepthNone(), 0 );
    m_d3dContext->RSSetState( m_states->CullNone() );

    m_effect->Apply(m_d3dContext.Get());

    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_batch->Begin();

    VertexPositionColor v1(Vector3(400.f, 150.f, 0.f), Colors::Yellow);
    VertexPositionColor v2(Vector3(600.f, 450.f, 0.f), Colors::Yellow);
    VertexPositionColor v3(Vector3(200.f, 450.f, 0.f), Colors::Yellow);

    m_batch->DrawTriangle(v1, v2, v3);

    m_batch->End();

Build and run to get the same image, but if you resize the window the triangle will not change in the second version.

## Technical notes
* The BasicEffect family of shader classes uses shader code built in to the ``DirectXTK.lib`` as static data so there's no need to compile shaders at runtime or to load data files from disk.
* Internally, both SpriteBatch and PrimitiveBatch make use of a _dynamic_ rather than _static_ vertex buffer object which makes use of special memory shared between the CPU and GPU. Generally, we prefer when possible to use static vertex buffers as they can reside in the video memory directly that is only accessible by the GPU.
* We are not using textures in our shaders for this lesson, so we don't need to use any sampler state objects.
* Be sure to call ``SetVertexColorEnabled`` before calling ``GetVertexShaderBytecode`` as this impacts which shader permutation is actually used for rendering. See [[IEffect]] for more details.

# State Objects

The use of ``CullNone`` for our rasterizer state above allows triangles and quads--which in Direct3D are just two triangles--to be drawn with arbitrary winding order. If you modify **Render** above as follows:

    m_d3dContext->RSSetState( m_states->CullClockwise() );

Then build & run you run you will see nothing drawn because the triangle winding order was specified in clockwise order. If you changed it again to:

    m_d3dContext->RSSetState( m_states->CullCounterClockwise() );

Then build & run you will see the triangle reappear.

For 'closed' objects, you typically use [backface culling](https://en.wikipedia.org/wiki/Back-face_culling) to speed up rendering which can quickly reject triangles that are not facing the viewer and avoids the need to run the pixel shader for those pixels.

> The culling mode does not affect points or lines.

# Drawing a grid

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

    DirectX::SimpleMath::Matrix m_world;
    DirectX::SimpleMath::Matrix m_view;
    DirectX::SimpleMath::Matrix m_proj;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    m_world = Matrix::Identity;

In **Game.cpp**, add to the TODO of **CreateResources**:

    m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
        Vector3::Zero, Vector3::UnitY);
    m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
        float(backBufferWidth) / float(backBufferHeight), 0.1f, 10.f);

    m_effect->SetView(m_view);
    m_effect->SetProjection(m_proj);

In **Game.cpp**, modify to the TODO of **Render**:

    m_d3dContext->OMSetBlendState( m_states->Opaque(), nullptr, 0xFFFFFFFF );
    m_d3dContext->OMSetDepthStencilState( m_states->DepthNone(), 0 );
    m_d3dContext->RSSetState( m_states->CullNone() );

    m_effect->SetWorld(m_world);

    m_effect->Apply(m_d3dContext.Get());

    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_batch->Begin();

    Vector3 xaxis(2.f, 0.f, 0.f);
    Vector3 yaxis(0.f, 0.f, 2.f);
    Vector3 origin = Vector3::Zero;

    size_t divisions = 20;

    for( size_t i = 0; i <= divisions; ++i )
    {
        float fPercent = float(i) / float(divisions);
        fPercent = ( fPercent * 2.0f ) - 1.0f;

        Vector3 scale = xaxis * fPercent + origin;

        VertexPositionColor v1( scale - yaxis, Colors::White );
        VertexPositionColor v2( scale + yaxis, Colors::White );
        m_batch->DrawLine( v1, v2 );
    }

    for( size_t i = 0; i <= divisions; i++ )
    {
        float fPercent = float(i) / float(divisions);
        fPercent = ( fPercent * 2.0f ) - 1.0f;

        Vector3 scale = yaxis * fPercent + origin;

        VertexPositionColor v1( scale - xaxis, Colors::White );
        VertexPositionColor v2( scale + xaxis, Colors::White );
        m_batch->DrawLine( v1, v2 );
    }

    m_batch->End();

Build and run to see a 3D grid.

![Screenshot of the grid](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotGrid.PNG)

# Anti-aliasing

Taking a closer look at the grid in the previous screenshot, you can see the lines are a little thin and jagged in places. To make this more visible, in **Game.cpp**, add to the TODO of **Update**:

    m_world = Matrix::CreateRotationY( cosf( static_cast<float>(timer.GetTotalSeconds())));

Build and run to see the grid spinning, and notice the slight shimmering of the lines--it will be more obvious if you maximize the window size.    

There are two approaches to addressing this problem, known as [aliasing](https://en.wikipedia.org/wiki/Aliasing).

## Anti-aliased lines
The first is to enable a special anti-aliasing mode specific to line drawing in Direct3D.

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, TRUE);

    DX::ThrowIfFailed(m_d3dDevice->CreateRasterizerState( &rastDesc,
        m_raster.ReleaseAndGetAddressOf() ));

> This creates a raster state that is the same as our standard ``CullNone`` but with ``AntialiasedLineEnable`` set to TRUE and ``MultisampleEnable`` set to FALSE.

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_raster.Reset();

In **Game.cpp**, modify **Render**, changing:

    m_d3dContext->RSSetState(m_states->CullNone());

to
 
    m_d3dContext->RSSetState(m_raster.Get());

Build and run to see the shimmering of the lines lessen, although they will appear to be a bit thicker than a single pixel.

![Screenshot of the AA grid](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotGridAA.PNG)

## Multisampling
A second more general solution is to use [Multisample anti-aliasing](https://en.wikipedia.org/wiki/Multisample_anti-aliasing) (MSAA) which uses more video memory and pixel-fill performance to achieve higher quality rendering results. In this case, we will make use of 4x MSAA where the render target and the depth buffer will be 4 times larger. MSAA can be used with all primitives, not just lines.

In **Game.cpp**, modify **CreateDevice**:

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

> This creates a raster state that is the same as our standard ``CullNone`` which has ``MultisampleEnable`` set to TRUE.

In **Game.cpp**, modify **CreateResources**:

Change the sample count in both places for ``DXGI_SWAP_CHAIN_DESC1`` and ``DXGI_SWAP_CHAIN_DESC`` 

    swapChainDesc.SampleDesc.Count = 4;

Change the depth/stencil texture description to:

    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat,
        backBufferWidth, backBufferHeight,
        1, 1, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, 4, 0);

Change the depth/stencil view description to:

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);

Build and run to see the shimmering of the lines lessen compared to the first version, and is slightly less thickened than when we used the AA line mode.

![Screenshot of the MSAA grid](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotGridMSAA.PNG)

> _Troubleshooting:_ If you get an exception, it may be because your device does not support 4x MSAA--which you can confirm by checking the debug device output at the failure point. Direct3D Feature Level 10.1 or later devices all support 4x MSAA for most render target formats, but some 10.0 and 9.x feature levels do not. You can try using 2 instead of 4 in the sample counts if this happens. If you have a Feature Level 11.0 or later device, you can try using 4 or 8. Be sure to change `` DXGI_SWAP_CHAIN_DESC1 ``, `` DXGI_SWAP_CHAIN_DESC``, and ``depthStencilDesc``. See also [CheckMultisampleQualityLevels](https://msdn.microsoft.com/en-us/library/windows/desktop/ff476499.aspx).

### Technical Note
The ability to create an MSAA DXGI swap chain is only supported for the older "bit-blt" style presentation modes, specifically ``DXGI_SWAP_EFFECT_DISCARD`` or ``DXGI_SWAP_EFFECT_SEQUENTIAL``. The newer "flip" style presentation modes ``DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL`` or ``DXGI_SWAP_EFFECT_FLIP_DISCARD`` required for Universal Windows Platform (UWP) apps don't support creating MSAA swap chains. Instead, you create your own MSAA render target and then explicitly resolve from MSAA to single-sample via ``ResolveSubresource`` to the DXGI back-buffer for presentation. See [Multisampling in Universal Windows Platform (UWP) apps](https://msdn.microsoft.com/en-us/windows/uwp/gaming/multisampling--multi-sample-anti-aliasing--in-windows-store-apps) and the [SimpleMSAA](https://github.com/Microsoft/Xbox-ATG-Samples/tree/master/UWPSamples/IntroGraphics/SimpleMSAA_UWP) sample which details how you implement MSAA in these cases.

# More to explore

* PrimitiveBatch is ideally suited for drawing debug displays such as visualizing bounding volumes, collision data, etc. For more on this, see [[DebugDraw]].

**Next lesson:** [[3D shapes]]

# Further reading

DirectX Tool Kit docs [[CommonStates]], [[Effects]], [[PrimitiveBatch]], [[VertexTypes]]  
[FXAA](https://en.wikipedia.org/wiki/Fast_approximate_anti-aliasing)  
[SMAA](http://www.iryoku.com/smaa/)  
[Spatial anti-alising](https://en.wikipedia.org/wiki/Spatial_anti-aliasing)
