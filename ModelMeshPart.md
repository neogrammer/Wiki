This class is part of the [[Model]] hierarchy. The purpose of this class is to be container for a 'submesh' which is referenced by an instance of the [[ModelMesh]] class. A 'submesh' is a portion of a mesh that is all drawn using the same 'material' settings as reflected by an [[Effects]] instance. A submesh is drawn in a single Direct3D 11 ``Draw`` call using a single effect.

_Note that ModelMesh uses a collection of ``std::unique_ptr`` instances to ModelMeshPart since submeshes are not typically shared between mesh instances. Submeshes can easily share Direct3D 11 resources (index buffers, vertex buffers, effects, etc.)._

# Header
    #include <Model.h>

# Initialization
ModelMeshPart instances are typically created by a Model loader along with the ModelMesh instances that contain the submeshes.

# Data
All members of ModelMeshPart are public to facilitate writing custom model loaders and sophisticated rendering solutions. Care should be taken whenever modifying any of these elements as they have interdependencies, and can ultimately be referenced by numerous instances of Model via shared ModelMesh instances.

* _indexCount_ - Number of indices in the index buffer associated with this submesh. This is used as the first parameter to Direct3D11's [DrawIndexed](http://msdn.microsoft.com/en-us/library/ff476409.aspx) method.
* _startIndex_ - The index to start drawing with for this submesh. This facilitates sharing index buffers between multiple submeshes. This is used as the second parameter to Direct3D11's ``DrawIndexed`` method.
* _vertexOffset_ - base vertex location which is added to each index in the index buffer before referencing the vertex buffer. This facilitates sharing of vertex buffers between multiple submeshes. This is used as the third parameter to Direct3D11's ``DrawIndexed`` method.
* _vertexBuffer_ - COM smart pointer to the vertex buffer associated with this submesh. This is used when calling Direct3D 11's [IASetVertexBuffers](http://msdn.microsoft.com/en-us/library/ff476456.aspx) method. This vertex buffer can be shared by multiple submeshes or even other meshes.
* _vertexStride_ - Stride in bytes for each vertex in the vertex buffer associated with this submesh. This is used when calling Direct3D 11's ``IASetVertexBuffers`` method.
* _indexBuffer_ - COM smart pointer to the index buffer associated with this submesh. This is used when calling Direct3D11's [IASetIndexBuffer](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476453.aspx) method. This index buffer can be shared by multiple submeshes or even other meshes.
* _indexFormat_ - Format of the index buffer. For 16-bit index buffers, this is set to ``DXGI_FORMAT_R16_UINT``. For 32-bit index buffers, this is set to ``DXGI_FORMAT_R32_UINT``. This is used when calling Direct3D11's ``IASetIndexBuffer`` method.
* _primitiveType_ - Topology for the primitives in the submesh. It is usually set to ``D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST`` with each three indices in the index buffer forming a triangle. This is used when calling Direct3D11's [IASetPrimitiveTopology](http://msdn.microsoft.com/en-us/library/ff476455.aspx) method.
* _inputLayout_ - COM smart pointer to the input layout associated with this submesh. This is used when calling Direct3D11's [IASetInputLayout](http://msdn.microsoft.com/en-us/library/ff476454.aspx) method. This must correctly match the signature of the effect and match the vertex data format in the vertex buffer.
* _effect_ - An [[IEffect|Effects]] instance which is used to set up the proper constant buffer and shader states before drawing the submesh.

# Metadata
The _ModelMeshPart::isAlpha_ boolean indicates if the ModelMeshPart instance's effect makes use of alpha-blending. This information is used by ModelMesh::Draw to ensure that opaque effects are applied before transparent effects, and the depth buffer is used accordingly.

The _ModelMeshPart::vbDecl_ contains the input layout element descriptor needed for creating input layouts that match this submeshes vertex buffer and effect instance. This information is used by **ModelMeshPart::CreateInputLayout** for setting up custom effect overrides and **ModelMeshPart::ModifyEffect** when changing the effect instance used by the submesh.

# Simple drawing
Use the ``Model::Draw`` function which will call ``ModelMesh::Draw`` on all the meshes it contains, which in turn will call **ModelMeshPart::Draw** on each submesh part.

# Advanced drawing
The **ModelMeshPart::Draw** method is used to draw each individual submesh. **ModelMeshPart::Draw** can be used to implement complex 'scene graph' policies (such as sorting all ModelMeshPart instance in a scene by [[Effects]] to minimize draw overhead or rough-sorting by bounding volume front-to-back for opaque parts and back-to-front for alpha blended parts), or can be used to temporarily override the effect instance being used to draw the mesh (for example when drawing shadows).

    void Draw(ID3D11DeviceContext* deviceContext,
        IEffect* ieffect, ID3D11InputLayout* iinputLayout,
        std::function<void __cdecl()> setCustomState = nullptr) const;

You can also perform instanced drawing, although this scenario requires custom shader state to get interesting results:

    void DrawInstanced(ID3D11DeviceContext* deviceContext,
        IEffect* ieffect, ID3D11InputLayout* iinputLayout,
        uint32_t instanceCount, uint32_t startInstanceLocation = 0,
        std::function<void __cdecl()> setCustomState = nullptr) const;

To support using custom effect instances, **ModelMeshPart::CreateInputLayout** provides the ability to create a new input layout with a signature that matches the submesh's vertex buffer and the new custom effect instance.

    // An example of using a single custom effect when drawing all the parts of a Model
    IEffect* newEffect = ... (device)

    // Creating input layouts is expensive, so it shouldn't be done every frame
    std::vector<Microsoft::WRL::ComPtr<ID3D11InputLayout>> newInputLayouts;
    for( auto mit = tiny->meshes.cbegin(); mit != tiny->meshes.cend(); ++mit )
    {
        auto mesh = it->get();
        assert( mesh != 0 );

        mesh->PrepareForRendering(context, states);

        for( auto it = mesh->meshParts.cbegin(); it != mesh->meshParts.cend(); ++it )
        {
            auto part = it->get();
            assert( part != 0 );

            Microsoft::WRL::ComPtr<ID3D11InputLayout> il;
            part->CreateInputLayout( device.Get(), newEffect, il.GetAddressOf() );

            newInputLayouts.emplace_back(il);
        }
    }

    ...

    // Draw Model with custom effect override
    auto imatrices = dynamic_cast<IEffectMatrices*>( newEffect );
    if ( imatrices )
    {
        imatrices->SetWorld( world );
        imatrices->SetView( view );
        imatrices->SetProjection( projection );
    }

    size_t count = 0;
    for( auto mit = tiny->meshes.cbegin(); mit != tiny->meshes.cend(); ++mit )
    {
        auto mesh = mit->get();
        assert( mesh != 0 );

        for( auto it = mesh->meshParts.cbegin(); it != mesh->meshParts.cend(); ++it )
        {
            auto part = it->get();
            assert( part != 0 );

            // Could call if a custom transformation was desired for each part
            // if (imatrices) imatrices->SetWorld( local ) 

            part->Draw( deviceContext.Get(), newEffect,
                newInputLayouts[ count++ ].Get() );
         }
    }

> The ``ModelMesh::PrepareForRendering`` helper will only set up a texture sampler in slots 0 and 1. If you are using an [[Effects]] instance with more two textures, you need to set additional samplers manually.

# Effects Control
A Model loader is responsible for creating [[Effects]] instances via an [[IEffectFactory|EffectFactory]] so that the loaded Model is fully initialized. However, a developer may want to modify a Model's effect after it has been loaded. All the members of ModelMeshPart are public so this can be done directly, but must be done carefully.

If the effect instance is changed, the _inputLayout_ must be updated as well. Furthermore, Model maintains a cache of unique effect instances in all ModelMesh instances it contains, so any change to the ModelMeshPart instances must invalidate this cache. This is done by calling ``Model::Modified``. Keep in mind that since ModelMesh instances are shared, modification can impact multiple instances of a Model and all impacted Model instances need to have their effect cache invalidated to avoid crashes or lingering references to the wrong effect instances.

    IEffect* newEffect = ... (device)

    ModelMeshPart* part = ...

    // This will regenerate the inputLayout and needs to indicate
    // if newEffect is alpha-blended or not
    part->ModifyEffect( device.Get(), newEffect, false );

    // part comes from a Model instance 'model' and we assume it's
    // not shared by another Model instance here
    model->Modified();

# Optimization
Each submesh is drawn as a single Direct3D 11 ``DrawIndexed`` call, so it constitutes a draw batch. For best performance, each ModelMesh should have the fewest number of ModelMeshPart instances as this one of the more direct impacts on runtime performance. Larger draw batches are generally more efficient than small ones, but keep the Feature Level limits on primitive count, index format, and maximum index buffer size in mind.

# Feature Level Notes
If any ModelMeshPart makes use of 32-bit indices (i.e. _ModelMeshPart:: indexFormat_ equals ``DXGI_FORMAT_R32_UINT``) rather than 16-bit indices (``DXGI_FORMAT_R16_UINT``), then that model requires Feature Level 9.2 or greater.

If any ModelMeshPart uses adjacency (i.e. _ModelMeshPart::primitiveType_ equals ``D3D_PRIMITIVE_TOPOLOGY_*_ADJ``), then that model requires Feature Level 10.0 or greater. If using tessellation (i.e. ``D3D_PRIMITIVE_TOPOLOGY_?_CONTROL_POINT_PATCHLIST``), then that model requires Feature Level 11.0 or greater.

Keep in mind that there are maximum primitive count limits per ModelMeshPart based on feature level as well (65535 for Feature Level 9.1, 1048575 or Feature Level 9.2 and 9.3, and 4294967295 for Feature Level 10.0 or greater).

Remember that instanced drawing require Feature Level 9.3 or later.