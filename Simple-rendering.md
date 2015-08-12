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

For this lesson, the [[BasicEffect]] object will provide the vertex and pixel shader programs, [[VertexPositionColor|VertexTypes]] will provide the input layout, and **PrimitiveBatch** will provide the vertex buffer and primitive topology.

Note that since the _input layout_ is the bridge between the _vertex buffer_ data and the _vertex shader program_, this Direct3D object is created with information about both.

# Drawing a triangle
In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

    std::unique_ptr<DirectX::CommonStates> m_states;
    std::unique_ptr<DirectX::BasicEffect> m_effect;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    m_states.reset(new CommonStates(m_d3dDevice.Get()));

    m_effect.reset(new BasicEffect(m_d3dDevice.Get()));
    m_effect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;

    m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    DX::ThrowIfFailed(
            m_d3dDevice->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_inputLayout.ReleaseAndGetAddressOf()));

    m_batch.reset(new PrimitiveBatch<VertexPositionColor>(m_d3dContext.Get()));

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

The image above is drawn using device coordinates that are independent of the screen resolution and range from ``-1`` to ``+1``.  Resizing the window will result in the same image scaled to the new window. If instead you want to draw using screen pixel coordinates (which match the coordinate system used by [[SpriteBatch]]), then:

In **Game.cpp**, add to the TODO of **CreateResources**:

    Matrix proj = Matrix::CreateScale( 2.f/float(backBufferWidth),
       -2.f/float(backBufferHeight), 1.f)
       * Matrix::CreateTranslation( -1.f, 1.f, 0.f );
    m_effect->SetProjection(proj);

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

# State Objects

The use of ``CullNone`` for our rasterizer state above allows triangles and quads--which in Direct3D are just two triangles--to be drawn with arbitrary winding order. If you modify **Render** above as follows:

    m_d3dContext->RSSetState( m_states->CullClockwise() );

Then build & run you run you will see nothing drawn because the triangle winding order was specified in clockwise order. If you changed it again to:

    m_d3dContext->RSSetState( m_states->CullCounterClockwise() );

Then build & run you will see the triangle reappear.

For 'closed' objects, you typically use *backface culling* to speed up rendering which can quickly reject triangles that are not facing the viewer and avoids the need to run the pixel shader for those pixels.

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

![Screenshot of triangle](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotGrid.PNG)

**Next lesson:** [[3D shapes]]

# Further reading

DirectX Tool Kit docs [[CommonStates]], [[Effects]], [[PrimitiveBatch]], [[VertexTypes]]

