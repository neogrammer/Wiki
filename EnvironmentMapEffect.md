This is a native Direct3D 11 implementation of the built-in EnvironmentMapEffect from [XNA Game Studio 4](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.environmentmapeffect.aspx) which supports cubic environment mapping with texture mapping, vertex lighting, and fogging.

See also [[Effects]]

# Header
    #include <Effects.h>

# Initialization
Construction requires a Direct3D 11 device.

    std::unique_ptr<EnvironmentMapEffect> effect;
    effect = std::make_unique<EnvironmentMapEffect>(device);

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

EnvironmentMapEffect supports [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], and [[IEffectFog]]

# Input layout
This effect requires ``SV_Position``, ``NORMAL`` and ``TEXCOORD``.

# Properties

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1). Alpha channel (.w component) is ignored.

* **SetEmissiveColor**: Sets the emissive color of the effect. Defaults to black (0,0,0).

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque).

* **SetColorAndAlpha**: Sets the diffuse color of the effect and the alpha (transparency).

* **SetTexture**: Associates a texture shader resource view with the effect for the diffuse layer. Can be set to nullptr to remove a reference.

* **SetEnvironmentMap**: Associates the cubemap shader resource view with the effect. Can be set to nullptr to remove a reference.

* **SetEnvironmentMapAmount**: Controls the diffuse vs. environment map blending percentage, and ranges from 0 to 1. It defaults to 1.

* **SetEnvironmentMapSpecular**: Sets the specular color for the environment map. Defaults to black (0,0,0) which disables the specular highlight.

* **SetFresnelFactor**: Sets the Frensel factor for the environment map. Defaults to 1. Can be set to 0 to disable the Fresnel factor.

# Remarks
EnvironmentMapEffect always uses vertex lighting, and does not support per-pixel lighting.

This effect requires a texture sampler in both slots 0 and 1. [[GeometricPrimitive]] and [[SpriteBatch]] only set a texture sampler in slot 0 by default, [[Model]] sets a sampler in slots 0 and 1.

# Further reading

[EnvironmentMapEffect](http://blogs.msdn.com/b/shawnhar/archive/2010/08/09/environmentmapeffect.aspx)  
