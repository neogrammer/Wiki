In this lesson we learn about other built-in shader types and some of their uses.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Environment mapping

Start by saving [wood.dds](https://github.com/Microsoft/DirectXTK/wiki/wood.DDS) and [cubemap.dds](https://github.com/Microsoft/DirectXTK/wiki/cubemap.dds) into your new project's directory, and then from the top menu select **Project / Add Existing Item....** Select "wood.dds" and click "OK". Repeat for "cubemap.dds"

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

    DirectX::SimpleMath::Matrix m_world;
    DirectX::SimpleMath::Matrix m_view;
    DirectX::SimpleMath::Matrix m_proj;
    std::unique_ptr<DirectX::CommonStates> m_states;
    std::unique_ptr<DirectX::GeometricPrimitive> m_shape;
    std::unique_ptr<DirectX::EnvironmentMapEffect> m_effect;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemap;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

    m_effect = std::make_unique<EnvironmentMapEffect>(m_d3dDevice.Get());
    m_effect->EnableDefaultLighting();

    m_shape = GeometricPrimitive::CreateTeapot(m_d3dContext.Get());
    m_shape->CreateInputLayout(m_effect.get(),
        m_inputLayout.ReleaseAndGetAddressOf());

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(m_d3dDevice.Get(), L"wood.dds", nullptr,
        m_texture.ReleaseAndGetAddressOf()));

    m_effect->SetTexture(m_texture.Get());

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(m_d3dDevice.Get(), L"cubemap.dds", nullptr,
        m_cubemap.ReleaseAndGetAddressOf()));

    m_effect->SetEnvironmentMap(m_cubemap.Get());

    m_world = Matrix::Identity;

In **Game.cpp**, add to the TODO of **CreateResources**:

    m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
        Vector3::Zero, Vector3::UnitY);
    m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
        float(backBufferWidth) / float(backBufferHeight), 0.1f, 10.f);

    m_effect->SetView(m_view);
    m_effect->SetProjection(m_proj);

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_states.reset();
    m_shape.reset();
    m_effect.reset();
    m_inputLayout.Reset();
    m_texture.Reset();
    m_cubemap.Reset();

In **Game.cpp**, add to the TODO of **Render**:

    m_effect->SetWorld(m_world);
    m_shape->Draw(m_effect.get(), m_inputLayout.Get(), false, false, [=]{
        auto sampler = m_states->LinearWrap();
        m_d3dContext->PSSetSamplers( 1, 1, &sampler );
    });

In **Game.cpp**, add to the TODO of **Update**:

    float time = float(timer.GetTotalSeconds());

    m_world = Matrix::CreateRotationZ(cosf(time) * 2.f);

Build and run to see the teapot rendered with a fancy material.

![Screenshot of teapot](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTeapot.PNG)

In **Game.cpp** add the following to the TODO section of **Update**:

    m_effect->SetFresnelFactor(cosf(time * 2.f));

Build and run to see the effect of animating the Fresnel factor.

![Screenshot of teapot](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTeapot2.PNG)

# More to explore

* The [[NormalMapEffect]] is similar to the [[BasicEffect]] with the addition of a normal texture map and an optional specular texture map. This requires the input layout to contain vertex _tangents_ which are included in ``.CMO`` models, can be included when exporting and ``.SDKMESH`` via the [Content Exporter](http://go.microsoft.com/fwlink/?LinkId=226208), or can be computed using [DirectXMesh](https://github.com/Microsoft/DirectXMesh).

* [[DualTextureEffect]] is used to render a material with two textures applied. This requires the input layout to contain a second set of _texture coordinates_. This does not perform vertex or per-pixel lighting, as the second texture is most often a lightmap with statically computed lighting information.  SDKMESH`` and the [Content Exporter](http://go.microsoft.com/fwlink/?LinkId=226208) support exporting light-mapped models which utilize this effect.

* The [[AlphaTestEffect]]

**Next lesson:** [[Creating custom shaders with DGSL]]

# Further reading 

DirectX Tool Kit docs [[Effects]]