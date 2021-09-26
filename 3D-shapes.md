This lesson draws simple shapes in 3D.

# Setup
First create a new project using the instructions from the earlier lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Background
In the previous lesson, we generated geometry with code using [[PrimitiveBatch]] to draw simple shapes. Here we make use of **GeometricPrimitive** which procedurally generates shapes like spheres, cubes, etc. These 3D shapes are more efficient to render because they make use of indexed primitives, and because they make use of a static rather than dynamic *vertex buffer* and *index buffer*.

# Drawing a sphere

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::SimpleMath::Matrix m_world;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;

std::unique_ptr<DirectX::GeometricPrimitive> m_shape;
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
auto context = m_deviceResources->GetD3DDeviceContext();
m_shape = GeometricPrimitive::CreateSphere(context);

m_world = Matrix::Identity;
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
    Vector3::Zero, Vector3::UnitY);
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(size.right) / float(size.bottom), 0.1f, 10.f);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_shape.reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
m_shape->Draw(m_world, m_view, m_proj);
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
auto time = static_cast<float>(timer.GetTotalSeconds());

m_world = Matrix::CreateRotationZ(cosf(time) * 2.f);
```

Build and run, and you'll see a white lit sphere.

![Screenshot Sphere](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotSphere.PNG)

# Drawing other built-in shapes

In **Game.cpp** modify the TODO of **CreateDeviceDependentResources**:

```cpp
m_shape = GeometricPrimitive::CreateTorus(context);
```

Build and run to see a torus instead of a sphere.

![Screenshot Torus](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTorus.PNG)

You can try out other shapes like a cube, cone, cylinder, dodecahedron, or the classic Utah teapot.

```cpp
m_shape = GeometricPrimitive::CreateCube(context);

m_shape = GeometricPrimitive::CreateCone(context);

m_shape = GeometricPrimitive::CreateCylinder(context);

m_shape = GeometricPrimitive::CreateDodecahedron(context);

m_shape = GeometricPrimitive::CreateTeapot(context);
```

# Adding textures to 3D shapes

Start by saving [earth.bmp](https://github.com/Microsoft/DirectXTK/wiki/images/earth.bmp) into your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "earth.bmp" and click "OK".

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

```cpp
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
DX::ThrowIfFailed(
    CreateWICTextureFromFile(device, L"earth.bmp", nullptr,
    m_texture.ReleaseAndGetAddressOf()));
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_texture.Reset();
```

In **Game.cpp** modify the TODO of **CreateDeviceDependentResources**:

```cpp
m_shape = GeometricPrimitive::CreateSphere(context);
```

In **Game.cpp**, modify to the TODO of **Render**:

```cpp
m_shape->Draw(m_world, m_view, m_proj, Colors::White, m_texture.Get());
```

In **Game.cpp**, modify to the TODO of **Update**:

```cpp
auto time = static_cast<float>(timer.GetTotalSeconds());

m_world = Matrix::CreateRotationY(time);
```

Build and you'll see planet earth spinning.

![Screenshot Earth](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotEarth1.PNG)

# Using custom lighting and effects

By default the geometric primitive renderer uses a simple [[BasicEffect]] with default lighting settings. To get more control over the rendering, you can use your own effect.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::BasicEffect> m_effect;
Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
```

In **Game.cpp** modify the TODO of **CreateDeviceDependentResources**:

```cpp
m_effect = std::make_unique<BasicEffect>(device);
m_effect->SetTextureEnabled(true);
m_effect->SetPerPixelLighting(true);
m_effect->SetLightingEnabled(true);
m_effect->SetLightEnabled(0, true);
m_effect->SetLightDiffuseColor(0, Colors::White);
m_effect->SetLightDirection(0, -Vector3::UnitZ);

m_shape = GeometricPrimitive::CreateSphere(context);
m_shape->CreateInputLayout(m_effect.get(),
    m_inputLayout.ReleaseAndGetAddressOf());

DX::ThrowIfFailed(
   CreateWICTextureFromFile(device, L"earth.bmp", nullptr,
        m_texture.ReleaseAndGetAddressOf()));

m_effect->SetTexture(m_texture.Get());

m_world = Matrix::Identity;
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
m_effect->SetView(m_view);
m_effect->SetProjection(m_proj);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_effect.reset();
m_inputLayout.Reset();
```

In **Game.cpp**, modify to the TODO of **Render**:

```cpp
m_effect->SetWorld(m_world);
m_shape->Draw(m_effect.get(), m_inputLayout.Get());
```

Build and run to see earth with more 'space-like' lighting.

![Screenshot Earth](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotEarth2.PNG)

# More to explore

* The ``GeometricPrimitive`` class is designed for simple rendering, so it always uses the ``VertexPositionNormalTexture`` vertex format. For details on how to utilize the built-in shape generation for other vertex formats, see [[Custom vertex format|GeometricPrimitive#custom-vertex-format]].

* Using a custom shader, you can use the basic box or sphere primitive as a "skybox". See [[Authoring an Effect]].

* Methods are provided to get access to the shape data as ``std::vector``s of ``VertexType`` and ``uint16_t``, and ``GeometricPrimitive`` also allows you to create drawable instances from such data. See [[Custom geometry|GeometricPrimitive#custom-geometry]]

**Next lesson:** [[Rendering a model]]

# Further reading

DirectX Tool Kit docs [[Effects]], [[GeometricPrimitive]]
