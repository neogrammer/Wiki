This is a helper for drawing simple geometric shapes including texture coordinates and surface normals.

* Cube (aka hexahedron)
* Sphere
* Geodesic Sphere
* Cylinder
* Cone
* Torus
* Tetrahedron
* Octahedron
* Dodecahedron
* Icosahedron
* Teapot

![GeometricPrimitive Example](https://github.com/Microsoft/DirectXTK/wiki/images/GeometricPrimitiveExample.png]

**Related tutorial:** [[3D shapes]]

# Header
    #include <GeometricPrimitive.h>

# Initialization
The GeometryPrimitive class must be created from a factory method which takes the Direct3D 11 device context.

    std::unique_ptr<GeometricPrimitive> shape(
    GeometricPrimitive::CreateTeapot(deviceContext) );

For exception safety, it is recommended you make use of the C++ RAII pattern and use a ``std::unique_ptr``.

* **CreateCube**( deviceContext, float size = 1)
* **CreateSphere**( deviceContext, float diameter = 1, size_t tessellation = 16)
* **CreateGeoSphere**( deviceContext, float diameter = 1, size_t tessellation = 3)
* **CreateCylinder**( deviceContext, float height = 1, float diameter = 1, size_t tessellation = 32)
* **CreateCone**( deviceContext, float diameter = 1, float height = 1, size_t tessellation = 32)
* **CreateTorus**( deviceContext, float diameter = 1, float thickness = 0.333f, size_t tessellation = 32)
* **CreateTetrahedron**( deviceContext, float size = 1)
* **CreateOctahedron**( deviceContext, float size = 1)
* **CreateDodecahedron**( deviceContext, float size = 1)
* **CreateIcosahedron**( deviceContext, float size = 1)
* **CreateTeapot**( deviceContext, float size = 1, size_t tessellation = 8)

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

The **Draw** operation will only set up a texture sampler in slot 0. If you are using an [Effects]] instance with more than a single texture, you need to set additional samplers manually (perhaps via ``setCustomState`` callbacks). For example, if using _EnvironmentMapEffect_:

    shape->Draw( myeffect, inputLayout.Get(), false, false, [=]
    {
        ID3D11SamplerState* samplerState[2] = { states.LinearClamp(), states.LinearWrap() };
        deviceContext->PSSetSamplers(0, 2, &samplerState);
    });

_Note that GeometricPrimitive shapes define a single set of texture coordinates, so you can't use them with _DualTextureEffect_. They also do not include tangents or bi-normals, so they don't work with _DGSLEffect_._

! Coordinate systems
These geometric primitives (based on the XNA Game Studio conventions) use right-handed coordinates. They can be used with left-handed coordinates by setting the _rhcoords_ parameter on the factory methods to 'false' to reverse the winding ordering (the parameter defaults to 'true').

    std::unique_ptr<GeometricPrimitive> shape(
        GeometricPrimitive::CreateTeapot( deviceContext, 1.f, 8, false ) );

_Note: Using the wrong value for rhcoords for your viewing setup will result in the objects looking 'inside out'._

# Alpha blending
Alpha blending defaults to using premultiplied alpha. To make use of 'straight' alpha textures, override the blending mode via the optional callback:

    CommonStates states(device);

    shape->Draw(world, view, projection, Colors::White, catTexture, false, [=]
    {
        deviceContext->OMSetBlendState( states.NonPremultiplied(), nullptr, 0xFFFFFFFF);
    });

# Feature Level Notes
In order to support [all feature levels](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476876.aspx), the GeometricPrimitive implementation make use of 16-bit indices (``DXGI_FORMAT_R16_UINT``) which limits to a maximum of 65535 vertices.

Feature Level 9.1 is also limited to a maximum of 65535 primitives in a single draw. This rule out using extremely large numbers for the tessellation factor, although the exact limit depends on the shape implementation.

Note that these shapes tend to duplicate rather than share vertices in order to provide the 'face-normal' lighting expected of sharp geometry.

# Threading model
Each GeometricPrimitive instance only supports drawing from one thread at a time, but you can simultaneously submit primitives on multiple threads if you create a separate GeometricPrimitive instance per Direct3D 11 deferred context.

[Immediate and Deferred Rendering](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476892.aspx)

# Remark
[Tetrahedron](http://en.wikipedia.org/wiki/Tetrahedron), [Cube](http://en.wikipedia.org/wiki/Cube)/[Hexahedron](http://en.wikipedia.org/wiki/Hexahedron), [Octahedron](http://en.wikipedia.org/wiki/Octahedron), [Dodecahedron](http://en.wikipedia.org/wiki/Dodecahedron), and [Icosahedron](http://en.wikipedia.org/wiki/Icosahedron) comprise the five "Platonic solids". The [Utah Teapot](http://en.wikipedia.org/wiki/Utah_teapot) is sometimes referred to as the "Sixth [Platonic solid](https://en.wikipedia.org/wiki/Platonic_solid)" due to its prevalence in rendering sample images.

The [sphere](http://en.wikipedia.org/wiki/Sphere), [cylinder](http://en.wikipedia.org/wiki/Cylinder_(geometry)), [cone](http://en.wikipedia.org/wiki/Cone), and [torus](http://en.wikipedia.org/wiki/Torus) are very common 3D graphics objects as well.
