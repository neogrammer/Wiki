In this lesson we learn how to use multistream rendering to implement GPU instancing.

# Input assembler

For these tutorial lessons, we've been providing a single stream of vertex data to the *input assembler*. Generally the most efficient rendering is a single vertex buffer with a stride of 16, 32, or 64 bytes, but there are times when arranging the vertex data in such a layout is expensive. The Direct3D Input Assembler can therefore pull vertex information from up to 16 (or 32 with Hardware Feature Level 11 or better) vertex buffers. This provides a lot of freedom in managing your vertex buffers.

For example, if we return to a case from [[Simple rendering]], here is the 'stock' vertex input layout for ``VertexPositionNormalTexture``:

```cpp
const D3D11_INPUT_ELEMENT_DESC c_InputElements[] =
{
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```

This describes a single vertex stream with three elements. We could instead arrange this into three VBs as follows:

```cpp
// Position in VB#0, NORMAL in VB#1, TEXCOORD in VB#2
const D3D11_INPUT_ELEMENT_DESC c_InputElements[] =
{
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```

To render, we'd need to create an InputLayout object using this with our vertex shader, and then bind the vertex buffers to each slot:

```cpp
ID3D11Buffer* vbs[3] = { m_positionVB.Get(), m_normalVB.Get(), m_texcoordVB.Get() };
UINT strides[3] = { sizeof(float) * 3, sizeof(float) * 3, sizeof(float) * 2 };
UINT offsets[3] = {};
context->IASetVertexBuffers(0, 3, vbs, strides, offsets);
```

Note if we are using ``DrawIndexed``, then the same index value is used to retrieve the 'ith' element from each vertex buffer (i.e. there is only one index per vertex, and all VBs must be at least as long as the highest index value).

## Per-vertex vs. Per-instance

In addition to pulling vertex data from multiple streams, the *input assembler* can also 'loop' over some streams to implement a feature called "instancing". Here the same vertex data is drawing multiple times with some per-vertex data changing "once per instance" as it loops over the other data. This allows you to efficiently render a large number of the same object in many locations, such as grass or boulders.

The [[NormalMapEffect]] supports GPU instancing using a per-vertex ``XMFLOAT3X4`` matrix which can include translations, rotations, scales, etc. For example if we were using ``VertexPositionNormalTexture`` vertex data with instancing, we'd create an input layout as follows:

```cpp
// VertexPositionNormalTexture in VB#0, XMFLOAT3X4 in VB#1
const D3D11_INPUT_ELEMENT_DESC c_InputElements[] =
{
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
    { "InstMatrix",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "InstMatrix",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "InstMatrix",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
};
```

Here the first vertex buffer has enough ``VertexPositionNormalTexture`` vertex data for *one* instance, and the second vertex buffer has as many  ``XMFLOAT3X4`` entries as instances.

> GPU instancing is not supported on Direct3D Hardware Feature Level 9.1 or 9.2, and is only partially supported on 9.3. The *DirectX Tool Kit* shaders that support GPU instancing are written for Shader Model 4.0 and require Feature Level 10 or greater.

# Setup
First create a new project using the instructions from the earlier lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Instancing

Start by saving [spnza_bricks_a.dds](https://github.com/Microsoft/DirectXTK/wiki/media/spnza_bricks_a.DDS),
[spnza_bricks_a_normal.dds](https://github.com/Microsoft/DirectXTK/wiki/media/spnza_bricks_a_normal.DDS),
[spnza_bricks_a_specular.dds](https://github.com/Microsoft/DirectXTK/wiki/media/spnza_bricks_a_specular.DDS) into your new project's directory, and then from the top menu select **Project / Add Existing Item....** Select "spnza_bricks_a.dds" and click "OK". Repeat for the two other files.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;

std::unique_ptr<DirectX::NormalMapEffect> m_effect;
std::unique_ptr<DirectX::GeometricPrimitive> m_shape;

Microsoft::WRL::ComPtr<ID3D11InputLayout> m_instanceLayout;
Microsoft::WRL::ComPtr<ID3D11Buffer> m_instancedVB;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_brickDiffuse;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_brickNormal;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_brickSpecular;

UINT m_instanceCount;
std::unique_ptr<DirectX::XMFLOAT3X4[]> m_instanceTransforms;
```

In **Game.cpp** file, modify the **Game** constructor to initialize the new variable:

```cpp
Game::Game() noexcept(false) :
    m_instanceCount(0)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
auto context = m_deviceResources->GetD3DDeviceContext();
m_shape = GeometricPrimitive::CreateSphere(context);

DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"spnza_bricks_a.DDS",
    nullptr, m_brickDiffuse.ReleaseAndGetAddressOf()));

DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"spnza_bricks_a_normal.DDS",
    nullptr, m_brickNormal.ReleaseAndGetAddressOf()));

DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"spnza_bricks_a_specular.DDS",
    nullptr, m_brickSpecular.ReleaseAndGetAddressOf()));

m_effect = std::make_unique<NormalMapEffect>(device);
m_effect->EnableDefaultLighting();
m_effect->SetTexture(m_brickDiffuse.Get());
m_effect->SetNormalTexture(m_brickNormal.Get());
m_effect->SetSpecularTexture(m_brickSpecular.Get());
m_effect->SetInstancingEnabled(true);

const D3D11_INPUT_ELEMENT_DESC c_InputElements[] =
{
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
    { "InstMatrix",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "InstMatrix",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "InstMatrix",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
};

DX::ThrowIfFailed(
    CreateInputLayoutFromEffect(device, m_effect.get(),
        c_InputElements, std::size(c_InputElements),
        m_instanceLayout.ReleaseAndGetAddressOf()));

// Create instance transforms.
{
    size_t j = 0;
    for (float y = -6.f; y < 6.f; y += 1.5f)
    {
        for (float x = -6.f; x < 6.f; x += 1.5f)
        {
            ++j;
        }
    }
    m_instanceCount = static_cast<UINT>(j);

    m_instanceTransforms = std::make_unique<XMFLOAT3X4[]>(j);

    constexpr XMFLOAT3X4 s_identity = { 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f };

    j = 0;
    for (float y = -6.f; y < 6.f; y += 1.5f)
    {
        for (float x = -6.f; x < 6.f; x += 1.5f)
        {
            m_instanceTransforms[j] = s_identity;
            m_instanceTransforms[j]._14 = x;
            m_instanceTransforms[j]._24 = y;
            ++j;
        }
    }

    auto desc = CD3D11_BUFFER_DESC(
        static_cast<UINT>(j * sizeof(XMFLOAT3X4)),
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_WRITE);

    D3D11_SUBRESOURCE_DATA initData = { m_instanceTransforms.get(), 0, 0 };

    DX::ThrowIfFailed(
        device->CreateBuffer(&desc, &initData,
            m_instancedVB.ReleaseAndGetAddressOf())
    );
}
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_view = Matrix::CreateLookAt(Vector3(0.f, 0.f, 12.f),
    Vector3::Zero, Vector3::UnitY);
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(size.right) / float(size.bottom), 0.1f, 25.f);

m_effect->SetView(m_view);
m_effect->SetProjection(m_proj);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_effect.reset();
m_shape.reset();
m_instanceLayout.Reset();
m_instancedVB.Reset();
m_brickDiffuse.Reset();
m_brickNormal.Reset();
m_brickSpecular.Reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
m_shape->DrawInstanced(m_effect.get(), m_instanceLayout.Get(), m_instanceCount, false, false, 0, [=]()
    {
        UINT stride = sizeof(XMFLOAT3X4);
        UINT offset = 0;
        context->IASetVertexBuffers(1, 1, m_instancedVB.GetAddressOf(), &stride, &offset);
    });
```

Build and run to see 64 spheres rendered.

![Screenshot of many sphere](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotInstancing.PNG)

# Adding animation

You can provide updated instance data per-frame to animate the locations, while leaving the original vertex data 'static' for best performance.

In **Game.cpp**, add to the TODO of **Update**:

```cpp
auto time = static_cast<float>(m_timer.GetTotalSeconds());

size_t j = 0;
for (float y = -6.f; y < 6.f; y += 1.5f)
{
    for (float x = -6.f; x < 6.f; x += 1.5f)
    {
        XMMATRIX m = XMMatrixTranslation(x,
            y,
            cos(time + float(x) * XM_PIDIV4)
            * sin(time + float(y) * XM_PIDIV4)
            * 2.f);
        XMStoreFloat3x4(&m_instanceTransforms[j], m);
        ++j;
    }
}

assert(j == m_instanceCount);
```

In **Game.cpp**, add to the TODO of **Render** prior to the call to ``DrawInstanced``:

```cpp
{
    MapGuard map(context, m_instancedVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0);
    memcpy(map.pData, m_instanceTransforms.get(),
        m_instanceCount * sizeof(XMFLOAT3X4));
}
```

> It's important that the ``map`` be in it's own scope since it's an RAII helper class which needs to get destructed before the call to ``DrawInstanced``.

Build and run to see the spheres moving individually.

![Screenshot of many moving sphere](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotInstancing2.PNG)

## Technical note

The ``XMFLOAT3X4`` data type is a little different than the other **DirectXMath** / [[SimpleMath]] types used in *DirectX Tool Kit*. It's a column-major transformation matrix. Normally we use row-major in DirectX samples, and when moving to a shader we transpose it as we set it into the Constant Buffer to  fit into HLSL's default of column-major.

The ``XMFLOAT3X4`` data type was introduced in [DirectXMath version 3.13](https://walbourn.github.io/directxmath-3-13/) to support DirectX Raytracing which used this column-major form in the API. It's a compact way to encode a '4x3' matrix where the last column is ``0, 0, 0, 1`` (i.e. a matrix which can encode affine 3D transformations like translation, scale, and rotation but *not* perspective projection). We transpose it to column-major and it fits neatly into three ``XMVECTOR`` values as the last row is implicitly ``0 0 0 1``. As you see above, we build a row-major ``XMMATRIX`` transformation matrix and then you can use ``XMStoreFloat3x4`` which will transpose it as it's written to the buffer.

If manipulating ``XMFLOAT3X4`` directly, the translation is in ``_14``, ``_24``, ``_34``.

> The skinning shaders use the same trick of encoding the bone transformation matrices as three ``XMVECTOR`` values each in the column-major form to allow bones to be packed into smaller space (72 bones instead of 54 in the same amount of space), but the API deals with the transpose when you set the array of ``XMMATRIX`` values. Since the instancing data is driven by the application, it doesn't make sense to try to hide this detail.

# Instanced Models

To render **Model** with instancing, you must use custom drawing to update the effect(s) to support instancing, create the proper input layout objects, as well as call ``DrawInstanced`` for each **ModelMeshPart**.

Here's a basic outline of implementing this:

```cpp
// Loading a model and adding instancing.
fxFactory->SetSharing(false);
instancedModel = Model::CreateFromSDKMESH(device, L"mymodel.sdkmesh", fxFactory);

// mymodel.sdkmesh must have vertex normals, vertex texture coordinates, and
// define normal maps in the materials so it uses NormalMapEffect or PBREffect.

static const D3D11_INPUT_ELEMENT_DESC s_instElements[] =
{
    // XMFLOAT3X4
    { "InstMatrix",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "InstMatrix",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "InstMatrix",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
};

instancedModel->UpdateEffects([&](IEffect* effect)
{
    auto nmap = dynamic_cast<NormalMapEffect*>(effect);
    if (nmap)
    {
        nmap->SetInstancingEnabled(true);
    }
    auto pbr = dynamic_cast<PBREffect*>(effect);
    if (pbr)
    {
        pbr->SetInstancingEnabled(true);
    }
});

for (const auto& mit : instancedModel->meshes)
{
    auto mesh = mit.get();
    assert(mesh != 0);

    for (const auto& it : mesh->meshParts)
    {
        auto part = it.get();
        assert(part != 0);
        auto il = *part->vbDecl;
        il.push_back(s_instElements[0]);
        il.push_back(s_instElements[1]);
        il.push_back(s_instElements[2]);
        DX::ThrowIfFailed(
            CreateInputLayoutFromEffect(device, part->effect.get(),
                il.data(), il.size(),
                part->inputLayout.ReleaseAndGetAddressOf()));
    }
}
```

```cpp
// Drawing the model with instancing
UINT stride = sizeof(XMFLOAT3X4);
UINT offset = 0;
context->IASetVertexBuffers(1, 1, m_instancedVB.GetAddressOf(), &stride, &offset);

for (const auto& mit : instancedModel->meshes)
{
    auto mesh = mit.get();
    assert(mesh != nullptr);

    mesh->PrepareForRendering(context, *m_states.get());

    for (const auto& it = mesh->meshParts)
    {
        auto part = it.get();
        assert(part != 0);

        auto imatrices = dynamic_cast<IEffectMatrices*>(part->effect.get());
        if (imatrices)
        {
            imatrices->SetMatrices(local, m_view, m_projection);
        }

        part->DrawInstanced(context, part->effect.get(), part->inputLayout.Get(), m_instanceCount);
    }
}
```

> The drawing above assumes an opaque model. If you have alpha-blending, you will need to do the drawing loop twice. See [[ModelMesh]] for details.

# More to explore

* GPU instancing is also supported by [[DebugEffect]] and [[PBREffect]]

* While **BasicEffect** does not support instancing, you can use **NormalMapEffect** to emulate **BasicEffect** by providing texture coordinates. The **NormalMapEffect** class will use a 
[default 1x1 white](https://github.com/Microsoft/DirectXTK/wiki/media/default.dds) texture (i.e. ``DXGI_FORMAT_R8G8B8A8_UNORM`` value ``0xFFFFFFFF``) and/or a [smooth 1x1 normal map](https://github.com/Microsoft/DirectXTK/wiki/media/smoothMap.dds) texture (i.e. ``DXGI_FORMAT_R8G8_UNORM`` value ``0x7F7F``).

**Next lessons:** [[Creating custom shaders with DGSL]]
