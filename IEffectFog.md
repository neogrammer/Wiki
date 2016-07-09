This abstract interface controls distance fog settings. Settings for this interface can influence the choice of shader permutation. This interface is implemented by _BasicEffect_, _AlphaTestEffect_, _DualTextureEffect_, _EnvironmentMapEffect_, and _SkinnedEffect_.

# Obtaining the interface

There are two methods used in DirectX Tool Kit. For simple cases, just maintain a reference directly to the desired effect class:

    std::shared_ptr<BasicEffect> effect;

    ...

    effect->SetFogEnable(true);
    effect->SetFogStart(6);
    effect->SetFogEnd(8);
    effect->SetFogColor( Colors::CornflowerBlue );

For more general cases where a number of effect classes can be in use (such as [[Model]] which uses a mix of _BasicEffect_, _DualTextureEffect_, _SkinnedEffect_, and/or _DGSLEffect_), use [Run-Time Type Information](https://en.wikipedia.org/wiki/Run-time_type_information) (RTTI) to obtain the interface.

    std::shared_ptr<IEffect> effect;

    ...

    auto ifog = dynamic_cast<IEffectFog*>( part->effect.get() );
    if ( ifog )
    {
        ifog->SetFogEnable(true);
        ifog->SetFogStart(6);
        ifog->SetFogEnd(8);
        ifog->SetFogColor( Colors::CornflowerBlue );
    }

# Fog
The fog effects work for both right-handed and left-handed coordinate systems, but the distance settings should be negated for left-handed coordinates.

# Built-in Effect Notes

## BasicEffect, AlphaTestEffect, DualTextureEffect, EnvironmentMapEffect, SkinnedEffect
These implement a simple linear fog which is inexpensive on all feature levels.

## DGSLEffect
Fog settings are encoded in the choice of DGSL pixel shader, if supported, and these shaders do not expose settings to control such effects. Therefore, this built-in effect does not support this interface. The default materials (_Unlit_, _Lambert_, and _Phong_) have no fog effects.

