This is a helper for drawing simple geometric shapes including texture coordinates and surface normals.

* Box/Cube
* Cone
* Cylinder
* Dodecahedron
* Icosahedron
* Octahedron
* Sphere (uv or geodesic)
* Teapot
* Tetrahedron
* Torus

![GeometricPrimitive Example](https://github.com/Microsoft/DirectXTK/wiki/images/GeometricPrimitiveExample.png)

**Related tutorial:** [[3D shapes]]

# Header
    #include <GeometricPrimitive.h>

# Initialization
The GeometryPrimitive class must be created from a factory method which takes the Direct3D 11 device context.

    std::unique_ptr<GeometricPrimitive> shape(
        GeometricPrimitive::CreateTeapot(deviceContext) );

For exception safety, the factory functions return a ``std::unique_ptr``.

* **CreateBox**( deviceContext, const XMFLOAT3& size): Creates a box with a non-uniform size.
* **CreateCone**( deviceContext, float diameter = 1, float height = 1, size_t tessellation = 32): Creates a [cone](http://en.wikipedia.org/wiki/Cone) of a given height, diameter, and tessellation factor.
* **CreateCube**( deviceContext, float size = 1): Creates a [cube](http://en.wikipedia.org/wiki/Cube) (also known as a [hexahedron](http://en.wikipedia.org/wiki/Hexahedron)) of the given size.
* **CreateCylinder**( deviceContext, float height = 1, float diameter = 1, size_t tessellation = 32): Creates a [cylinder](http://en.wikipedia.org/wiki/Cylinder_(geometry)) of given height, diameter, tessellation factor.
* **CreateDodecahedron**( deviceContext, float size = 1): Creates a [dodecahedron](http://en.wikipedia.org/wiki/Dodecahedron) of a given size..
* **CreateGeoSphere**( deviceContext, float diameter = 1, size_t tessellation = 3): Creates a geodesic [sphere](http://en.wikipedia.org/wiki/Sphere) with the given diameter and tessellation factor.
* **CreateIcosahedron**( deviceContext, float size = 1): Creates a [icosahedron](http://en.wikipedia.org/wiki/Icosahedron) of a given size..
* **CreateOctahedron**( deviceContext, float size = 1): Creates a [octahedron](http://en.wikipedia.org/wiki/Octahedron) of a given size..
* **CreateSphere**( deviceContext, float diameter = 1, size_t tessellation = 16): Creates a uv-[sphere](http://en.wikipedia.org/wiki/Sphere) of given diameter with the given tessellation factor.
* **CreateTeapot**( deviceContext, float size = 1, size_t tessellation = 8): Creates the [Utah Teapot](http://en.wikipedia.org/wiki/Utah_teapot) of a given size and tessellation factor.
* **CreateTetrahedron**( deviceContext, float size = 1): Creates a [tetrahedron](http://en.wikipedia.org/wiki/Tetrahedron) of given size..
* **CreateTorus**( deviceContext, float diameter = 1, float thickness = 0.333f, size_t tessellation = 32): Creates a [torus](http://en.wikipedia.org/wiki/Torus) of given diameter, thickness, and tessellation factor.

# Simple drawing
Simple solid shape drawing:

    shape->Draw(world, view, projection, Colors::CornflowerBlue);

Simple wireframe shape drawing:

    shape->Draw(world, view, projection, Colors::Gray, nullptr, true);

Simple solid textured shape drawing:

    shape->Draw(world, view, projection, Colors::Gray, cat);

The **Draw** method also accepts an optional callback function which can be used to override the default rendering state:

    shape->Draw(world, view, projection, Colors::White, catTexture, false, [=]
    {
        deviceContext->OMSetBlendState(...);
    });

This makes use of a [[BasicEffect|Effects]] shared by all geometric primitives drawn on that device context.

# Advanced drawing
This form of drawing makes use of a custom [[IEffect|Effects]] instance. The caller is responsible for using an input layout that matches the effect.

    IEffect* myeffect = ...

    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    shape->CreateInputLayout( myeffect, inputLayout.GetAddressOf() );

    shape->Draw( myeffect, inputLayout.Get() );

This takes an optional parameter for enabling alpha-blending, wireframe, and a callback function which can be used to override the default rendering state.

The **Draw** operation will only set up a texture sampler in slot 0. If you are using an [[IEffect|Effects]] instance with more than a single texture, you need to set additional samplers manually (perhaps via ``setCustomState`` callbacks). For example, if using _EnvironmentMapEffect_:

    shape->Draw( myeffect, inputLayout.Get(), false, false, [=]
    {
        ID3D11SamplerState* samplerState[2] = { states.LinearClamp(),
            states.LinearWrap() };
        deviceContext->PSSetSamplers(0, 2, &samplerState);
    });

> GeometricPrimitive shapes define a single set of texture coordinates, which works well with [[BasicEffect]] or
[[EnvironmentMapEffect]]. They don't include two sets of texture coordinates, so you can't use them with [[DualTextureEffect]]. They do not include tangents or bi-normals, so they don't work with [[DGSLEffect]]. They don't include skinning weights and bone indices, so they aren't suited for use with [[SkinnedEffect]].

# Coordinate systems
These geometric primitives (based on the XNA Game Studio conventions) use right-handed coordinates. They can be used with left-handed coordinates by setting the _rhcoords_ parameter on the factory methods to 'false' to reverse the winding ordering (the parameter defaults to 'true').

For a left-handed view system: 

    std::unique_ptr<GeometricPrimitive> shape(
        GeometricPrimitive::CreateTeapot( deviceContext, 1.f, 8, false ) );

_Note: Using the wrong value for rhcoords for your viewing setup will result in the objects looking 'inside out'._

# Inside vs. Outside
These geometric primitives are intended for view from the 'outside' for efficient back-face culling. However, both spheres and boxes are commonly used to form 'skyboxes' for backgrounds. To support this, you set the _rhcoords_ parameter backwards for your view coordinates, and then set _invertn_ to true.

For a right-handed view system: 

    std::unique_ptr<GeometricPrimitive> sky(
        GeometricPrimitive::CreateBox( deviceContext, XMFLOAT3(10,10,10), false, true);

    std::unique_ptr<GeometricPrimitive> sky(
        GeometricPrimitive::CreateSphere( deviceContext, 100.f, false, true);

For a left-handed view system: 

    std::unique_ptr<GeometricPrimitive> sky(
        GeometricPrimitive::CreateBox( deviceContext, XMFLOAT3(10,10,10), true, true);

    std::unique_ptr<GeometricPrimitive> sky(
        GeometricPrimitive::CreateSphere( deviceContext, 100.f, true, true);

# Alpha blending
Alpha blending defaults to using premultiplied alpha. To make use of 'straight' alpha textures, override the blending mode via the optional callback:

    CommonStates states(device);

    shape->Draw(world, view, projection, Colors::White, catTexture, false, [=]
    {
        deviceContext->OMSetBlendState( states.NonPremultiplied(), nullptr, 0xFFFFFFFF);
    });

# Custom geometry

There are equivalent static methods for each of the factory methods that return the vertex and index buffer data as ``std::vector``. These values can be modified, and then used to create a customized geometric primitive or drawn through some other mechanism.

    std::vector<VertexPositionNormalTexture> vertices;
    std::vector<uint16_t> indices;
    GeometricPrimitive::CreateBox( vertices, indices,
        XMFLOAT3(1.f/2.f, 2.f/2.f, 3.f/2.f) );

    // Tile the texture in a 5x5 grid
    for( auto it = vertices.begin(); it != vertices.end(); ++it )
    {
        it->textureCoordinate.x *= 5.f;
        it->textureCoordinate.y *= 5.f;
    }

    std::unique_ptr<GeometricPrimitive> customBox(
        GeometricPrimitive::CreateCustom( deviceContext, vertices, indices ) );

# Feature Level Notes
In order to support [all feature levels](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476876.aspx), the GeometricPrimitive implementation make use of 16-bit indices (``DXGI_FORMAT_R16_UINT``) which limits to a maximum of 65535 vertices.

Feature Level 9.1 is also limited to a maximum of 65535 primitives in a single draw. This rule out using extremely large numbers for the tessellation factor, although the exact limit depends on the shape implementation.

Note that these shapes tend to duplicate rather than share vertices in order to provide the 'face-normal' lighting expected of sharp geometry.

# Threading model
Each GeometricPrimitive instance only supports drawing from one thread at a time, but you can simultaneously submit primitives on multiple threads if you create a separate GeometricPrimitive instance per Direct3D 11 deferred context.

[Immediate and Deferred Rendering](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476892.aspx)

# State management

When ``Draw`` is called,  it will set the states needed to render with the effect. Existing state is not save or restored. For efficiency, it simply sets the state it requires to render and assumes that any subsequent rendering will overwrite state that it needs.

GeometricPrimitive makes use of the following states:

* BlendState
* Constant buffer (Vertex Shader and Pixel Shader stages, slot 0)
* DepthStencilState
* Index buffer
* Input layout
* Pixel shader
* Primitive topology
* RasterizerState
* SamplerState (Pixel Shader stage, slot 0)
* Shader resources (Pixel Shader stage, slot 0)
* Vertex buffer (slot 0)
* Vertex shader

The GeometricPrimitive class assumes you've already set the Render Target view, Depth Stencil view, and Viewport.

# Remark
Tetrahedron, Cube/Hexahedron, Octahedron, Dodecahedron, and Icosahedron comprise the five [Platonic solid](https://en.wikipedia.org/wiki/Platonic_solid). The Utah Teapot is sometimes referred to as the "Sixth Platonic solid" due to its prevalence in rendering sample images.
