|[[DirectXTK]]|
|---|

The **VertexTypes.h** header defines these commonly used vertex data structures:

| Structure | Stride |
|---|---
| VertexPosition | 12 bytes |
| VertexPositionColor | 28 bytes |
| VertexPositionTexture | 20 bytes |
| VertexPositionDualTexture | 28 bytes |
| VertexPositionNormal | 24 bytes |
| VertexPositionColorTexture  | 36 bytes |
| VertexPositionNormalColor | 40 bytes |
| VertexPositionNormalTexture | 32 bytes |
| VertexPositionNormalColorTexture  |  48 bytes |
| VertexPositionNormalTangentColorTexture | 52 bytes |
| VertexPositionNormalTangentColorTextureSkinning | 60 bytes |

# Header
```cpp
#include <VertexTypes.h>
```

# Input Layout
Each type also provides a ``D3D11_INPUT_ELEMENT_DESC`` array which can be used to create a matching input layout, for example:

```cpp
Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

device->CreateInputLayout(
    VertexPositionColorTexture::InputElements,
    VertexPositionColorTexture::InputElementCount,
    vertexShaderCode, vertexShaderCodeSize,
    inputLayout.GetAddressOf());
```

# Usage

*Position* (which uses the semantic "SV_Position") is required for all rendering. This is XMFLOAT3 (``DXGI_FORMAT_R32G32B32_FLOAT``).

*Normal* is required for any effect that supports lighting computations. This is XMFLOAT3 (``DXGI_FORMAT_R32G32B32_FLOAT``).

*Color* is for per-vertex coloring used by some effects.  Color is typically XMFLOAT4(``DXGI_FORMAT_R32G32B32A32_FLOAT``).

*Texture* coordinates (which uses the semantic "TEXCOORD0") are XMFLOAT2 (``DXGI_FORMAT_R32G32_FLOAT``).

*DualTexture* coordinates (which uses the semantic "TEXCOORD0" and "TEXCOORD1") are XMFLOAT2 (``DXGI_FORMAT_R32G32_FLOAT``).

# Geometry formats

See [DirectXMesh](https://github.com/Microsoft/DirectXMesh/wiki/Geometry-formats) for more information.

## Visual Studio content pipeline
_VertexPositionNormalTangentColorTexture_ contains the vertex elements needed for using Visual Studio Shader Designer (DGSL) shaders via [[DGSLEffect|Effects]] (which requires position, normal, tangent, color, and texture coordinates) and is used when loading ``.CMO`` file models. It defines Color as a ``uint32_t`` (``DXGI_FORMAT_R8G8B8A8_UNORM``)

_VertexPositionNormalTangentColorTextureSkinning_ extends VertexPositionNormalTangentColorTexture  with blend weights and indices for skinning using Visual Studio Shader Designer (DGSL) shaders via **DGSLEffect** and is used when loading ``.CMO`` file skinned models. It defines the blend indices as  ``uint32_t`` (``DXGI_FORMAT_R8G8B8A8_UINT``) and the blend weights as  ``uint32_t`` (``DXGI_FORMAT_R8G8B8A8_UNORM``).

## DirectX SDK SDKMESH
The ``.SDKMESH`` format describes input layouts as Direct3D 9 style vertex decls. There is therefore no specific vertex structure for ``.SDKMESH`` data and such input layouts are built on-the-fly. The [[Model]] loader attempts to create input layouts that match the needs of the standard [[Effects]], so it can end up ignoring some of the vertex layout content if the critical content is found first.

# Remarks

* *DirectX Tool Kit for DX12* does not support CMO, so it does not have ``VertexPositionNormalTangentColorTexture`` or ``VertexPositionNormalTangentColorTextureSkinning``.

* *DirectX Tool Kit* built-in [[Effects]] that require a tangent-space compute the tangent/bi-tangent in the shader from the normal and texture coordinates to simplify usage.

Christian Schüler, "Normal Mapping without Precomputed Tangents", *ShaderX 5*, Chapter 2.6, pp. 131 – 140 and [this blog post](http://www.thetenthplanet.de/archives/1180)
