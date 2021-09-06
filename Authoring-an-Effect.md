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

> **UNDER CONSTRUCTION**

# Implementing the SkyboxEffect

> **UNDER CONSTRUCTION**

# Rendering the sky

> **UNDER CONSTRUCTION**

**Next lessons:** [[Using HDR rendering]]
