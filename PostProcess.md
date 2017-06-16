Post-processing is a common technique applied to 3D rendering to achieve various effects. It's image-based, so it's performance is based on the size of the 2D render target rather than the complexity of the scene. The post-processing implementation in the DirectX Tool Kit implements common effects like monochrome and bloom, as well tone mapping which is essential to High Dynamic Range (HDR) rendering.

* [[BasicPostProcess]] supports post-processing that takes a single input texture such as monochrome conversion or blurring.
* [[DualPostProcess]] supports post-processing that operates on two images such as merging/blending.
* [[ToneMapPostProcess]] supports tone-map operations for HDR images such as the Reinhard operator. It also supports the HDR10 signal preparation needed for true 4k UHD wide color gamut rendering.

# Header

    #include "PostProcess.h"

# Initialization

The built-in post-processors typically require only the device.

    postProcess = std::make_unique<BasicPostProcess>(device);

# Usage

To make use of post-processing, you typically render the scene to a offscreen render texture.

    CD3D11_TEXTURE2D_DESC sceneDesc(
        DXGI_FORMAT_R16G16B16A16_FLOAT, width, height,
        1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

    DX::ThrowIfFailed(
       device->CreateTexture2D(&sceneDesc, nullptr, sceneTex.GetAddressOf())
    );

    DX::ThrowIfFailed(
        device->CreateShaderResourceView(m_sceneTex.Get(), nullptr, sceneSRV.ReleaseAndGetAddressOf())
    );

    DX::ThrowIfFailed(
        device->CreateRenderTargetView(m_sceneTex.Get(), nullptr, sceneRT.ReleaseAndGetAddressOf()
    ));

Instead of rendering to the usual render target that is created as part of the DXGI swap chain, you set the offscreen texture as your scene texture:

    context->ClearRenderTargetView(sceneRT.Get(), color);
    context->ClearDepthStencilView(...);
    context->OMSetRenderTargets(1, sceneRT.GetAddressOf(), depthStencilView.Get());

Then you render the scene as normal. When the scene is fully rendered, you then change the render target and use the previously generated render texture as your source texture (and you don't use a depth/stencil buffer for the post-processing):

    context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
    postProcess->SetSourceTexture(sceneSRV.Get());
    postProcess->Process(context);

In some cases, you will perform several post-processing passes between various off-screen render targets before applying the final pass to the swapchain render target for presentation.

# Interface

The post-processing system provides a ``IPostProcess`` interface to simplify use. The only method in this interface is ``Process`` which is expected to execute the post-processing pass with the result placed in the currently bound render target.

        void Process(ID3D11DeviceContext* deviceContext, std::function<void __cdecl()> setCustomState = nullptr);

# Remarks



# Feature level usage

The built-in post-processing shaders rely on Direct3D hardware feature level 10.0 or greater. This allows the vertex shader to self-generate a full-screen rectangle without requiring any vertex buffer to be bound using the built-in ``SV_VertexId`` system value. For this reason, the Windows Phone 8.1 projects do not include the source for the post-processing implementation as Windows Phone 8.1 hardware only supports Feature level 9.3. Applications should either have a minimum supported feature level of 10.0 or greater, or provide a run-time fallback (perhaps turning off post-processing effects or making use of simplified effects via [[SpriteBatch]] instead).

[Direct3D feature levels](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476876.aspx)

# Threading model

Post-processing typically takes place on the immediate context typically after the full scene has been rendered, so it should be done in series rather than being multi-threaded.

[Immediate and Deferred Rendering](http://msdn.microsoft.com/en-us/library/windows/desktop/ff476892.aspx)

# State management

When ``Process`` is called on an effect, it will set the states needed to render and perform the operation by drawing a full-screen rectangle. Existing state is not save or restored. For efficiency, it simply sets the state it requires to render and assumes that any subsequent rendering will overwrite state that it needs.

Most post-process effects make use of the following states:

    * BlendState
    * Constant buffer (Pixel Shader stage, slot 0)
    * DepthStencilState
    * Pixel shader
    * Primitive topology
    * RasterizerState
    * SamplerState (Pixel Shader stage, slot 0)
    * Shader resources (Pixel Shader stage, slot 0; optionally slot 1)
    * Vertex shader

# Further reading

[Video post-processing](https://en.wikipedia.org/wiki/Video_post-processing)  
[tone mapping](https://en.wikipedia.org/wiki/Tone_mapping) 