This is a native Direct3D 11 implementation of the built-in DualTextureEffect from [XNA Game Studio 4](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.dualtextureeffect.aspx) which supports two layer multi-texturing (for light maps or detail textures)

See also [[Effects]]

# Header
    #include <Effects.h>

# Initialization
Construction requires a Direct3D 11 device.

    std::unique_ptr<DualTextureEffect> effect;
    effect = std::make_unique<DualTextureEffect>(device);

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

DualTextureEffect supports [[IEffect]], [[IEffectMatrices]], and [[IEffectFog]]

Does not support lighting as it is assumed to be 'baked' into one of the two textures.

# Input layout
This effect requires ``SV_Position``, ``TEXCOORD0``, and  ``TEXCOORD1``. It requires ``COLOR`` if per-vertex colors are enabled.

# Properties

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1).

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque).

* **SetVertexColorEnabled**: Enables per-vertex color. Defaults to false. Modifying this setting requires recreating associated input layouts, and enabling it requires ``COLOR``.

* **SetTexture**: Associates a texture shader resource view with the effect. Can be set to nullptr to remove a reference.

* **SetTexture2**: Associates a texture shader resource view with the effect. Can be set to nullptr to remove a reference.

# Remarks

This effect requires a texture sampler in both slots 0 and 1. [[GeometricPrimitive]] and [[SpriteBatch]] only set a texture sampler in slot 0 by default, [[Model]] sets a sampler in slots 0 and 1.

[[GeometricPrimitive]], [[Model]] loaded from ``VBO`` or ``CMO``, and [[SpriteBatch]] only define a single set of texture coordinates in their vertex buffers so they can't be used with DualTextureEffect.

# Further reading

[DualTextureEffect](http://blogs.msdn.com/b/shawnhar/archive/2010/08/04/dualtextureeffect.aspx)  
