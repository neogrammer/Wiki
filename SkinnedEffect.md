|[[DirectXTK]]|[[Effects]]|
|---|---|

This is a native Direct3D 11 implementation of the built-in SkinnedEffect from XNA Game Studio 4 (``Microsoft.Xna.Framework.Graphics.SkinnedEffect``) which supports skinned animation with up to 72 bones and 1, 2, or 4 bone influences per vertex. It supports texture mapping, directional vertex lighting, directional per-pixel lighting, and fog.

**Related tutorial:** [[Using skinned models]]

```mermaid
classDiagram
class IEffect{
    <<Interface>>
    +Apply()
    +GetVertexShaderBytecode()
}
class IEffectMatrices{
    <<Interface>>
    +SetWorld()
    +SetView()
    +SetProjection()
    +SetMatrices()
}
class IEffectLights{
    <<Interface>>
    +SetPerPixelLighting()
    +SetAmbientLightColor()
    +SetLightEnabled()
    +SetLightDirection()
    +SetLightDiffuseColor()
    +SetLightSpecularColor()
    +EnableDefaultLighting()
}
class IEffectFog{
    <<Interface>>
    +SetFogEnabled()
    +SetFogStart()
    +SetFogEnd()
    +SetFogColor()
}
class IEffectSkinning{
    <<Interface>>
    +SetWeightsPerVertex()
    +SetBoneTransforms()
    +ResetBoneTransforms()
}
class SkinnedEffect{
    +SetDiffuseColor()
    +SetEmissiveColor()
    +SetSpecularColor()
    +SetSpecularPower()
    +DisableSpecular()
    +SetAlpha()
    +SetColorAndAlpha()
    +SetTexture()
    +SetBiasedVertexNormals()
}
SkinnedEffect --|> IEffect
SkinnedEffect --|> IEffectMatrices
SkinnedEffect --|> IEffectLights
SkinnedEffect --|> IEffectFog
SkinnedEffect --|> IEffectSkinning
```

# Header
```cpp
#include <Effects.h>
```

# Initialization
Construction requires a Direct3D 11 device.

```cpp
std::unique_ptr<SkinnedEffect> effect;
effect = std::make_unique<SkinnedEffect>(device);
```

For exception safety, it is recommended you make use of the C++ [RAII](http://wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

SkinnedEffect supports [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], [[IEffectFog]], and [[IEffectSkinning]]

# Input layout
This effect requires ``NORMAL``, ``TEXCOORD0``, ``BLENDINDICES`` and ``BLENDWEIGHT``.

# Properties

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1). Alpha channel (.w component) is ignored.

* **SetEmissiveColor**: Sets the emissive color of the effect. Defaults to black (0,0,0).

* **SetSpecularColor**: Sets the specular color of the effect. Defaults to white (1,1,1).

* **SetSpecularPower**: Sets the specular power of the effect. Defaults to 16. _Settings power to 0 can cause strange rendering artifacts._

* **DisableSpecular**: Disables the specular lighting for the effect. Sets the color to black (0,0,0) and power to 1.

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque). This value is also used for binning opaque vs. transparent geometry.

* **SetColorAndAlpha**: Sets the diffuse color of the effect and the alpha (transparency).

* **SetTexture**: Associates a texture shader resource view with the effect. Can be set to nullptr to remove a reference. Can optionally include an alpha channel as well.

* **SetBiasedVertexNormals**: Enables support for compressed vertex normals which require ``*2 - 1`` biasing at runtime such as ``DXGI_FORMAT_R10G10B10A2_UNORM``.

# Bone weights
The ``BLENDINDICES`` and ``BLENDWEIGHT`` elements can hold up to 4 individual bone influences per vertex. Since each influence adds more computation to the shader, you can optimize this effect by setting the supported number of influences to 1, 2, or 4 via **SetWeightsPerVertex** and any additional influences will be ignored. This value defaults to 4.

# Remarks
The effect always performs either vertex or per-pixel lighting. Calling ``SetLightingEnabled(false);`` on an instance of **SkinnedEffect** will throw a C++ exception.

This effect always performs texturing, so if 'untextured' rendering is desired you must provide texture coordinates, and a sampler in slot 0. The class will default to a 1x1 texture with white (1,1,1,1).
