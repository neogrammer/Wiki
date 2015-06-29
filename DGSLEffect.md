DGSLEffect is a _DirectX Tool Kit_ implementation of the Visual Studio Shader Designer (DGSL) content pipeline both with and without skinned animation with up to 8 textures.

See also [[Effects]], [[DGSLEffectFactory|EffectFactory]]

# Header
    #include <Effects.h>

# Initialization
Construction requires a Direct3D 11 device and a _pixelShader_ instance.

    std::unique_ptr<DGSLEffect> effect(new DGSLEffect(device,pixelShader));

If the _pixelShader_ instance is null, it uses one of the three built-in default materials: _Unlit_, _Lambert_, and _Phong_. This class assumes the pixel shader provided is signature compatible with the built-in DGSL vertex shader, and will work for the feature level of the device.

For exception safety, it is recommended you make use of the C++ RAII pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

DGSLEffect supports [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], [[IEffectSkinning]]

Fog settings are not supported by this effect, but could be 'baked in' to a given DGSL pixel shader.

# Properties
* **SetAmbientColor**: Sets the ambient color of the material. Defaults to black (0,0,0). _Note that other built-in effects do not make use of the ambient material color assuming they react equally to all ambient light in the scene._

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1).

* **SetEmissiveColor**: Sets the emissive color of the effect. Defaults to black (0,0,0).

* **SetSpecularColor**: Sets the specular color of the effect. Defaults to white (1,1,1).

* **SetSpecularPower**: Sets the specular power of the effect. Defaults to 16. _Settings power to 0 can cause strange rendering artifacts._

* **DisableSpecular**: Disables the specular lighting for the effect. Sets the color to black (0,0,0) and power to 1.

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque).

* **SetUVTransform**: The DGSL effects can apply a UV transform.

* **SetViewport**: DGSL shaders can reference the size of the viewport.

* **SetTime**: DGSL shaders can animate based on a time value.

* **SetAlphaDiscardEnable**: DGSL shaders can optionally use alpha discard.

* **SetVertexColorEnabled**: Enables per-vertex color. Defaults to false. Modifying this setting requires recreating associated input layouts, and enabling it requires ``COLOR``.

* **SetTextureEnabled**: Enables texturing. Defaults to false. Modifying this setting requires recreating associated input layouts, and enabling it requires ``TEXCOORD0``.

* **SetTexture**: Associates a texture shader resource view with the effect, which can support up to 8 textures (_MaxTextures_ is 8). Can be set to nullptr to remove a reference: ``SetTexture(value);`` is equivalent to ``SetTexture(0, value);``,

* **SetTexture2**: Associates a texture shader resource view with the effect. Can be set to nullptr to remove a reference. Equivalent to calling ``SetTexture(1, value);``

# Feature Level Notes
The DGSLEffect includes built-in support for the three default materials: _Unlit_, _Lambert_, and _Phong_. These built-in DGSL materials support all feature levels, as does the built-in DGSL-compatible vertex shader

Visual Studio Shader Designer (DGSL) ``.DGSL.CSO`` files support Feature Level 10.0+.

The [[DGSLEffectFactory|EffectFactory]] automatically attempts to locate a suitably named standard ``.CSO`` on Feature Level 9.x which is a manually created fall-back shader. The method for creating these fall-back shaders is to use "Export to HLSL..." from the Visual Studio Shader Designer, then modify that ``.hlsl`` file so it will successfully compile with ``ps_4_0_level_9_1`` or ``ps_4_0_level_9_3`` (whichever is your minimum supported feature level).

# Remarks

DGSLEffect is typically used with ``VertexPositionNormalTangentColorTexture`` or ``VertexPositionNormalTangentColorTextureSkinning``. See [[VertexTypes]]

DGSL shaders always use per-pixel lighting if lighting is supported by the effect.

DGSL shaders can require up to 8 texture samplers. [[GeometricPrimitive]] and [[SpriteBatch]] only set a texture sampler in slot 0 by default, [[Model]] sets a sampler in slots 0 and 1. Note that it only makes use of a single set of texture coordinates no matter how many textures are used.

DGSL shaders support up to 4 directional lights, while the other built-in shaders only support 3 (i.e. ``DGSLEffect::MaxDirectionalLights`` is 4).

# Further reading

[BasicEffect optimizations in XNA Game Studio 4.0](http://blogs.msdn.com/b/shawnhar/archive/2010/04/25/basiceffect-optimizations-in-xna-game-studio-4-0.aspx)  
