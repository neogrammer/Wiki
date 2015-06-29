This is a native Direct3D 11 implementation of the five built-in effects from XNA Game Studio 4, providing identical functionality and API:

* [[BasicEffect]] supports texture mapping, vertex coloring, directional lighting, and fog
* [[AlphaTestEffect]] supports per-pixel alpha testing
* [[DualTextureEffect]] supports two layer multi-texturing (for light maps or detail textures)
* [[EnvironmentMapEffect]] supports cubic environment mapping
* [[SkinnedEffect]] supports skinned animation with up to 72 bones and 1, 2, or 4 bone influences per vertex.

DirectX Tool Kit also includes the following built-in effects:
* [[DGSLEffect]] which supports the [Visual Studio Shader Designer](https://msdn.microsoft.com/en-us/library/hh315733.aspx) (DGSL) content pipeline both with and without skinned animation with up to 8 textures.

See also [[EffectFactory]]

![Effects Example](https://github.com/Microsoft/DirectXTK/wiki/images/EffectsExample.png)

**Related tutorials:** [[Simple rendering]], [[Using advanced shaders]], [[Applying lightmaps]], [[Creating custom shaders with DGSL]], [[Writing custom shaders]]

# Header
    #include <Effects.h>

# Initialization
The built-in effect constructors requires a Direct3D 11 device.

    std::unique_ptr<BasicEffect> effect(new BasicEffect(device));

For exception safety, it is recommended you make use of the C++ RAII pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

The **DGSLEffect** constructor takes a pixelShader instance. If null, it uses one of the three built-in default materials: _Unlit_, _Lambert_, and _Phong_. This class assumes the pixel shader provided is signature compatible with the built-in DGSL vertex shader, and will work for the feature level of the device.

    std::unique_ptr<DGSLEffect> effect(new DGSLEffect(device,pixelShader));

# Set effect parameters

    effect->SetWorld(world);
    effect->SetView(view);
    effect->SetProjection(projection);

    effect->SetTexture(cat);
    effect->SetTextureEnabled(true);

    effect->EnableDefaultLighting();

The built-in effects default to a standard lighting and color set

* Matrices are all set to identity
* Ambient, diffuse, and emissive colors default to black
* Fully opaque (alpha set to 1)
* Specular defaults to white with a power of 16
* Default lights are set to white with no specular highlight and a default direction of ``[0, -1, 0]``. All lights are disabled by default.

The **EnableDefaultLighting** method sets up a standard three light setup (key, fill, and back) with some ambient light and some soft specular highlights.

*Note*: To disable specular highlights on a material with _BasicEffect_, _SkinnedEffect_ and _DGSLEffect_; **DisableSpecular** sets the specular color to black ``[0,0,0]`` and the specular power to 1. A specular power of 0 can result in strange rendering artifacts.

# Draw using the effect

    effect->Apply(deviceContext);

    deviceContext->IASetInputLayout(...);
    deviceContext->IASetVertexBuffers(...);
    deviceContext->IASetIndexBuffer(...);
    deviceContext->IASetPrimitiveTopology(...);
    deviceContext->PSSetSamplers(...);

    deviceContext->DrawIndexed(...);

**DualTextureEffect** and **EnvironmentMapEffect** require a texture sampler in both slots 0 and 1. **DGSLEffect** can require up to 8 texture samplers. The other textured effects only require a texture sampler in slot 0. [[GeometricPrimitive]] and [[SpriteBatch]] only set a texture sampler in slot 0 by default, [[Model]] sets a sampler in slots 0 and 1.

**DualTextureEffect** requires two sets of texture coordinates, while the other textured effects only require a single set. [[GeometricPrimitive]], [[Model]] loaded from ``VBO`` or ``CMO``, and [[SpriteBatch]] only define a single set of texture coordinates in their vertex buffers so they can't be used with _DualTextureEffect_.

# Input Layout

To create an input layout matching the effect vertex shader input signature:

    // First, configure effect parameters the way you will be using it. Turning
    // lighting, texture map, or vertex color on/off alters which vertex shader
    // is used, so GetVertexShaderBytecode will return a different blob after
    // you alter these parameters. If you create an input layout using a
    // BasicEffect that had lighting disabled, but then later enable lighting,
    // that input layout will no longer match as it will not include the
    // now-necessary normal vector.

    void const* shaderByteCode;
    size_t byteCodeLength;

    effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    device->CreateInputLayout(VertexPositionNormalTexture::InputElements,
                              VertexPositionNormalTexture::InputElementCount,
                              shaderByteCode, byteCodeLength,
                              pInputLayout);

For the built-in effects, the trigger for needing to create a new layout would be:

* Enabling or disabling lighting (which requires a vertex normal)
* Enabling or disabling per vertex color (which requires a vertex color value)
* Enabling or disabling textures (which requires vertex texture coordinates)
* Changing effect class (BasicEffect <-> SkinnedEffect or DGSLEffect)

_Note:_ DGSLEffect is typically used with VertexPositionNormalTangentColorTexture or VertexPositionNormalTangentColorTextureSkinning.

# Interfaces

The built-in effects support a number of different settings, some of which are organized into more 'generic' interfaces.

* [[IEffect]] is the basic interface for all effects which includes applying it to the device context and obtaining the shader information needed to create a Direct3D 11 input layout with a signature that matches the effect's shader. Remember that a given Effect instance could return a different shader based on internal state.
* [[IEffectMatrices]] is the interface for setting an effects' world, view, and projection matrices. All the built-in effects support this interface.
* [[IEffectLights]] is the interface for controlling the effects' lighting computations and settings. This is supported by _BasicEffect_, _EnvironmentMapEffect_, _SkinningEffect_, and _DGSLEffect_
* [[IEffectFog]] is the interface to control the effects' fog settings. This is supported by _BasicEffect_, _AlphaTestEffect_, _DualTextureEffect_, _EnvironmentMapEffect_, and _SkinnedEffect_.
* [[IEffectSkinning]] is the interface to control skinned animation settings. This is supported by _DGSLEffect_ and _SkinnedEffect_. This includes setting the bone transform matrices, and optimizing the number of bone influences per vertex to process (1, 2, or 4; defaults to 4).

# Coordinate systems

The built-in effects work equally well for both right-handed and left-handed coordinate systems. The one difference is that the fog settings start & end for left-handed coordinate systems need to be negated (i.e. ``SetFogStart(6)``, ``SetFogEnd(8)`` for right-handed coordinates becomes ``SetFogStart(-6)``, ``SetFogEnd(-8)`` for left-handed coordinates).

# Feature Level Notes
The built-in shaders are compiled using the ``vs_4_0_level_9_1`` and ``ps_4_0_level_9_1`` profiles to support all feature levels.

The compiled shaders are integrated into the DirectXTK library to avoid the need for runtime compilation, shader reflection, or deploying compiled shader binary files (``.cso``).

The DGSLEffect includes built-in support for the three default materials: Unlit, Lambert, and Phong. These built-in DGSL materials support all feature levels, as does the built-in DGSL-compatible vertex shader. Visual Studio Shader Designer (DGSL) ``.DGSL.CSO`` files support Feature Level 10.0+. The [[DGSLEffectFactory|EffectFactory]] automatically attempts to locate a suitably named standard ``.CSO`` on Feature Level 9.x which is a manually created fall-back shader. The method for creating these fall-back shaders is to use "Export to HLSL..." from the Visual Studio Shader Designer, then modify that ``.hlsl`` file so it will successfully compile with ``ps_4_0_level_9_1`` or ``ps_4_0_level_9_3`` (whichever is your minimum supported feature level).

# Threading model

Creation is fully asynchronous, so you can instantiate multiple effect  instances at the same time on different threads. Each instance only supports drawing from one thread at a time, but you can simultaneously draw on multiple threads if you create a separate effect instance per Direct3D 11 deferred context.

[Immediate and Deferred Rendering](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476892.aspx)

# Further reading

[New built-in effects in XNA Game Studio 4.0](http://blogs.msdn.com/b/shawnhar/archive/2010/04/28/new-built-in-effects-in-xna-game-studio-4-0.aspx)  
[Built-in effects, permutations, and performance](http://blogs.msdn.com/b/shawnhar/archive/2010/04/30/built-in-effects-permutations-and-performance.aspx)  
[HLSL, FXC, and D3DCompile](http://blogs.msdn.com/b/chuckw/archive/2012/05/07/hlsl-fxc-and-d3dcompile.aspx)
