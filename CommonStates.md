|[[DirectXTK]]|
|---|

The CommonStates class is a factory which simplifies setting the most common combinations of Direct3D rendering states. This is simliar to the XNA Game Studio 4 (``Microsoft.Xna.Framework.Graphics.BlendState``, ``DepthStencilState``, ``RasterizerState``, ``SamplerState``) design.

**Related tutorial:** [[Sprites and textures]]

# Header
```cpp
#include <CommonStates.h>
```

# Initialization

The CommonStates constructor requires a Direct3D 11 device.

```cpp
std::unique_ptr<CommonStates> states;
states = std::make_unique<CommonStates>(device);
```

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Using this helper to set device state

```cpp
deviceContext->OMSetBlendState(states->Opaque(), Colors::Black, 0xFFFFFFFF);
deviceContext->OMSetDepthStencilState(states->DepthDefault(), 0);
deviceContext->RSSetState(states->CullCounterClockwise());

auto samplerState = states->LinearWrap();
deviceContext->PSSetSamplers(0, 1, &samplerState);
```

# Blending State
[D3D11_BLEND_DESC](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_blend_desc)  
[D3D11_BLEND_DESC1](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11_1/ns-d3d11_1-cd3d11_blend_desc1)  

* ID3D11BlendState* **Opaque**();
* ID3D11BlendState* **AlphaBlend**();
* ID3D11BlendState* **Additive**();
* ID3D11BlendState* **NonPremultiplied**();

## Typical usage

For standard drawing, typically you should make use of Opaque().

For drawing alpha-blended objects, you should use AlphaBlend() if using premultiplied alpha, or
NonPremultiplied() if using 'straight' alpha.

For multipass rendering, you'd typically use Additive().

# Depth/Stencil State
[D3D11_DEPTH_STENCIL_DESC](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_depth_stencil_desc)  

* ID3D11DepthStencilState* **DepthNone**();
* ID3D11DepthStencilState* **DepthDefault**();
* ID3D11DepthStencilState* **DepthRead**();
* ID3D11DepthStencilState* **DepthReverseZ**();
* ID3D11DepthStencilState* **DepthReadReverseZ**();

## Typical usage

For standard rendering with a z-buffer, you should use DepthDefault(). If using a [reverse z-buffer](https://developer.nvidia.com/content/depth-precision-visualized), then you should use DepthReverseZ().

For drawing alpha blended objects (which is typically done after all opaque objects have been drawn), use DepthRead() which will respect the existing z-buffer values, but will not update them with 'closer' pixels.

For drawing objects without any depth-sort at all, use DepthNone().

# Rasterizer State
[D3D11_RASTERIZER_DESC](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_rasterizer_desc)  
[D3D11_RASTERIZER_DESC1](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11_1/ns-d3d11_1-cd3d11_rasterizer_desc1)

* ID3D11RasterizerState* **CullNone**();
* ID3D11RasterizerState* **CullClockwise**();
* ID3D11RasterizerState* **CullCounterClockwise**();
* ID3D11RasterizerState* **Wireframe**();

## Typical usage

For default geometry winding use CullCounterClockwise(). For inverted winding (typically when using assets designed for left-handed coordinates but rendering with right-handed coordinates or vice-versa), use CullClockwise().

For "double-sided" geometry, use CullNone(). Keep in mind this is a potentially large performance hit, so use it sparingly.

Wireframe() is a wireframe rendering mode and shows both sides of the geometry.

# Sampler State
[D3D11_SAMPLER_DESC](https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ns-d3d11-d3d11_sampler_desc)

* ID3D11SamplerState* **PointWrap**();
* ID3D11SamplerState* **PointClamp**();
* ID3D11SamplerState* **LinearWrap**();
* ID3D11SamplerState* **LinearClamp**();
* ID3D11SamplerState* **AnisotropicWrap**();
* ID3D11SamplerState* **AnisotropicClamp**();

## Typical usage

Usually LinearClamp() is the standard setting, and covers a large number of cases. For improved mipmap filtering quality use the Anisotropic settings.

Remember that ??SetSamplers() actually takes an _array_ of sampler state objects, rather than a pointer directly to the sampler state object, since there can be multiple textures in use at the same time.

# Feature level usage

All common states work with all feature levels. ``Anisotropic*`` uses a *MaxAnisotropy* of 2 on Feature Level 9.1. On all other feature levels is uses 16 (``D3D11_MAX_MAXANISOTROPY``).

[Direct3D feature levels](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-devices-downlevel-intro)

# Remarks

These common states are equivalent to using the following descriptors:

## Blend states

```cpp
CD3D11_DEFAULT def;

// Opaque
CD3D11_BLEND_DESC desc(def);

// AlphaBlend
CD3D11_BLEND_DESC desc(def);
desc.RenderTarget[0].BlendEnable = TRUE;
desc.RenderTarget[0].SrcBlend =
desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
desc.RenderTarget[0].DestBlend =
desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

// Additive
CD3D11_BLEND_DESC desc(def);
desc.RenderTarget[0].BlendEnable = TRUE;
desc.RenderTarget[0].SrcBlend =
desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
desc.RenderTarget[0].DestBlend =
desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

// NonPremultiplied
CD3D11_BLEND_DESC desc(def);
desc.RenderTarget[0].BlendEnable = TRUE;
desc.RenderTarget[0].SrcBlend =
desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
desc.RenderTarget[0].DestBlend =
desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
```

## Depth/Stencil states

```cpp
CD3D11_DEFAULT def;

// DepthNone
CD3D11_DEPTH_STENCIL_DESC desc(def);
desc.DepthEnable = FALSE;
desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

// DepthDefault
CD3D11_DEPTH_STENCIL_DESC desc(def);
desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

// DepthRead
CD3D11_DEPTH_STENCIL_DESC desc(def);
desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

// DepthReverseZ
CD3D11_DEPTH_STENCIL_DESC desc(def);
desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

// DepthReadReverseZ
CD3D11_DEPTH_STENCIL_DESC desc(def);
desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
```

## Rasterizer states

```cpp
// CullNone
CD3D11_RASTERIZER_DESC desc(D3D11_FILL_SOLID, D3D11_CULL_NONE,
    FALSE /* FrontCounterClockwise */,
    D3D11_DEFAULT_DEPTH_BIAS,
    D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE /* DepthClipEnable */,
    FALSE /* ScissorEnable */,
    TRUE /* MultisampleEnable */,
    FALSE /* AntialiasedLineEnable */);

// CullClockwise
CD3D11_RASTERIZER_DESC desc(D3D11_FILL_SOLID, D3D11_CULL_FRONT,
    FALSE /* FrontCounterClockwise */,
    D3D11_DEFAULT_DEPTH_BIAS,
    D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE /* DepthClipEnable */,
    FALSE /* ScissorEnable */,
    TRUE /* MultisampleEnable */,
    FALSE /* AntialiasedLineEnable */);

// CullCounterClockwise
CD3D11_RASTERIZER_DESC desc(D3D11_FILL_SOLID, D3D11_CULL_BACK,
    FALSE /* FrontCounterClockwise */,
    D3D11_DEFAULT_DEPTH_BIAS,
    D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE /* DepthClipEnable */,
    FALSE /* ScissorEnable */,
    TRUE /* MultisampleEnable */,
    FALSE /* AntialiasedLineEnable */);

// Wireframe
CD3D11_RASTERIZER_DESC desc(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE,
    FALSE /* FrontCounterClockwise */,
    D3D11_DEFAULT_DEPTH_BIAS,
    D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE /* DepthClipEnable */,
    FALSE /* ScissorEnable */,
    TRUE /* MultisampleEnable */,
    FALSE /* AntialiasedLineEnable */);
```

## Sampler states

```cpp
const float border[4] = { 0.f, 0.f, 0.f, 0.f };
float maxAnisotropy = (device->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? 16 : 2;

// PointWrap
CD3D11_SAMPLER_DESC desc(D3D11_FILTER_MIN_MAG_MIP_POINT,
    D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
    0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

// PointClamp
CD3D11_SAMPLER_DESC desc(D3D11_FILTER_MIN_MAG_MIP_POINT,
    D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
    0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

// LinearWrap
CD3D11_SAMPLER_DESC desc(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
    0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

// LinearClamp
CD3D11_SAMPLER_DESC desc(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
    0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

// AnisotropicWrap
CD3D11_SAMPLER_DESC desc(D3D11_FILTER_ANISOTROPIC,
    D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
    0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

// AnisotropicClamp
CD3D11_SAMPLER_DESC desc(D3D11_FILTER_ANISOTROPIC,
    D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
    0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);
```

# Further reading

[State objects in XNA Game Studio 4.0](http://www.shawnhargreaves.com/blog/state-objects-in-xna-game-studio-4-0.html)

[Premultiplied alpha](http://www.shawnhargreaves.com/blog/premultiplied-alpha.html)  
[Premultiplied alpha and image composition](http://www.shawnhargreaves.com/blog/premultiplied-alpha-and-image-composition.html)  
[Premultiplied alpha in XNA Game Studio 4.0](http://www.shawnhargreaves.com/blog/premultiplied-alpha-in-xna-game-studio-4-0.html)
