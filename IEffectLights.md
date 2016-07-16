This abstract interface controls directional lighting. Settings for this interface can influence the choice of shader permutation and input layout signature. This interface is implemented by _BasicEffect_, _DGSLEffect_, _EnvironmentMapEffect_, _NormalMapEffect_, and _SkinningEffect_.

Effects that implement this interface require ``NORMAL`` semantic data in the vertex input layout.

# Obtaining the interface
There are two methods used in _DirectX Tool Kit_. For simple cases, just maintain a reference directly to the desired effect class:

    std::shared_ptr<BasicEffect> effect;

    ...

    effect->SetLightingEnabled(true);
    effect->SetLightEnabled( 0, true );

    static const XMVECTORF32 light { 0.f, -1.f, 0.f, 0.f };
    effect->SetLightDirection( 0, light );

For more general cases where a number of effect classes can be in use (such as [[Model]] which uses a mix of _BasicEffect_, _DualTextureEffect_, _SkinnedEffect_, and/or _DGSLEffect_), use [Run-Time Type Information](https://en.wikipedia.org/wiki/Run-time_type_information) (RTTI) to obtain the interface.

    std::shared_ptr<IEffect> effect;

    ...

    auto ilights = dynamic_cast<IEffectLights*>( effect.get() );
    if ( ilights )
    {
        ilights->SetLightEnabled( 0, true );

        static const XMVECTORF32 light { 0.f, -1.f, 0.f, 0.f };
        ilights->SetLightDirection( 0, light );
    }

# Controlling lights
The ``IEffectLights`` interface supports 1, 2, or 3 directional lights (_MaxDirectionalLights_ is 3) with an ambient light setting. The lights support both diffuse and specular color, and some effects support per-pixel lighting.

The direction vector for the light is assumed to be normalized, and is pointing towards the objects it lights.

The default lighting set by **EnableDefaultLighting** consist of three lights:

* Ambient: (0.05333332, 0.09882354, 0.1819608)
* Light 0: Direction (-0.5265408, -0.5735765, -0.6275069), Diffuse (1, 0.9607844, 0.8078432), Specular (1, 0.9607844, 0.8078432)
* Light 1: Direction (0.7198464,  0.3420201,  0.6040227), Diffuse (0.9647059, 0.7607844, 0.4078432), Specular (0, 0, 0)
* Light 2: Direction (0.4545195, -0.7660444,  0.4545195), Diffuse (0.3231373, 0.3607844, 0.3937255), Specular (0.3231373, 0.3607844, 0.3937255).

# Built-in Effect Notes

## AlphaTestEffect, DualTextureEffect
These built-in effects do not support this interface.

## BasicEffect 
Supports up to 3 directional lights with vertex or per-pixel lighting, optionally using per-vertex colors. Materials definitions include diffuse color, specular color, specular power, and emissive color. Light definitions include ambient color, diffuse color, specular color, and specular power. Note that the standard lighting model does not include an ambient material color.

## DGSLEffect
DGSL shaders always use per-pixel lighting, so the **SetPerPixelLighting** method is not supported for this effect. Any calls to **SetPerPixelLighting** through the base interface are ignored. DGSL materials include ambient color, diffuse color, specular color, and specular power. DGSL light definitions include ambient color, diffuse color, specular color, and specular power.

The number of valid lights is determined by the implementation of the DGSL pixel shader, so calls to **SetLightingEnabled** may have no effect. DGSL defines up to 4 directional lights (``DGSLEffect::MaxDirectionalLights`` is 4). The built-in implementation of the default material "Lambert" supports up to 4 directional lights, while the built-in default material "Phong" supports only 3 directional lights in order to support Feature Level 9.1.

## EnvironmentMapEffect
This effect always implements lighting with 3 directional lights using the cubemap and specular factor for specular highlights, so the **SetLightingEnabled** and **SetLightSpecularColor** methods are not supported for this effect.  Calling ``SetLightingEnabled(false)`` through the base interface will generate an exception, and calls to ``SetLightSpecularColor`` are ignored.

## SkinnedEffect
This shader always implements lighting with 3 directional lights, so the **SetLightingEnabled** method is not supported for this effect. Calling ``SetLightingEnabled(false)`` through the base interface will generate an exception.

## NormalMapEffect
This effect always implements per-pixel lighting with 3 directional lights, so the **SetLightingEnabled** and **SetPerPixelLighting** methods are not supported for this effect.  Calling ``SetLightingEnabled(false)`` through the base interface will generate an exception, and calls to ``SetPerPixelLighting`` are ignored.