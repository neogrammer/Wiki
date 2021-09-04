This effect extends [[BasicEffect]] to support normal-mapping and an optional specular map. It supports texture mapping, vertex coloring, directional per-pixel lighting, fog, and GPU instancing.

See also [[Effects]]

**Related tutorials:** [[Using advanced shaders]], [[Multistream rendering and instancing]]

# Header
```cpp
#include <Effects.h>
```

# Initialization
Construction requires a Direct3D 11 device.

```cpp
std::unique_ptr<NormalMapEffect> effect;
effect = std::make_unique<NormalMapEffect>(device);
```

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

NormalMapEffect supports [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], and [[IEffectFog]]

# Input layout
This effect requires ``SV_Position``, ``NORMAL``, and ``TEXCOORD0``. If per-vertex colors are enabled, it also requires ``COLOR``.

If instancing is enabled, this effect also requires these vertex elements:

```
"InstMatrix",  0, DXGI_FORMAT_R32G32B32A32_FLOAT
"InstMatrix",  1, DXGI_FORMAT_R32G32B32A32_FLOAT
"InstMatrix",  2, DXGI_FORMAT_R32G32B32A32_FLOAT
```

# Properties

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1). Alpha channel (.w component) is ignored.

* **SetEmissiveColor**: Sets the emissive color of the effect. Defaults to black (0,0,0).

* **SetSpecularColor**: Sets the specular color of the effect. Defaults to white (1,1,1).

* **SetSpecularPower**: Sets the specular power of the effect. Defaults to 16. _Settings power to 0 can cause strange rendering artifacts._

* **DisableSpecular**: Disables the specular lighting for the effect. Sets the color to black (0,0,0) and power to 1.

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque). This value is also used for binning opaque vs. transparent geometry.

* **SetColorAndAlpha**: Sets the diffuse color of the effect and the alpha (transparency).

* **SetVertexColorEnabled**: Enables per-vertex color. Defaults to false. Modifying this setting requires recreating associated input layouts, and enabling it requires ``COLOR``.

* **SetTexture**: Associates a texture shader resource view with the effect for the diffuse/albedo texture. Can be set to nullptr to remove a reference. Can optionally include an alpha channel as well.

* **SetNormalTexture**: Associates a texture shader resource view with the effect for the normal map texture. Can be set to nullptr to remove a reference.

* **SetSpecularTexture**: Associates a texture shader resource view with the effect for the specular texture. Can be set to nullptr.

* **SetBiasedVertexNormals**: Enables support for compressed vertex normals which require ``*2 - 1`` biasing at runtime such as ``DXGI_FORMAT_R10G10B10A2_UNORM``.

* **SetInstancingEnabled**: Enables support for per-vertex instancing by adding a per-vertex ``XMFLOAT3X4`` transform matrix.

# Normal maps
The normal-mapping textures used by this effect are tangent-space normal maps as opposed to object-space normal maps.

The normal map textures used by this effect are assumed to be ``_UNORM`` formats and therefore encoded using the 'x2 bias' model, i.e. the normal component's value range of -1 to +1 have been converted to 0 to +1. This is a simple encoding that is commonly used for normal texture maps since most file image formats don't support negative numbers. The conversion back to signed values is handled internally in the shader code.

The red (x) and green (y) channels are loaded from the texture, but blue (z) and alpha (w) channels are unused. The shader reconstructs the z component of the normal using ``z = sqrt( 1 - dot( xy, xy ) )``. This has two positive benefits:

1. You can make use the ``DXGI_FORMAT_BC5_UNORM`` (i.e. [3Dc](https://en.wikipedia.org/wiki/3Dc)) for normal map texture compression without any shader or configuration changes.
1. Due to quantization effects, the actual unit length of the normal can drift, so this reconstruction can improve quality by effectively renormalizing the value.

Finally, the green (y) channel is used "as is" in the shader code. Some viewing and materials conventions can require the green channel to be inverted to properly 'push in' or 'push out'. This issue should be handled by modifying the texture content either offline or at load time.

# Remarks

This effect implements the classic diffuse [Lambertian](https://en.wikipedia.org/wiki/Lambertian_reflectance) shading with [Phong](https://en.wikipedia.org/wiki/Phong_reflection_model) specular highlights lighting model with the addition of per-pixel normals and lighting. Optionally the specular highlights can be implemented as a specular map.

Calls to ``SetLightingEnabled(false);`` will generate a C++ exception, and calls to **SetPerPixelLighting** are ignored.

This effect always performs texturing, so if 'untextured' rendering is desired you must provide texture coordinates, and a sampler in slot 0. The class will default to a 1x1 texture with white (1,1,1,1) for the texture, and (0.5, 0.5) for the normal map (i.e. a 'smooth' surface). This is useful when using GPU instancing which is not supported by [[BasicEffect]].

<table border=0>
 <tr>
  <td>Diffuse map</td>
  <td>Normal map</td>
  <td>Specular map</td>
 </tr>
 <tr>
  <td><img src="https://github.com/Microsoft/DirectXTK/wiki/images/diffusemap.png"></td>
  <td><img src="https://github.com/Microsoft/DirectXTK/wiki/images/normalmap.png"></td>
  <td><img src="https://github.com/Microsoft/DirectXTK/wiki/images/specularmap.png"></td>
 <tr>
</table>

# Feature Level Notes

This effect uses Shader Model 4.0 so requires Direct3D hardware feature level 10.0 or greater.

> Note this means you can also count on ``DXGI_FORMAT_BC5_UNORM`` texture compression hardware support for your normal maps.

[Direct3D feature levels](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-devices-downlevel-intro)

# Further reading

Christian Schüler, "Normal Mapping without Precomputed Tangents", *ShaderX 5*, Chapter 2.6, pp. 131 – 140 and [this blog post](http://www.thetenthplanet.de/archives/1180)

J.M.P. van Waveren and Ignacio Castaño, "Real-Time Normal Map DXT Compression", id Software, February 2008 [PDF](http://developer.download.nvidia.com/whitepapers/2008/real-time-normal-map-dxt-compression.pdf) (note: BC5 is "tangent-space 3Dc")
