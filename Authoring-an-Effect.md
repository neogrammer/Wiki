|[[Getting Started]]|
|---|

This lesson covers writing your own [[IEffect|Effects]] implementation, specifically a custom effect for rendering a [skybox](https://wikipedia.org/wiki/Skybox_%28video_games%29) with a cubemap.

# Setup
First create a new project using the instructions from the previous lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Writing a skybox shader

A skybox is a shape like a sphere or a cube that surrounds the render scene giving the appearance of the distant sky, horizon, and/or surroundings. In our implementation we are going to be using a cubemap (i.e. 6-faced texture) to provide the texture information on the sky. For the geometry, we'll make use of [[GeometricPrimitive]], and the shaders we are using for the **SkyboxEffect** are as follows:

[SkyboxEffect_Common.hlsli](https://github.com/Microsoft/DirectXTK/wiki/shaders/SkyboxEffect_Common.hlsli)
```cpp
cbuffer SkyboxConstants : register(b0)
{
    float4x4 WorldViewProj;
}

struct VSOutput
{
    float3 TexCoord : TEXCOORD0;
    float4 PositionPS : SV_Position;
};
```

[SkyboxEffect_VS.hlsl](https://github.com/Microsoft/DirectXTK/wiki/shaders/SkyboxEffect_VS.hlsl)
```cpp
VSOutput main(float4 position : SV_Position)
{
    VSOutput vout;

    vout.PositionPS = mul(position, WorldViewProj);
    vout.PositionPS.z = vout.PositionPS.w; // Draw on far plane
    vout.TexCoord = position.xyz;

    return vout;
}
```

[SkyboxEffect_PS.hlsl](https://github.com/Microsoft/DirectXTK/wiki/shaders/SkyboxEffect_PS.hlsl)
```cpp
TextureCube<float4> CubeMap : register(t0);
SamplerState Sampler        : register(s0);

float4 main(float3 texCoord : TEXCOORD0) : SV_TARGET0
{
    return CubeMap.Sample(Sampler, normalize(texCoord));
}
```

Our *vertex shader* only makes use of the vertex position information, and generates the texture coordinates assuming the skybox is centered about the origin (0,0,0). It also sets the ``z`` value so that it's at the view far plane.

Save these three files to your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "SkyboxEffect_Common.hlsli", "SkyboxEffect_VS.hlsl", and "SkyboxEffect_PS.hlsl" and add them.

View **Properties** on "SkyboxEffect_VS.hlsl" and for "All Configurations" and "All Platforms", set the "Shader Type" to "Vertex Shader (/vs)" and select "OK".

View **Properties** on "SkyboxEffect_PS.hlsl" and for "All Configurations" and "All Platforms", set the "Shader Type" to "Pixel Shader (/ps)" and select "OK".

Build and run your project. It will have the same blank scene as before, but should have produced ``SkyboxEffect_VS.cso`` and ``SkyboxEffect_PS.cso``.

# Implementing the SkyboxEffect

Save the file [ReadData.h](https://github.com/Microsoft/DirectXTK/wiki/ReadData.h) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item....** Select "ReadData.h" and hit "OK".

Create a new file **SkyboxEffect.h** in your project:

```cpp
#include <vector>

class SkyboxEffect : public DirectX::IEffect
{
public:
    explicit SkyboxEffect(ID3D11Device* device);

    virtual void Apply(
        ID3D11DeviceContext* deviceContext) override;

    virtual void GetVertexShaderBytecode(
        void const** pShaderByteCode,
        size_t* pByteCodeLength) override;

    void SetTexture(ID3D11ShaderResourceView* value);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
    std::vector<uint8_t> m_vsBlob;
};
```

Create a new file **SkyboxEffect.cpp** in your project:

```cpp
#include "pch.h"
#include "SkyboxEffect.h"
#include "ReadData.h"

SkyboxEffect::SkyboxEffect(
    ID3D11Device* device)
{
    m_vsBlob = DX::ReadData(L"SkyboxEffect_VS.cso");

    DX::ThrowIfFailed(
        device->CreateVertexShader(m_vsBlob.data(), m_vsBlob.size(),
            nullptr, m_vs.ReleaseAndGetAddressOf()));

    auto psBlob = DX::ReadData(L"SkyboxEffect_PS.cso");

    DX::ThrowIfFailed(
        device->CreatePixelShader(psBlob.data(), psBlob.size(),
            nullptr, m_ps.ReleaseAndGetAddressOf()));
}

void SkyboxEffect::Apply(
    ID3D11DeviceContext* deviceContext)
{
    deviceContext->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

    deviceContext->VSSetShader(m_vs.Get(), nullptr, 0);
    deviceContext->PSSetShader(m_ps.Get(), nullptr, 0);
}

void SkyboxEffect::GetVertexShaderBytecode(
    void const** pShaderByteCode,
    size_t* pByteCodeLength)
{
    assert(pShaderByteCode != nullptr && pByteCodeLength != nullptr);
    *pShaderByteCode = m_vsBlob.data();
    *pByteCodeLength = m_vsBlob.size();
}

void SkyboxEffect::SetTexture(
    ID3D11ShaderResourceView* value)
{
    m_texture = value;
}
```

For a *DirectX Tool Kit* effect, only **Apply** and **GetVertexShaderBytecode** are required. We added **SetTexture** as a way to set the texture resource which can be changed dynamically.

Build and make sure it compiles.

> Many of the *DirectX Tool Kit* built-in effects support a number of properties which influence the selection of the VS/PS shader combination. This typically is implemented by creating the ``ID3D11VertexShader`` and ``ID3D11PixelShader`` objects on-demand when **Apply** is called based on current property values. Since our skybox effect has only one set of shaders, we go ahead and create them in the constructor.

# Adding camera settings

While we've implemented the basic effect, we are missing a key bit of information. The *vertex shader* makes use of a constant buffer to provide the transformation matrix. While we could just add another effect-specific method to control this, we will instead add the standard [[IEffectMatrices]] interface to our skybox effect.

In **SkyboxEffect.h**, modify your class's declaration to add the additional interface:

```cpp
class SkyboxEffect : public DirectX::IEffect, public DirectX::IEffectMatrices
```

Then add to the ``public`` section:

```cpp
void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;
```

And finally add to the ``private`` section (note we have a good reason for not having a ``m_world`` matrix variable which we will get to shortly):

```cpp
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;
DirectX::SimpleMath::Matrix m_worldViewProj;
```

In  **SkyboxEffect.cpp**, add these methods:

```cpp
void SkyboxEffect::SetWorld(FXMMATRIX /*value*/)
{
    // Skybox doesn't use the world matrix by design.
}

void SkyboxEffect::SetView(FXMMATRIX value)
{
    m_view = value;
}

void SkyboxEffect::SetProjection(FXMMATRIX value)
{
    m_proj = value;
}

void SkyboxEffect::SetMatrices(FXMMATRIX /*world*/, CXMMATRIX view, CXMMATRIX projection)
{
    // Skybox doesn't use the world matrix by design.
    m_view = view;
    m_proj = projection;
}
```

> It should be noted that we are very careful in the use of C++ multiple-inheritance only for interfaces. See [C++ Core Guidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c135-use-multiple-inheritance-to-represent-multiple-distinct-interfaces).

# Managing the constant buffer

We have all the data we need now for the constant buffer. In order to compute the *worldViewProj* value, we will need to do a matrix multiply. We'd like to avoid doing this more often than necessary, either keeping it from the previous frame if it's not changed or doing it only once if multiple matrices are updated in a single frame. The built-in effects all use a [dirty bits](https://wikipedia.org/wiki/Dirty_bit) design which we will use here.

In **SkyboxEffect.h**, add to the class's private variable declarations:

```cpp
uint32_t m_dirtyFlags;

struct __declspec(align(16)) SkyboxEffectConstants
{
    DirectX::XMMATRIX worldViewProj;
};

DirectX::ConstantBuffer<SkyboxEffectConstants> m_constantBuffer;
```

At the top of **SkyboxEffect.cpp** after the ``using`` statements, add:

```cpp
namespace
{
    constexpr uint32_t DirtyConstantBuffer = 0x1;
    constexpr uint32_t DirtyWVPMatrix = 0x2;
}
```

Update the **SkyboxEffect** constructor to initialize the new variables with all 'dirty bits' set:

```cpp
SkyboxEffect::SkyboxEffect(ID3D11Device* device) :
    m_dirtyFlags(uint32_t(-1)),
    m_constantBuffer(device)
{
    static_assert((sizeof(SkyboxEffect::SkyboxEffectConstants) % 16) == 0, "CB size alignment");

    // Get shaders
...
```

In each of the camera settings methods **SetView**, **SetProjection**, and **SetMatrices**, set the dirty bit to indicate the matrix changed:

```cpp
m_dirtyFlags |= DirtyWVPMatrix;
```

Now we revisit **Apply** to handle the constant buffer computation and updating using those 'dirty bits':

```cpp
void SkyboxEffect::Apply(ID3D11DeviceContext* deviceContext)
{
    if (m_dirtyFlags & DirtyWVPMatrix)
    {
        // Skybox ignores m_world matrix and the translation of m_view
        XMMATRIX view = m_view;
        view.r[3] = g_XMIdentityR3;
        m_worldViewProj = XMMatrixMultiply(view, m_proj);

        m_dirtyFlags &= ~DirtyWVPMatrix;
        m_dirtyFlags |= DirtyConstantBuffer;
    }

    if (m_dirtyFlags & DirtyConstantBuffer)
    {
        SkyboxEffectConstants constants;
        constants.worldViewProj = XMMatrixTranspose(m_worldViewProj);
        m_constantBuffer.SetData(deviceContext, constants);

        m_dirtyFlags &= ~DirtyConstantBuffer;
    }

    auto cb = m_constantBuffer.GetBuffer();
    deviceContext->VSSetConstantBuffers(0, 1, &cb);
    deviceContext->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

    deviceContext->VSSetShader(m_vs.Get(), nullptr, 0);
    deviceContext->PSSetShader(m_ps.Get(), nullptr, 0);
}
```

In most effects, we use *world*, *view*, and *projection* matrices. Many built-in effects compute inverse matrices as well as various concatenations from these matrices. For the skybox effect, however, we want unique behavior. The skybox is 'infinitely far away', so no amount of translation of the view should move it. Also, there really is no sense it should be 'scaled' nor does the sky itself need it's own transform. Therefore, we don't use a *world* matrix at all in our effect, and we zero out the translation from the camera *view* matrix.

At this point, build to ensure everything compiles and we now have a fully implemented skybox effect.

# Rendering the sky

Save the files [lobbycube.dds](https://github.com/Microsoft/DirectXTK/wiki/media/lobbycube.dds) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item....** Select "lobbycube.dds" and hit "OK".

Add to the **Game.h** file after the other includes:

```cpp
#include "SkyboxEffect.h"
```

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::GamePad> m_gamePad;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;

float m_pitch;
float m_yaw;

std::unique_ptr<DirectX::GeometricPrimitive> m_sky;
std::unique_ptr<DX::SkyboxEffect> m_effect;

Microsoft::WRL::ComPtr<ID3D11InputLayout> m_skyInputLayout;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubemap;
```

In the **Game.cpp** file, modify the constructor to initialize the new variables:

```cpp
Game::Game() noexcept(false) :
    m_pitch(0),
    m_yaw(0)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}
```

In the **Initialize** method, add:

```cpp
m_gamePad = std::make_unique<GamePad>();
```

In **Game.cpp**, modify TODO of **CreateDeviceDependentResources** to be:

```cpp
auto context = m_deviceResources->GetD3DDeviceContext();
m_sky = GeometricPrimitive::CreateGeoSphere(context, 2.f, 3,
    false /*invert for being inside the shape*/);

m_effect = std::make_unique<DX::SkyboxEffect>(device);

m_sky->CreateInputLayout(m_effect.get(),
    m_skyInputLayout.ReleaseAndGetAddressOf());

DX::ThrowIfFailed(
    CreateDDSTextureFromFile(device, L"lobbycube.dds",
        nullptr, m_cubemap.ReleaseAndGetAddressOf()));

m_effect->SetTexture(m_cubemap.Get());
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto size = m_deviceResources->GetOutputSize();

m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(size.right) / float(size.bottom), 0.1f, 10.f);

m_effect->SetProjection(m_proj);
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
auto pad = m_gamePad->GetState(0);

if (pad.IsConnected())
{
    if (pad.IsViewPressed())
    {
        ExitGame();
    }

    if (pad.IsLeftStickPressed())
    {
        m_yaw = m_pitch = 0.f;
    }
    else
    {
        constexpr float ROTATION_GAIN = 0.1f;
        m_yaw += -pad.thumbSticks.leftX * ROTATION_GAIN;
        m_pitch += pad.thumbSticks.leftY * ROTATION_GAIN;
    }
}

// limit pitch to straight up or straight down
constexpr float limit = XM_PIDIV2 - 0.01f;
m_pitch = std::max(-limit, m_pitch);
m_pitch = std::min(+limit, m_pitch);

// keep longitude in sane range by wrapping
if (m_yaw > XM_PI)
{
    m_yaw -= XM_2PI;
}
else if (m_yaw < -XM_PI)
{
    m_yaw += XM_2PI;
}

float y = sinf(m_pitch);
float r = cosf(m_pitch);
float z = r * cosf(m_yaw);
float x = r * sinf(m_yaw);

XMVECTORF32 lookAt = { x, y, z, 0.f };
m_view = XMMatrixLookAtRH(g_XMZero, lookAt, Vector3::Up);
```

In **Game.cpp**, add to the TODO of both **OnActivated** and **OnResuming**:

```cpp
m_gamePad->Resume();
```

In **Game.cpp**, add to the TODO of both **OnDeactivated** and **OnSuspending**:

```cpp
m_gamePad->Suspend();
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_sky.reset();
m_effect.reset();
m_skyInputLayout.Reset();
m_cubemap.Reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
m_effect->SetView(m_view);
m_sky->Draw(m_effect.get(), m_skyInputLayout.Get());
```

Build and run to see the skybox. Plug in a Xbox game controller and use the left stick to adjust the view to look around.

> If you prefer to use mouse & keyboard controls, use the control scheme in [[Mouse and keyboard input]].

You may notice that the text in the lobby-cube image appears backwards. The environment mapping in the shader assumes the cubemap 'surrounds' the scene, which means we are looking 'through' it which is what makes it seem backwards. If you want to flip it, just edit the *vertex shader* code in ``SkyboxEffect_VS.hlsl``:

```cpp
VSOutput main(float4 position : SV_Position)
{
    VSOutput vout;

    vout.PositionPS = mul(position, WorldViewProj);
    vout.PositionPS.z = vout.PositionPS.w; // Draw on far plane

    vout.TexCoord.x = -position.x;
    vout.TexCoord.yz = position.yz;

    return vout;
}
```

# More to explore

* Usable versions of the **SkyboxEffect** class can be downloaded from here: [.cpp](https://raw.githubusercontent.com/wiki/microsoft/DirectXTK/SkyboxEffect.cpp) / [.h](https://raw.githubusercontent.com/wiki/microsoft/DirectXTK/SkyboxEffect.h), and you will need the three shader files above as well.

**Next lessons:** [[Using HDR rendering]]

# Further reading

[HLSL, FXC, and D3DCompile](https://walbourn.github.io/hlsl-fxc-and-d3dcompile/)  
