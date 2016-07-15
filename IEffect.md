This is the abstract interfaced implemente by all effects in _DirectX Tool Kit_, and is typically used as the 'base' reference. For example, [[ModelMeshPart]] defines a reference to an effect as 

    std::shared_ptr<IEffect> effect;

# Drawing
The **Apply** method is used to set up a device context for drawing with the effect. This includes setting up the appropriate vertex and pixel shaders (and potentially other shader stages as well), and updating and setting the required constant buffer(s) consumed by these shaders.

The caller is assumed to handle setting the appropriate input layout, vertex and index buffer, primitive topology, blend state, depth-stencil state, and rasterizer state. Texturing effects will also set the shader resources needed for the texturing modes it supports, but assumes the caller has set up the appropriate samplers.

_Note:_ [[GeometricPrimitive]] and [[Model]] set the various state settings to reasonable defaults as part of their prepare and/or drawing methods. [[PrimitiveBatch]] only sets vertex buffer, index buffer, primitive topology leaving the rest of the device state vector up to the caller.

    CommonStates states(device);

    ...

    deviceContext->OMSetBlendState(state.Opaque(), nullptr, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(states.DepthDefault(), 0);
    deviceContext->RSSetState( states.CullCounterClockwise() );

    auto samplerState = states.LinearClamp();
    deviceContext->PSSetSamplers(0, 1, &samplerState);

    ...

    effect->Apply( devicecontext );

    deviceContext->IASetInputLayout(inputLayout);

    auto vertexBuffer = mVertexBuffer.Get();
    UINT vertexStride = sizeof(VertexType);
    UINT vertexOffset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer,
        &vertexStride, &vertexOffset);

    deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext->DrawIndexed(mIndexCount, 0, 0);

# Input Layouts
Direct3D 11 requires that an input layout is used that matches the signature of the vertex shader used for rendering.  Since effect instances are dynamic and use many different shaders depending on various settings, this requires that the caller have access to the correct vertex shader code to create the input layout. The **GetVertexShaderBytecode** method handles this request.

    // Setup effect and parameters

    ...

    void const* shaderByteCode;
    size_t byteCodeLength;

    effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    device->CreateInputLayout(VertexPositionNormalTexture::InputElements,
                              VertexPositionNormalTexture::InputElementCount,
                              shaderByteCode, byteCodeLength,
                              pInputLayout);

Some dynamic state changes in an effect only impact contstant buffer content, which is easily updated.  Other state changes can result in a different shader permutation, in which case the the associated input layouts may no longer bind (you'll get a Direct3D 11 error at runtime if this occurs).

For the built-in effects, the trigger for needing to create a new layout would be:

* Enabling or disabling lighting (which requires a vertex normal)
* Enabling or disabling per vertex color (which requires a vertex color value)
* Enabling or disabling textures (which requires vertex texture coordinates)
* Changing effect class (BasicEffect <-> SkinnedEffect or DGSLEffect)

# Built-in Effect Notes

All built-in effects require the ``SV_Position`` semantic.

## AlphaTestEffect
This effect requires ``TEXCOORD0``, and ``COLOR`` if per-vertex colors are enabled. 

## BasicEffect
This effect requires ``NORMAL`` if lighting is enabled, ``COLOR`` if per-vertex colors are enabled, and ``TEXCOORD0`` if texturing is enabled.

## DGSLEffect
This effect requires ``NORMAL``, ``TANGENT``, and ``TEXCOORD0``. If _enableSkinning_ is true, it also requires ``BLENDINDICES`` and ``BLENDWEIGHT``.

## DualTextureEffect
This effect requires ``TEXCOORD0`` and ``TEXCOORD1``.

## EnvironmentMapEffect
This effect requires ``NORMAL`` and ``TEXCOORD0``.

## NormalMapEffect
This effect requires ``NORMAL`` , ``TEXCOORD0`` and ``TANGENT``. If per-vertex colors are enabled, it also requires ``COLOR``.

## SkinnedEffect
This effect requires ``NORMAL``, ``TEXCOORD0``, ``BLENDINDICES`` and ``BLENDWEIGHT``.
