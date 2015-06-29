This is a native Direct3D 11 implementation of the built-in BasicEffect from [XNA Game Studio 4](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.basiceffect.aspx) which supports texture mapping, vertex coloring, directional lighting, and fog.

See also [[Effects]]

# Header
    #include <Effects.h>

# Initialization
Construction requires a Direct3D 11 device.

    std::unique_ptr<BasicEffect> effect(new BasicEffect(device));

For exception safety, it is recommended you make use of the C++ RAII pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Interfaces

BasicEffect supports [[IEffect]], [[IEffectMatrices]], [[IEffectLights]], and [[IEffectFog]]

# Properties

* **SetDiffuseColor**: Sets the diffuse color of the effect. Defaults to white (1,1,1).

* **SetEmissiveColor**: Sets the emissive color of the effect. Defaults to black (0,0,0).

* **SetSpecularColor**: Sets the specular color of the effect. Defaults to white (1,1,1).

* **SetSpecularPower**: Sets the specular power of the effect. Defaults to 16. _Settings power to 0 can cause strange rendering artifacts._

* **DisableSpecular**: Disables the specular lighting for the effect. Sets the color to black (0,0,0) and power to 1.

* **SetAlpha**: Sets the alpha (transparency) of the effect. Defaults to 1 (fully opaque).

* **SetVertexColorEnabled**: Enables per-vertex color. Defaults to false. Modifying this setting requires recreating associated input layouts, and enabling it requires ``COLOR``.

* **SetTextureEnabled**: Enables texturing. Defaults to false. Modifying this setting requires recreating associated input layouts, and enabling it requires ``TEXCOORD0``.

* **SetTexture**: Associates a texture shader resource view with the effect. Can be set to nullptr to remove a reference.

# Further reading

[BasicEffect optimizations in XNA Game Studio 4.0](http://blogs.msdn.com/b/shawnhar/archive/2010/04/25/basiceffect-optimizations-in-xna-game-studio-4-0.aspx)  
