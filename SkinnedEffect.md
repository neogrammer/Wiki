This is a native Direct3D 11 implementation of the built-in SkinnedEffect from [XNA Game Studio 4](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.skinnedeffect.aspx) which supports skinned animation with up to 72 bones and 1, 2, or 4 bone influences per vertex.

See also [[Effects]]

# Header
    #include <Effects.h>

# Initialization
Construction requires a Direct3D 11 device.

    std::unique_ptr<SkinnedEffect> effect(new SkinnedEffect(device));

For exception safety, it is recommended you make use of the C++ RAII pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

SkinnedEffect supports [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], and [[IEffectFog]]

# Input layout
This effect requires ``NORMAL``, ``TEXCOORD0``, ``BLENDINDICES`` and ``BLENDWEIGHT``.

# Properties

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1).

* **SetEmissiveColor**: Sets the emissive color of the effect. Defaults to black (0,0,0).

* **SetSpecularColor**: Sets the specular color of the effect. Defaults to white (1,1,1).

* **SetSpecularPower**: Sets the specular power of the effect. Defaults to 16. _Settings power to 0 can cause strange rendering artifacts._

* **DisableSpecular**: Disables the specular lighting for the effect. Sets the color to black (0,0,0) and power to 1.

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque).

* **SetTexture**: Associates a texture shader resource view with the effect. Can be set to nullptr to remove a reference.

# Remarks
The effect always performs either vertex or per-pixel lighting.

This effect always performs texturing, so if 'untextured' rendering is desired you must provide texture coordinate, a sampler in slot 0, and a 1x1 texture with white (1,1,1,1).

