This effect extends [[BasicEffect]] to support normal-mapping and an optional specular map. It supports texture mapping, vertex coloring, directional per-pixel lighting, and fog.

See also [[Effects]]

# Header
    #include <Effects.h>

# Initialization
Construction requires a Direct3D 11 device.

    std::unique_ptr<NormalMapEffect> effect;
    effect = std::make_unique<NormalMapEffect>(device);

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

NormalMapEffect supports [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], and [[IEffectFog]]

# Input layout
This effect requires ``SV_Position``, ``NORMAL``, ``TEXCOORD0``, and ``TANGENT``. If per-vertex colors are enabled, it also requires ``COLOR``.

# Properties

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1). Alpha channel (.w component) is ignored.

* **SetEmissiveColor**: Sets the emissive color of the effect. Defaults to black (0,0,0).

* **SetSpecularColor**: Sets the specular color of the effect. Defaults to white (1,1,1).

* **SetSpecularPower**: Sets the specular power of the effect. Defaults to 16. _Settings power to 0 can cause strange rendering artifacts._

* **DisableSpecular**: Disables the specular lighting for the effect. Sets the color to black (0,0,0) and power to 1.

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque).

* **SetColorAndAlpha**: Sets the diffuse color of the effect and the alpha (transparency).

* **SetVertexColorEnabled**: Enables per-vertex color. Defaults to false. Modifying this setting requires recreating associated input layouts, and enabling it requires ``COLOR``.

* **SetTexture**: Associates a texture shader resource view with the effect for the diffuse texture. Can be set to nullptr to remove a reference.

* **SetNormalTexture**: Associates a texture shader resource view with the effect for the normal map texture. Can be set to nullptr to remove a reference.

* **SetSpecularTexture**: Associates a texture shader resource view with the effect for the specular texture. Can be set to nullptr.

* **SetBiasedVertexNormalsAndTangents**: Enables support for compressed vertex normals and tangents which require ``*2 - 1`` biasing at runtime.

# Remarks

This effect always performs per-pixel lighting. Calls to ``SetLightingEnabled(false);`` will generate a C++ exception, and calls to **SetPerPixelLighting** are ignored.

This effect always performs texturing, so if 'untextured' diffuse rendering is desired you must provide a 1x1 texture with white (1,1,1,1).