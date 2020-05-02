This lesson covers creating custom shaders with the Visual Studio DGSL Shader Designer and using them with _DirectX Tool Kit_.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Creating custom shaders using DGSL
One approach to creating your own shader is to use a visual designer tool, such as Visual Studio's DGSL Shader Designer. In this tool, the vertex shader is 'fixed' and the visual tool is used to create the pixel shader. The result of the designer tool is a compiled shader in a ``.DGSL.CSO`` file that can be loaded at runtime. The resulting shaders can be complex and use up to 8 textures at once, perform tangent-space lighting, and many other complex effects.

To use these with _DirectX Tool Kit_, you can manually create [[DGSLEffect]] instances and use them with [[PrimitiveBatch]]. You can also load them automatically from a CMO using Model when you provide the [[DGSLEffectFactory|EffectFactory]] rather than the standard [[EffectFactory]] as we demonstrated in [[Rendering a model]].

# Rendering a sphere with our effect
Save the files [MyDGSLShader.dgsl](https://github.com/Microsoft/DirectXTK/wiki/MyDGSLShader.dgsl), [billard15.dds](https://github.com/Microsoft/DirectXTK/wiki/media/billard15.dds), [envmap.dds](https://github.com/Microsoft/DirectXTK/wiki/media/envmap.dds), [ReadData.h](https://github.com/Microsoft/DirectXTK/wiki/ReadData.h), and [dgslsphere.inc](https://github.com/Microsoft/DirectXTK/wiki/dgslsphere.inc) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item...**. Select "MyDGSLShader.dgsl" and hit "OK".  Repeat for each file.

Right click on your project in the Solution Explorer. Select **Build Dependencies** / **Build Customizations...**. Check "ShaderGraphContentTask..." and hit "Ok".

![Build Customization Settings](https://github.com/Microsoft/DirectXTK/wiki/images/settingsBCS.PNG)

Right-click on the "MyDGSLShader.dgsl" file in the Solution Explorer, select **Properties...**. Set _Item Type_ to "Shader Graph Content Pipeline".

![DGSL Settings](https://github.com/Microsoft/DirectXTK/wiki/images/settingsDGSL.PNG)

In **pch.h** add after the other ``#include`` statements:

```cpp
#include "ReadData.h"
```

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::SimpleMath::Matrix m_world;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;
std::unique_ptr<DirectX::CommonStates> m_states;
std::unique_ptr<DirectX::DGSLEffect> m_effect;
Microsoft::WRL::ComPtr<ID3D11Buffer> m_shapeVB;
Microsoft::WRL::ComPtr<ID3D11Buffer> m_shapeIB;
Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;
Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
```

In **Game.cpp** after the using namespace statements, add:

```cpp
namespace
{
#include "dgslsphere.inc"
}
```

In **Game.cpp** modify in **CreateDevice**:

```cpp
static const D3D_FEATURE_LEVEL featureLevels [] =
{
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
};
```

> **Universal Windows Platform app:** The new list would also include ``D3D_FEATURE_LEVEL_12_0`` and ``D3D_FEATURE_LEVEL_12_1``. The change is to delete ``D3D_FEATURE_LEVEL_9_1``, ``D3D_FEATURE_LEVEL_9_2``, and ``D3D_FEATURE_LEVEL_9_3``.

In **Game.cpp**, add to the TODO of **CreateDevice**:

```cpp
m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

// Create DGSL Effect
auto blob = DX::ReadData( L"MyDGSLShader.cso" );
DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(&blob.front(), blob.size(),
    nullptr, m_pixelShader.ReleaseAndGetAddressOf()));

m_effect = std::make_unique<DGSLEffect>(m_d3dDevice.Get(), m_pixelShader.Get());
m_effect->SetTextureEnabled(true);
m_effect->SetVertexColorEnabled(true);

DX::ThrowIfFailed(
    CreateDDSTextureFromFile(m_d3dDevice.Get(), L"billard15.dds", nullptr,
    m_texture.ReleaseAndGetAddressOf()));

m_effect->SetTexture(m_texture.Get());

DX::ThrowIfFailed(
    CreateDDSTextureFromFile(m_d3dDevice.Get(), L"envmap.dds", nullptr,
    m_texture2.ReleaseAndGetAddressOf()));

m_effect->SetTexture(1, m_texture2.Get());
m_effect->EnableDefaultLighting();

void const* shaderByteCode;
size_t byteCodeLength;

m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

DX::ThrowIfFailed( m_d3dDevice->CreateInputLayout(
    VertexPositionNormalTangentColorTexture::InputElements,
    VertexPositionNormalTangentColorTexture::InputElementCount,
    shaderByteCode, byteCodeLength,
    m_inputLayout.ReleaseAndGetAddressOf()));

// Create sphere geometry with DGSL vertex data
{
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(g_sphereVB);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = g_sphereVB;

    DX::ThrowIfFailed( m_d3dDevice->CreateBuffer( &desc, &initData,
        m_shapeVB.ReleaseAndGetAddressOf() ) );
}

{
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(g_sphereIB);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = g_sphereIB;

    DX::ThrowIfFailed( m_d3dDevice->CreateBuffer( &desc, &initData,
        m_shapeIB.ReleaseAndGetAddressOf() ) );
}

m_world = Matrix::Identity;
```

In **Game.cpp**, add to the TODO of **CreateResources**:

```cpp
m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
    Vector3::Zero, Vector3::UnitY);
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(backBufferWidth) / float(backBufferHeight), 0.1f, 10.f);

m_effect->SetViewport( float(backBufferWidth), float(backBufferHeight) );

m_effect->SetView(m_view);
m_effect->SetProjection(m_proj);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_states.reset();
m_effect.reset();
m_shapeVB.Reset();
m_shapeIB.Reset();
m_inputLayout.Reset();
m_texture.Reset();
m_texture2.Reset();
m_pixelShader.Reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
m_effect->Apply( m_d3dContext.Get() );

auto sampler = m_states->LinearWrap();
m_d3dContext->PSSetSamplers( 0, 1, &sampler );

m_d3dContext->RSSetState( m_states->CullClockwise() );

m_d3dContext->IASetIndexBuffer( m_shapeIB.Get(), DXGI_FORMAT_R16_UINT, 0 );

m_d3dContext->IASetInputLayout( m_inputLayout.Get() );

UINT stride = sizeof(VertexPositionNormalTangentColorTexture);
UINT offset = 0;
m_d3dContext->IASetVertexBuffers(0, 1, m_shapeVB.GetAddressOf(), &stride, &offset);

m_d3dContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

m_d3dContext->DrawIndexed( _countof(g_sphereIB), 0, 0 );
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
float time = float(timer.GetTotalSeconds());

m_effect->SetTime(time);

m_world = Matrix::CreateRotationZ(cosf(time) * 2.f);

m_effect->SetWorld(m_world);
```

Build and run to see the sphere.

![Screenshot of ball](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotBall.PNG)

> _Troubleshooting_: If you get a runtime exception, then you may have the "billard15.dds" or "envmap.dds" in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``CreateDDSTextureFromFile`` and step into the code to find the exact problem. If you get an exception from  ``DX::ReadData``, then you may not have the "MyDGSLShader.dgsl" file building correctly.

## Technical notes
We are not able to use a ``GeometricPrimitive`` because the DGSL rendering pipeline requires per-vertex tangent information. Therefore, we load a custom vertex buffer/index buffer for this lesson that uses the ``VertexPositionNormalTangentColorTexture`` vertex structure with the tangent information. This vertex information is always included with ``CMO`` models.

While the example DGSL here doesn't require it, this lesson shows setting the extra DGSL variables for time and viewport size in the **Update** and **CreateResources** methods.

# Limitations
The main limitation of using the DGSL tool is that it creates only Shader Model 4.0 Pixel Shaders, which requires Direct3D feature level 10.0 or greater. This is why we removed the 9.x feature levels from **CreateDevice** above.

To support Windows phone, you need feature level 9.3, and to target Surface RT you need feature level 9.1. There is a workaround which is to manually export the DGSL shader to an HLSL file, then compile it using FXC with either the ``ps_4_0_level_9_1`` or ``ps_4_0_level_9_3`` shader profile. It is quite likely you'll need to manually simplify the HLSL shader to successfully get it to compile. [[DGSLEffectFactory|EffectFactory]] implements this workaround by looking for a 'base-name' equivalent of the ``.DGSL.CSO`` file as a ``.CSO`` file when on feature level 9.x devices.

[How to: Export a Shader](https://docs.microsoft.com/en-us/visualstudio/designers/how-to-export-a-shader)

**Next lesson:** [[Writing custom shaders]]

# Further reading

DirectX Tool Kit docs [[Effects]]  
[Using 3-D Assets in Your Game or App](https://docs.microsoft.com/en-us/visualstudio/designers/using-3-d-assets-in-your-game-or-app)  
[Working with Shaders](https://docs.microsoft.com/en-us/visualstudio/designers/working-with-shaders)  
[Visual Studio 3D Starter Kit (Windows 8.1)](http://aka.ms/vs3dkitwin)
