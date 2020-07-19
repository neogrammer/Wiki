Here we learn how to render a triangle.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Background
In order to do a draw operation with Direct3D 11, we need to provide the following objects and settings:

* A *vertex buffer* containing the vertices of the elements to draw.
* The *input layout* that [describes](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_input_element_desc) the memory layout of the vertices in the _vertex buffer_.
* A *primitive topology* setting that indicates how to interpret the individual vertices (as a point, a line, a triangle, etc.)
* A compiled *vertex shader* program
* A compiled *pixel shader* program
* Set any required *state objects* for [rasterizer state](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_rasterizer_desc), [depth/stencil state](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_depth_stencil_desc), [blend state](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_blend_desc), and [sampler state](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_sampler_desc) (if using textures)

For this lesson, the [[BasicEffect]] object will provide the vertex and pixel shader programs, [[VertexPositionColor|VertexTypes]] will provide the input layout, [[CommonStates]] will provide the 'stock' state objects, and **PrimitiveBatch** will provide the vertex buffer and primitive topology.

Note that since the _input layout_ is the bridge between the _vertex buffer_ data and the _vertex shader program_, this Direct3D object is created with information about both.

# Drawing a triangle
In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
using VertexType = DirectX::VertexPositionColor;

std::unique_ptr<DirectX::CommonStates> m_states;
std::unique_ptr<DirectX::BasicEffect> m_effect;
std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;
Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
```

In **Game.cpp**, add to the TODO of **CreateDevice**:

```cpp
m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

m_effect = std::make_unique<BasicEffect>(m_d3dDevice.Get());
m_effect->SetVertexColorEnabled(true);

void const* shaderByteCode;
size_t byteCodeLength;

m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

DX::ThrowIfFailed(
        m_d3dDevice->CreateInputLayout(VertexType::InputElements,
            VertexType::InputElementCount,
            shaderByteCode, byteCodeLength,
            m_inputLayout.ReleaseAndGetAddressOf()));

m_batch = std::make_unique<PrimitiveBatch<VertexType>>(m_d3dContext.Get());
```

*Optional*: You can simplify the last part using [[DirectXHelpers]]:

```cpp
m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

m_effect = std::make_unique<BasicEffect>(m_d3dDevice.Get());
m_effect->SetVertexColorEnabled(true);

DX::ThrowIfFailed(
    CreateInputLayoutFromEffect<VertexType>(m_d3dDevice.Get(), m_effect.get(),
        m_inputLayout.ReleaseAndGetAddressOf()
    );

m_batch = std::make_unique<PrimitiveBatch<VertexType>>(m_d3dContext.Get());
```

> *Technical note*: The input layout object needs to contain all the correct per-vertex elements needed for drawing. Therefore, it is important that you configure the ``BasicEffect`` before you call ``GetVertexShaderBytecode`` so it can return the proper shader. Hence why we called ``SetVertexColorEnabled`` to ensure we get ``SV_Position`` and ``COLOR``. See the [[BasicEffect]] for more information on the various shader/input layout configurations.
 
In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_states.reset();
m_effect.reset();
m_batch.reset();
m_inputLayout.Reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
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
```

Build and run to see a simple yellow triangle rendered in 2D.

![Screenshot of triangle](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTriangle.PNG)

> You don't have to use a type alias here like ``VertexType`` and you can just use ``DirectX::VertexPositionColor`` for the header and ``VertexPositionColor`` in the cpp file directly. I use the alias to simplify the tutorial a bit later on.

## Pixel vs. normalized coordinates
The image above is drawn using coordinates that are independent of the screen resolution and range from ``-1`` to ``+1``.  Resizing the window will result in the same image scaled to the new window. If instead you want to draw using screen pixel coordinates (which match the coordinate system used by [[SpriteBatch]]), then:

In **Game.cpp**, add to the TODO of **CreateResources**:

```cpp
Matrix proj = Matrix::CreateScale( 2.f/float(backBufferWidth),
   -2.f/float(backBufferHeight), 1.f)
   * Matrix::CreateTranslation( -1.f, 1.f, 0.f );
m_effect->SetProjection(proj);
```

> The projection matrix can also be created with ``Matrix::CreateOrthographicOffCenter(0.f, float(backBufferWidth), float(backBufferHeight), 0.f, 0.f, 1.f);``

In **Game.cpp**, modify the TODO of **Render**:

```cpp
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
```

Build and run to get the same image, but if you resize the window the triangle will not change in the second version *if* your window size is 800 by 600.

## Technical notes
* The BasicEffect family of shader classes uses shader code built in to the ``DirectXTK.lib`` as static data so there's no need to compile shaders at runtime or to load data files from disk.
* Internally, both SpriteBatch and PrimitiveBatch make use of a _dynamic_ rather than _static_ vertex buffer object which makes use of special memory shared between the CPU and GPU. Generally, we prefer when possible to use static vertex buffers as they can reside in the video memory directly that is only accessible by the GPU.
* Be sure to call ``SetVertexColorEnabled`` before calling ``GetVertexShaderBytecode`` as this impacts which shader permutation is actually used for rendering. See [[IEffect]] for more details.
* Since we haven't used any textures yet, we don't need to use any sampler state objects.

# State Objects

The use of ``CullNone`` for our rasterizer state above allows triangles and quads--which in Direct3D are just two triangles--to be drawn with arbitrary winding order. If you modify **Render** above as follows:

```cpp
m_d3dContext->RSSetState( m_states->CullClockwise() );
```

Then build & run you run you will see nothing drawn because the triangle winding order was specified in clockwise order. If you changed it again to:

```cpp
m_d3dContext->RSSetState( m_states->CullCounterClockwise() );
```

Then build & run you will see the triangle reappear.

For 'closed' objects, you typically use [backface culling](https://en.wikipedia.org/wiki/Back-face_culling) to speed up rendering which can quickly reject triangles that are not facing the viewer and avoids the need to run the pixel shader for those pixels.

> The culling mode does not affect points or lines.

# Drawing with textures

Start by saving [rocks.jpg](https://github.com/Microsoft/DirectXTK/wiki/media/rocks.jpg) into your project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "rocks.jpg" and click "OK".

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

```cpp
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
```

In **Game.cpp**, add to the TODO of **CreateDevice**:

```cpp
DX::ThrowIfFailed(
    CreateWICTextureFromFile(m_d3dDevice.Get(), L"rocks.jpg", nullptr,
    m_texture.ReleaseAndGetAddressOf()));
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_texture.Reset();
```

Build and run to make sure the texture loads fine. Nothing else should be different yet.

Now go back to your **Game.h** and modify the ``VertexType`` alias we used earlier to use [[VertexPositionTexture|VertexTypes]].

```cpp
using VertexType = DirectX::VertexPositionTexture;
```

Then in **Game.cpp** modify **CreateDevice**:

```cpp
...

m_effect = std::make_unique<BasicEffect>(m_d3dDevice.Get());
m_effect->SetTextureEnabled(true);

// Make sure you called CreateWICTextureFromFile before this point!
m_effect->SetTexture(m_texture.Get());

void const* shaderByteCode;
size_t byteCodeLength;
...
```

> *Technical note*: Again, the input layout object needs to contain all the correct per-vertex elements needed for drawing. We do not use any per-vertex color so we do not call ``SetVertexColorEnabled`` and instead we call ``SetTextureEnabled``. The resulting input layout includes ``SV_Position`` and ``TEXCOORD0``. You can associate the texture SRV at any point, but we do it here for convenience.

In **Game.cpp**, modify the TODO of **Render**:

```cpp
m_d3dContext->OMSetBlendState( m_states->Opaque(), nullptr, 0xFFFFFFFF );
m_d3dContext->OMSetDepthStencilState( m_states->DepthNone(), 0 );
m_d3dContext->RSSetState( m_states->CullNone() );

m_effect->Apply(m_d3dContext.Get());

auto sampler = m_states->LinearClamp();
m_d3dContext->PSSetSamplers(0, 1, &sampler);

m_d3dContext->IASetInputLayout(m_inputLayout.Get());

m_batch->Begin();

VertexPositionTexture v1(Vector3(400.f, 150.f, 0.f), Vector2(.5f, 0));
VertexPositionTexture v2(Vector3(600.f, 450.f, 0.f), Vector2(1, 1));
VertexPositionTexture v3(Vector3(200.f, 450.f, 0.f), Vector2(0, 1));

m_batch->DrawTriangle(v1, v2, v3);

m_batch->End();
```

Build and run to see a simple textured triangle rendered in 2D.

![Screenshot of textured triangle](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTriangleTextured.PNG)

## Technical notes
* Because we are using a texture, we have to set a sampler state object. ``BasicEffect`` uses slot 0 of the pixel shader samplers by design, so that's the one we need to set.
* ``PSSetSamplers`` takes an array of pointers to sampler interfaces, which is why we have to create a temporary object ``sampler``.

# Drawing with lighting

Start by saving [rocks_normalmap.dds](https://github.com/Microsoft/DirectXTK/wiki/media/rocks_normalmap.dds) into your project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "rocks_normalmap.dds" and click "OK".

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

```cpp
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMap;
```

In **Game.cpp**, add to the TODO of **CreateDevice**:

```cpp
DX::ThrowIfFailed(
    CreateDDSTextureFromFile(m_d3dDevice.Get(), L"rocks_normalmap.dds", nullptr,
    m_normalMap.ReleaseAndGetAddressOf()));
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_normalMap.Reset();
```

Build and run to make sure this second texture loads fine. Nothing else should be different yet.

Now go back to your **Game.h** and modify the ``VertexType`` alias we used earlier to use [[VertexPositionNormalTexture|VertexTypes]].

```cpp
using VertexType = DirectX::VertexPositionNormalTexture;
```

Also change the type of effect. Since we are using a flat 2D triangle, the lighting is not going to be very interesting so we are going to add some simple normal mapping to give the texture some definition.

```cpp
std::unique_ptr<DirectX::NormalMapEffect> m_effect;
```

Then in **Game.cpp** modify **CreateDevice**:

```cpp
...

m_effect = std::make_unique<NormalMapEffect>(m_d3dDevice.Get());
m_effect->SetTextureEnabled(true);

// Make sure you called CreateDDSTextureFromFile and CreateWICTextureFromFile before this point!
m_effect->SetTexture(m_texture.Get());
m_effect->SetNormalTexture(m_normalMap.Get());

m_effect->EnableDefaultLighting();
m_effect->SetLightDiffuseColor(0, Colors::Gray);

void const* shaderByteCode;
size_t byteCodeLength;
...
```

> *Technical note*: We change how the input layout is created one more time. This time we want to ensure we have ``SV_Position``, ``NORMAL``, and ``TEXCOORD0``. You can associate the texture and normal map SRVs at any point, so we just do it here for convenience. See the [[NormalMapEffect]] for more information on the various shader/input layout configurations.

Then in **Game.cpp** add to **Update**:

```cpp
auto time = static_cast<float>(m_timer.GetTotalSeconds());

float yaw = time * 0.4f;
float pitch = time * 0.7f;
float roll = time * 1.1f;

auto quat = Quaternion::CreateFromYawPitchRoll(pitch, yaw, roll);

auto light = XMVector3Rotate(g_XMOne, quat);

m_effect->SetLightDirection(0, light);
```

In **Game.cpp**, modify the TODO of **Render**:

```cpp
m_d3dContext->OMSetBlendState( m_states->Opaque(), nullptr, 0xFFFFFFFF );
m_d3dContext->OMSetDepthStencilState( m_states->DepthNone(), 0 );
m_d3dContext->RSSetState( m_states->CullNone() );

m_effect->Apply(m_d3dContext.Get());

auto sampler = m_states->LinearClamp();
m_d3dContext->PSSetSamplers(0, 1, &sampler);

m_d3dContext->IASetInputLayout(m_inputLayout.Get());

m_batch->Begin();

VertexPositionNormalTexture v1(Vector3(400.f, 150.f, 0.f), -Vector3::UnitZ, Vector2(.5f, 0));
VertexPositionNormalTexture v2(Vector3(600.f, 450.f, 0.f), -Vector3::UnitZ, Vector2(1, 1));
VertexPositionNormalTexture v3(Vector3(200.f, 450.f, 0.f), -Vector3::UnitZ, Vector2(0, 1));

m_batch->DrawTriangle(v1, v2, v3);

m_batch->End();
```

Build and run, and you'll see the 2D triangle drawn with dynamic lighting effects.

![Screenshot of lit triangle](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTriangleLit.PNG)

## Technical notes
* The tangent-space normal map used here was generated from a [height map](https://github.com/Microsoft/DirectXTK/wiki/media/rocks_NM_height.dds) using [texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv)'s ``-nmap`` feature.

```
texconv rocks_NM_height.dds -nmap l -nmapamp 4
```

* Note this last part of the lesson requires Direct3D [hardware feature level](https://walbourn.github.io/direct3d-feature-levels/) 10.0 or better hardware.

**Next lesson:** [[Line drawing and anti-aliasing]]

# Further reading

DirectX Tool Kit docs [[CommonStates]], [[Effects]], [[PrimitiveBatch]], [[VertexTypes]]  
