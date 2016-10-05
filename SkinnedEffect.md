This is a native Direct3D 11 implementation of the built-in SkinnedEffect from [XNA Game Studio 4](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.skinnedeffect.aspx) which supports skinned animation with up to 72 bones and 1, 2, or 4 bone influences per vertex. It supports texture mapping, directional vertex lighting, directional per-pixel lighting, and fog.

See also [[Effects]]

# Header
    #include <Effects.h>

# Initialization
Construction requires a Direct3D 11 device.

    std::unique_ptr<SkinnedEffect> effect;
    effect = std::make_unique<SkinnedEffect>(device);

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

SkinnedEffect supports [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], and [[IEffectFog]]

# Input layout
This effect requires ``NORMAL``, ``TEXCOORD0``, ``BLENDINDICES`` and ``BLENDWEIGHT``.

# Properties

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1). Alpha channel (.w component) is ignored.

* **SetEmissiveColor**: Sets the emissive color of the effect. Defaults to black (0,0,0).

* **SetSpecularColor**: Sets the specular color of the effect. Defaults to white (1,1,1).

* **SetSpecularPower**: Sets the specular power of the effect. Defaults to 16. _Settings power to 0 can cause strange rendering artifacts._

* **DisableSpecular**: Disables the specular lighting for the effect. Sets the color to black (0,0,0) and power to 1.

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque).

* **SetColorAndAlpha**: Sets the diffuse color of the effect and the alpha (transparency).

* **SetTexture**: Associates a texture shader resource view with the effect. Can be set to nullptr to remove a reference.

* **SetBiasedVertexNormals**: Enables support for compressed vertex normals which require ``*2 - 1`` biasing at runtime.

# Bone weights
The ``BLENDINDICES`` and ``BLENDWEIGHT`` elements can hold up to 4 individual bone influences per vertex. Since each influence adds more computation to the shader, you can optimize this effect by setting the supported number of influences to 1, 2, or 4 via **SetWeightsPerVertex** and any additional influences will be ignored. This value defaults to 4.

# Remarks
The effect always performs either vertex or per-pixel lighting.

This effect always performs texturing, so if 'untextured' rendering is desired you must provide texture coordinates, a sampler in slot 0, and a 1x1 texture with white (1,1,1,1).

