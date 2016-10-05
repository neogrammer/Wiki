This is a native Direct3D 11 implementation of the built-in BasicEffect from [XNA Game Studio 4](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.basiceffect.aspx) which supports texture mapping, vertex coloring, directional vertex lighting, directional per-pixel lighting, and fog.

See also [[Effects]]

# Header
    #include <Effects.h>

# Initialization
Construction requires a Direct3D 11 device.

    std::unique_ptr<BasicEffect> effect;
    effect = std::make_unique<BasicEffect>(device);

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

BasicEffect supports [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], and [[IEffectFog]]

# Input layout
This effect requires ``SV_Position``, ``NORMAL`` if lighting is enabled, ``COLOR`` if per-vertex colors are enabled, and  ``TEXCOORD0`` if texturing is enabled.

# Properties

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1). Alpha channel (.w component) is ignored.

* **SetEmissiveColor**: Sets the emissive color of the effect. Defaults to black (0,0,0).

* **SetSpecularColor**: Sets the specular color of the effect. Defaults to white (1,1,1).

* **SetSpecularPower**: Sets the specular power of the effect. Defaults to 16. _Settings power to 0 can cause strange rendering artifacts._

* **DisableSpecular**: Disables the specular lighting for the effect. Sets the color to black (0,0,0) and power to 1.

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque).

* **SetColorAndAlpha**: Sets the diffuse color of the effect and the alpha (transparency).

* **SetVertexColorEnabled**: Enables per-vertex color. Defaults to false. Modifying this setting requires recreating associated input layouts, and enabling it requires ``COLOR``.

* **SetTextureEnabled**: Enables texturing. Defaults to false. Modifying this setting requires recreating associated input layouts, and enabling it requires ``TEXCOORD0``.

* **SetTexture**: Associates a texture shader resource view with the effect. Can be set to nullptr to remove a reference.

* **SetBiasedVertexNormals**: Enables support for compressed vertex normals which require ``*2 - 1`` biasing at runtime.

# Example
Here is an example of creating and using a basic effect instance:

        std::unique_ptr<DirectX::BasicEffect> basicEffect;

When creating device-dependent resources:

    basicEffect = std::make_unique<BasicEffect>(device);

    basicEffect->EnableDefaultLighting();
    basicEffect->SetDiffuseColor(Colors::Red);
    basicEffect->SetFogColor(Colors::CornflowerBlue);
    basicEffect->SetFogStart(fogstart);
    basicEffect->SetFogEnd(fogend);
    basicEffect->SetTexture( texture.Get() );

When the window size is changed is where you typically set the projection:

    basicEffect->SetProjection(projection);

A view matrix is computed based on user input and camera settings:

    basicEffect->SetView(view);

Then to render:

    basicEffect->SetWorld(world);
    basicEffect->Apply(context);

    context->OMSetBlendState(states->AlphaBlend(), Colors::White, 0xFFFFFFFF);
    context->OMSetDepthStencilState(states->DepthDefault(), 0);
    context->RSSetState(states->None());

    ID3D11SamplerState* samplers[] = { states->LinearWrap() };
    context->PSSetSamplers(0, 1, samplers);

    context->IASetVertexBuffers(...);
    context->IASetIndexBuffer(...);
    context->IASetPrimitiveTopology(...);
    context->DrawIndexed(...);

When dealing with lost device:

    basicEffect.reset();

# Further reading

[BasicEffect optimizations in XNA Game Studio 4.0](http://blogs.msdn.com/b/shawnhar/archive/2010/04/25/basiceffect-optimizations-in-xna-game-studio-4-0.aspx)  
[BasicEffect: a misnomer?](http://blogs.msdn.com/b/shawnhar/archive/2008/08/22/basiceffect-a-misnomer.aspx)  
