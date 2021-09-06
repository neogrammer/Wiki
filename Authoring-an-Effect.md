This lesson covers writing your own [[IEffect|Effects]] implementation, specifically a custom effect for rendering a [skybox](https://en.wikipedia.org/wiki/Skybox_%28video_games%29) with a cubemap.

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

View **Properties** on "SkyboxEffect_VS.hlsl" and for "All Configurations" and "All Platforms", set the "Shader Type" to "Vertex Shader (/ps)" and select "OK".

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

    virtual void Apply(ID3D11DeviceContext* deviceContext) override;
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

> **UNDER CONSTRUCTION**

# Adding camera settings

> **UNDER CONSTRUCTION**

# Rendering the sky

Save the files [lobbycube.dds](https://github.com/Microsoft/DirectXTK/wiki/media/lobbycube.dds) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item....** Select "lobbycube.dds" and hit "OK".

> **UNDER CONSTRUCTION**

# More to explore

* Usable versions of the **SkyboxEffect** class can be downloaded from here: [.cpp](https://raw.githubusercontent.com/wiki/microsoft/DirectXTK/SkyboxEffect.cpp) / [.h](https://raw.githubusercontent.com/wiki/microsoft/DirectXTK/SkyboxEffect.h), and you will need the three shader files above as well.

**Next lessons:** [[Using HDR rendering]]
