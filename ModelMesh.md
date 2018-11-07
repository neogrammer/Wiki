This class is part of the [[Model]] hierarchy. The primary purpose of this class is to be a shared container for a list of [[ModelMeshPart]] instances which are referenced by one or more instances of the Model class.

Note that Model uses a collection of ``std::shared_ptr`` instances to ModelMesh since meshes can be shared by multiple instances of Model.

# Header
```cpp
#include <Model.h>
```

# Initialization
ModelMesh instances are typically created by a Model loader along with the ModelMeshPart instances that make up the mesh.

# Simple drawing
Use the ``Model::Draw`` function which will call **ModelMesh::Draw** on all the meshes it contains. See [[Model]] for an example.

# Advanced drawing
The **ModelMesh::Draw** method draws the mesh in two passes. In the first pass, all 'opaque' ModelMeshPart instances are drawn (i.e. _ModelMeshPart::isAlpha_ is false), and in the second pass all 'alpha' instances are drawn (i.e. _ModelMeshPart::isAlpha_ is true).

The Draw method assumes that the proper blend state, depth/stencil state, rasterizer state, and sampler state have been set up before being called. The **ModelMesh::PrepareForRendering** method can be used to set up standard defaults via [[CommonStates]] or it can be skipped in favor of custom state setting.

* The opaque pass is drawn using _Opaque_ blending, _DepthDefault_ sorting, an optional wireframe mode or a winding mode based on _ModelMesh::ccw_.
* The alpha pass is drawn using alpha blending, _DepthRead_ sorting, an optional wireframe mode or a winding mode based on _ModelMesh::ccw_. The choice of  alpha blending is based on _ModelMesh::pmalpha_ to select between _AlphaBlend_ and _NonPremultiplied_.

``ModelMesh::Draw`` can be used to implement alternate 'scene graph' policies.

```cpp
// Rather than draw each model's opaque and then alpha parts in turn, this version
// draws all the models' opaque parts first then all the alpha parts second which
// can be important for some complex scenes.

std::list<std::unique_ptr<Model>> models;

...

// Draw opaque parts
for( auto mit = models.cbegin(); mit != models.cend(); ++mit )
{
    auto model = mit->get();
    assert( model != 0 );

    for( auto it = model->meshes.cbegin(); it != model->meshes.cend(); ++it )
    {
        auto mesh = it->get();
        assert( mesh != 0 );

        mesh->PrepareForRendering( deviceContext, states, false );

        // Do model-level setCustomState work here

        mesh->Draw( deviceContext, world, view, projection, false );
    }
}

// Draw alpha parts (should really be done in back-to-front sorted order)
for( auto mit = models.cbegin(); mit != models.cend(); ++mit )
{
    auto model = mit->get();
    assert( model != 0 );

    for( auto it = model->meshes.cbegin(); it != model->meshes.cend(); ++it )
    {
        auto mesh = it->get();
        assert( mesh != 0 );

        mesh->PrepareForRendering( deviceContext, states, true );

        // Do model-level setCustomState work here

        mesh->Draw( deviceContext, world, view, projection, true );
    }
}
```

# Metadata
In addition to the list of ModelMeshPart instances that make up the mesh, a ModelMesh also includes a _name_ (a wide-character string) for tracking and application logic.

It includes a bool to indicate if the mesh should be rendered using counter-clockwise winding or clockwise winding (_ccw_), as well as a bool to indicate if the mesh should be rendered with premultiplied alpha blending or 'straight' alpha blending (_pmalpha_).

A ModelMesh also includes bounding information for culling & collision detection in the form of a [BoundingSphere](http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.directxcollision.boundingsphere.aspx) and a [BoundingBox](https://docs.microsoft.com/en-us/windows/desktop/api/directxcollision/ns-directxcollision-boundingbox).

The choice of frame-of-reference for these bounding volumes is up to the Model loader, but is typically in 'local' coordinates.
