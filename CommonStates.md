The CommonStates class is a factory which simplifies setting the most common combinations of Direct3D rendering states.

**Related tutorial:** [[Sprites and textures]]

# Header
    #include <CommonStates.h>

# Initialization

The CommonStates constructor requires a Direct3D 11 device.

    std::unique_ptr<CommonStates> states(new CommonStates(device));

For exception safety, it is recommended you make use of the C++ RAII pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Using this helper to set device state

    deviceContext->OMSetBlendState(states->Opaque(), Colors::Black, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(states->DepthDefault(), 0);
    deviceContext->RSSetState(states->CullCounterClockwise());

    auto samplerState = states->LinearWrap();
    deviceContext->PSSetSamplers(0, 1, &samplerState);

# Blending State
[XNA Game Studio](http://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.blendstate_fields.aspx)  
[D3D11_BLEND_DESC](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476087.aspx)  
[D3D11_BLEND_DESC1](http://msdn.microsoft.com/en-us/library/windows/desktop/hh404435.aspx)  

* ID3D11BlendState* Opaque();
* ID3D11BlendState* AlphaBlend();
* ID3D11BlendState* Additive();
* ID3D11BlendState* NonPremultiplied();

## Typical usage

For standard drawing, typically you should make use of Opaque().

For drawing alpha-blended objects, you should use AlphaBlend() if using premultiplied alpha, or 
NonPremultiplied() if using 'straight' alpha.

For multipass rendering, you'd typically use Additive().

# Depth/Stencil State
[XNA Game Studio](http://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.depthstencilstate_fields.aspx)  
[D3D11_DEPTH_STENCIL_DESC](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476110.aspx)  

* ID3D11DepthStencilState* DepthNone();
* ID3D11DepthStencilState* DepthDefault();
* ID3D11DepthStencilState* DepthRead();

## Typical usage

For standard rendering with a z-buffer, you should use DepthDefault().

For drawing alpha blended objects (which is typically done after all opaque objects have been drawn), use DepthRead() which will respect the existing z-buffer values, but will not update them with 'closer' pixels.

For drawing objects without any depth-sort at all, use DepthNone().

# Rasterizer State
[XNA Game Studio](http://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.rasterizerstate_fields.aspx)  
[D3D11_RASTERIZER_DESC](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476198.aspx)  
[D3D11_RASTERIZER_DESC1](http://msdn.microsoft.com/en-us/library/windows/desktop/hh404489.aspx)

* ID3D11RasterizerState* CullNone();
* ID3D11RasterizerState* CullClockwise();
* ID3D11RasterizerState* CullCounterClockwise();
* ID3D11RasterizerState* Wireframe();

## Typical usage

For default geometry winding use CullCounterClockwise(). For inverted winding (typically when using assets designed for left-handed coordinates but rendering with right-handed coordinates or vice-versa), use CullClockwise().

For "double-sided" geometry, use CullNone(). Keep in mind this is a potentially large performance hit, so use it sparingly.

Wireframe() is self-explanatory.

# Sampler State
[XNA Game Studio](http://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.samplerstate_fields.aspx)  
[D3D11_SAMPLER_DESC](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476207.aspx)

* ID3D11SamplerState* PointWrap();
* ID3D11SamplerState* PointClamp();
* ID3D11SamplerState* LinearWrap();
* ID3D11SamplerState* LinearClamp();
* ID3D11SamplerState* AnisotropicWrap();
* ID3D11SamplerState* AnisotropicClamp();

## Typical usage

Usually LinearClamp() is the standard setting, and covers a large number of cases. For improved mipmap filtering quality use the Ansiotropic settings.

Remember that ??SetSamplers() actually takes an _array_ of sampler state objects, rather than a pointer directly to the sampler state object, since there can be multiple textures in use at the same time.

# Feature level usage

All common states work with all feature levels. Anisotropic*() uses a MaxAnisotropy of 2 on Feature Level 9.1.

[Direct3D feature levels](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476876.aspx)

# Remarks

These common states are equivalent to using the following descriptors:

## Blend states

    CD3D11_DEFAULT default;

    // Opaque
    CD3D11_BLEND_DESC desc(default);

    // AlphaBlend
    CD3D11_BLEND_DESC desc(default);
    desc.RenderTarget[0].BlendEnable = TRUE;
    desc.RenderTarget[0].SrcBlend = desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

    // Additive
    CD3D11_BLEND_DESC desc(default);
    desc.RenderTarget[0].BlendEnable = TRUE;
    desc.RenderTarget[0].SrcBlend = desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

    // NonPremultiplied
    CD3D11_BLEND_DESC desc(default);
    desc.RenderTarget[0].BlendEnable = TRUE;
    desc.RenderTarget[0].SrcBlend = desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

## Depth/Stencil states

    CD3D11_DEFAULT default;

    // DepthNone
    CD3D11_DEPTH_STENCIL_DESC desc(default);
    desc.DepthEnable = FALSE;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    // DepthDefault
    CD3D11_DEPTH_STENCIL_DESC desc(default);
    desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    // DepthRead
    CD3D11_DEPTH_STENCIL_DESC desc(default);
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

## Rasterizer states

    // CullNone
    CD3D11_RASTERIZER_DESC desc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE, 0, 0.f, 0.f, TRUE, FALSE, TRUE, FALSE);

    // CullClockwise
    CD3D11_RASTERIZER_DESC desc(D3D11_FILL_SOLID, D3D11_CULL_FRONT, FALSE, 0, 0.f, 0.f, TRUE, FALSE, TRUE, FALSE);

    // CullCounterClockwise
    CD3D11_RASTERIZER_DESC desc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE, 0, 0.f, 0.f, TRUE, FALSE, TRUE, FALSE);

    CD3D11_RASTERIZER_DESC desc(D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, FALSE, 0, 0.f, 0.f, TRUE, FALSE, TRUE, FALSE);

## Sampler states

    const float border[4] = { 0.f, 0.f, 0.f, 0.f };
    float maxAnisotropy = (device->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? 16 : 2;

    // PointWrap
    CD3D11_SAMPLER_DESC desc(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, 0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

    // PointClamp
    CD3D11_SAMPLER_DESC desc(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

    // LinearWrap
    CD3D11_SAMPLER_DESC desc(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, 0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

    // LinearClamp
    CD3D11_SAMPLER_DESC desc(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

    // AnisotropicWrap
    CD3D11_SAMPLER_DESC desc(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, 0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

    // AnisotropicClamp
    CD3D11_SAMPLER_DESC desc(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

# Further reading

[State objects in XNA Game Studio 4.0](http://blogs.msdn.com/b/shawnhar/archive/2010/04/02/state-objects-in-xna-game-studio-4-0.aspx)

[Premultiplied Alpha](http://www.shawnhargreaves.com/blogindex.html#premultipliedalpha)

