|[[Getting Started]]|
|---|

This lesson covers the basics of writing your own [HLSL shaders](https://wikipedia.org/wiki/High-Level_Shading_Language) and using them with _DirectX Tool Kit_, in particular to customize **SpriteBatch**.

# Setup
First create a new project using the instructions from the previous lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Creating custom shaders with HLSL

The general approach is to author your own shaders in HLSL and compile them. For this lesson, we'll focus on writing a custom pixel shader and rely on the built-in vertex shader for [[SpriteBatch]], but the same basic principles apply to all HLSL shaders: _vertex shaders_, _pixel shaders_, _geometry shaders_, _hull shaders_, _domain shaders_, and even _compute shaders_.

# Setting up our test scene

Save the file [sunset.jpg](https://github.com/Microsoft/DirectXTK/wiki/images/sunset.jpg) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item....** Select "sunset.jpg" and hit "OK".

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::CommonStates> m_states;
std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
std::unique_ptr<DirectX::GeometricPrimitive> m_shape;

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;

DirectX::SimpleMath::Matrix m_world;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_projection;

RECT m_fullscreenRect;
```

In **Game.cpp**, update the constructor:

```cpp
Game::Game() noexcept(false) :
    m_fullscreenRect{}
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
DX::ThrowIfFailed(CreateWICTextureFromFile(device,
    L"sunset.jpg", nullptr,
    m_background.ReleaseAndGetAddressOf()));

m_states = std::make_unique<CommonStates>(device);

auto context = m_deviceResources->GetD3DDeviceContext();
m_spriteBatch = std::make_unique<SpriteBatch>(context);
m_shape = GeometricPrimitive::CreateTorus(context);

m_view = Matrix::CreateLookAt(Vector3(0.f, 3.f, -3.f),
    Vector3::Zero, Vector3::UnitY);
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_fullscreenRect = size;

m_projection = Matrix::CreatePerspectiveFieldOfView(XM_PIDIV4,
    float(size.right) / float(size.bottom), 0.01f, 100.f);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_states.reset();
m_spriteBatch.reset();
m_shape.reset();
m_background.Reset();
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
auto totalTime = static_cast<float>(timer.GetTotalSeconds());

m_world = Matrix::CreateRotationZ(totalTime / 2.f)
    * Matrix::CreateRotationY(totalTime)
    * Matrix::CreateRotationX(totalTime * 2.f);
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
m_spriteBatch->Begin();
m_spriteBatch->Draw(m_background.Get(), m_fullscreenRect);
m_spriteBatch->End();

m_shape->Draw(m_world, m_view, m_projection);
```

In **Game.cpp**, modify **Clear** to remove the call to ``ClearRenderTargetView`` since we are drawing a full-screen sprite first which sets every pixel--we still need to clear the depth/stencil buffer of course:

```cpp
context->ClearDepthStencilView(depthStencil,
    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
context->OMSetRenderTargets(1, &renderTarget, depthStencil);
```

Build and run to see our initial scene.

![Screenshot of Torus](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTorus1.PNG)

<details><summary><i>Click here for troubleshooting advice</i></summary>
<p>If you get a runtime exception, then you may have the "sunset.jpg" in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on <code>CreateWICTextureFromFile</code> and step into the code to find the exact problem.</p></details>

# Compiling and loading shaders

Save the files [Bloom.hlsli](https://github.com/Microsoft/DirectXTK/wiki/shaders/Bloom.hlsli), [BloomCombine.hlsl](https://github.com/Microsoft/DirectXTK/wiki/shaders/BloomCombine.hlsl), [BloomExtract.hlsl](https://github.com/Microsoft/DirectXTK/wiki/shaders/BloomExtract.hlsl), [GaussianBlur.hlsl](https://github.com/Microsoft/DirectXTK/wiki/shaders/GaussianBlur.hlsl), and [ReadData.h](https://github.com/Microsoft/DirectXTK/wiki/ReadData.h) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item....** Select "Bloom.hlsli" and hit "OK". Repeat for "BloomCombine.hlsl", "BloomExtract.hlsl", "GaussianBlur.hlsl", and "ReadData.h".

View **Properties** on each of the three ``.hlsl`` files ("BloomCombine.hlsl", "BloomExtract.hlsl", and "GaussianBlur.hlsl") and for "All Configurations" and "All Platforms", set the "Shader Type" to "Pixel Shader (/ps)" and select "OK".

![HLSL Compiler Settings](https://github.com/Microsoft/DirectXTK/wiki/images/settingsHLSL.PNG)

In **pch.h** add after the other ``#include`` statements:

```cpp
#include "ReadData.h"
```

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
Microsoft::WRL::ComPtr<ID3D11PixelShader> m_bloomExtractPS;
Microsoft::WRL::ComPtr<ID3D11PixelShader> m_bloomCombinePS;
Microsoft::WRL::ComPtr<ID3D11PixelShader> m_gaussianBlurPS;

Microsoft::WRL::ComPtr<ID3D11Buffer> m_bloomParams;
Microsoft::WRL::ComPtr<ID3D11Buffer> m_blurParamsWidth;
Microsoft::WRL::ComPtr<ID3D11Buffer> m_blurParamsHeight;
```

At the top of the **Game.cpp** file after the using namespace statements, add the following:

```cpp
namespace
{
    struct VS_BLOOM_PARAMETERS
    {
        float bloomThreshold;
        float blurAmount;
        float bloomIntensity;
        float baseIntensity;
        float bloomSaturation;
        float baseSaturation;
        uint8_t na[8];
    };

    static_assert(!(sizeof(VS_BLOOM_PARAMETERS) % 16),
        "VS_BLOOM_PARAMETERS needs to be 16 bytes aligned");

    struct VS_BLUR_PARAMETERS
    {
        static constexpr size_t SAMPLE_COUNT = 15;

        XMFLOAT4 sampleOffsets[SAMPLE_COUNT];
        XMFLOAT4 sampleWeights[SAMPLE_COUNT];

        void SetBlurEffectParameters(float dx, float dy,
            const VS_BLOOM_PARAMETERS& params)
        {
            sampleWeights[0].x = ComputeGaussian(0, params.blurAmount);
            sampleOffsets[0].x = sampleOffsets[0].y = 0.f;

            float totalWeights = sampleWeights[0].x;

            // Add pairs of additional sample taps, positioned
            // along a line in both directions from the center.
            for (size_t i = 0; i < SAMPLE_COUNT / 2; i++)
            {
                // Store weights for the positive and negative taps.
                float weight = ComputeGaussian( float(i + 1.f), params.blurAmount);

                sampleWeights[i * 2 + 1].x = weight;
                sampleWeights[i * 2 + 2].x = weight;

                totalWeights += weight * 2;

                // To get the maximum amount of blurring from a limited number of
                // pixel shader samples, we take advantage of the bilinear filtering
                // hardware inside the texture fetch unit. If we position our texture
                // coordinates exactly halfway between two texels, the filtering unit
                // will average them for us, giving two samples for the price of one.
                // This allows us to step in units of two texels per sample, rather
                // than just one at a time. The 1.5 offset kicks things off by
                // positioning us nicely in between two texels.
                float sampleOffset = float(i) * 2.f + 1.5f;

                Vector2 delta = Vector2(dx, dy) * sampleOffset;

                // Store texture coordinate offsets for the positive and negative taps.
                sampleOffsets[i * 2 + 1].x = delta.x;
                sampleOffsets[i * 2 + 1].y = delta.y;
                sampleOffsets[i * 2 + 2].x = -delta.x;
                sampleOffsets[i * 2 + 2].y = -delta.y;
            }

            for (size_t i = 0; i < SAMPLE_COUNT; i++)
            {
                sampleWeights[i].x /= totalWeights;
            }
        }

    private:
        float ComputeGaussian(float n, float theta)
        {
            return (float)((1.0 / sqrtf(2 * XM_PI * theta))
                * expf(-(n * n) / (2 * theta * theta)));
        }
    };

    static_assert(!(sizeof(VS_BLUR_PARAMETERS) % 16),
        "VS_BLUR_PARAMETERS needs to be 16 bytes aligned");

    enum BloomPresets
    {
        Default = 0,
        Soft,
        Desaturated,
        Saturated,
        Blurry,
        Subtle,
        None
    };

    BloomPresets g_Bloom = Default;

    static const VS_BLOOM_PARAMETERS g_BloomPresets[] =
    {
        //Thresh  Blur Bloom  Base  BloomSat BaseSat
        { 0.25f,  4,   1.25f, 1,    1,       1 }, // Default
        { 0,      3,   1,     1,    1,       1 }, // Soft
        { 0.5f,   8,   2,     1,    0,       1 }, // Desaturated
        { 0.25f,  4,   2,     1,    2,       0 }, // Saturated
        { 0,      2,   1,     0.1f, 1,       1 }, // Blurry
        { 0.5f,   2,   1,     1,    1,       1 }, // Subtle
        { 0.25f,  4,   1.25f, 1,    1,       1 }, // None
    };
}
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
auto blob = DX::ReadData( L"BloomExtract.cso" );
DX::ThrowIfFailed(device->CreatePixelShader( blob.data(), blob.size(),
    nullptr, m_bloomExtractPS.ReleaseAndGetAddressOf()));

blob = DX::ReadData( L"BloomCombine.cso" );
DX::ThrowIfFailed(device->CreatePixelShader( blob.data(), blob.size(),
    nullptr, m_bloomCombinePS.ReleaseAndGetAddressOf()));

blob = DX::ReadData( L"GaussianBlur.cso" );
DX::ThrowIfFailed(device->CreatePixelShader( blob.data(), blob.size(),
    nullptr, m_gaussianBlurPS.ReleaseAndGetAddressOf()));

{
    CD3D11_BUFFER_DESC cbDesc(sizeof(VS_BLOOM_PARAMETERS),
        D3D11_BIND_CONSTANT_BUFFER);
    D3D11_SUBRESOURCE_DATA initData = { &g_BloomPresets[g_Bloom], 0, 0 };
    DX::ThrowIfFailed(device->CreateBuffer(&cbDesc, &initData,
        m_bloomParams.ReleaseAndGetAddressOf()));
}

{
    CD3D11_BUFFER_DESC cbDesc(sizeof(VS_BLUR_PARAMETERS),
        D3D11_BIND_CONSTANT_BUFFER);
    DX::ThrowIfFailed(device->CreateBuffer(&cbDesc, nullptr,
        m_blurParamsWidth.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(device->CreateBuffer(&cbDesc, nullptr,
        m_blurParamsHeight.ReleaseAndGetAddressOf()));
}
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto context = m_deviceResources->GetD3DDeviceContext();

VS_BLUR_PARAMETERS blurData = {};
blurData.SetBlurEffectParameters(1.f / (float(size.right) / 2), 0,
    g_BloomPresets[g_Bloom]);
context->UpdateSubresource(m_blurParamsWidth.Get(), 0, nullptr,
    &blurData, sizeof(VS_BLUR_PARAMETERS), 0);

blurData.SetBlurEffectParameters(0, 1.f / (float(size.bottom) / 2),
    g_BloomPresets[g_Bloom]);
context->UpdateSubresource(m_blurParamsHeight.Get(), 0, nullptr,
    &blurData, sizeof(VS_BLUR_PARAMETERS), 0);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_bloomExtractPS.Reset();
m_bloomCombinePS.Reset();
m_gaussianBlurPS.Reset();

m_bloomParams.Reset();
m_blurParamsWidth.Reset();
m_blurParamsHeight.Reset();
```

Build and run. The scene is unchanged, but we've loaded our new shaders.

<details><summary><i>Click here for troubleshooting advice</i></summary>
<p>If you get a runtime exception, then the shaders are not getting built as expected by Visual Studio. The <code>DX::ReadData</code> helper looks in the same directory as the EXE for the compiled shader files (since they are built for each configuration Debug, Release, etc.). See if the files "BloomExtract.cso", "BloomCombine.cso", and "GaussianBlur.cso" are present in the directory where the project's EXE is built. If one or more of them is missing, check the properties on each of the HLSL files as above, and double-check the general settings for those files as well.</p></details>

![HLSL Compiler Settings](https://github.com/Microsoft/DirectXTK/wiki/images/settingsHLSLg.PNG)

# Implementing a post processing effect

Save the files [RenderTexture.h](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.h) and [RenderTexture.cpp](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.cpp) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item....** Select "RenderTexture.h" and hit "OK". Repeat for "RenderTexture.cpp".

Add to the **Game.h** file to the ``#include`` section:

```cpp
#include "RenderTexture.h"
```

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DX::RenderTexture> m_offscreenTexture;
std::unique_ptr<DX::RenderTexture> m_renderTarget1;
std::unique_ptr<DX::RenderTexture> m_renderTarget2;

RECT m_bloomRect;
```

Then add the following method to the Game class's private declarations:

```cpp
void PostProcess();
```

In **Game.cpp**, update the constructor:

```cpp
Game::Game() noexcept(false) :
    m_fullscreenRect{},
    m_bloomRect{}
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);

    const auto format = m_deviceResources->GetBackBufferFormat();
    m_offscreenTexture = std::make_unique<DX::RenderTexture>(format);
    m_renderTarget1 = std::make_unique<DX::RenderTexture>(format);
    m_renderTarget2 = std::make_unique<DX::RenderTexture>(format);
}
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
m_offscreenTexture->SetDevice(device);
m_renderTarget1->SetDevice(device);
m_renderTarget2->SetDevice(device);
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
m_offscreenTexture->SetWindow(size);

// Half-size blurring render targets
m_bloomRect = { 0, 0, size.right / 2, size.bottom / 2 };

m_renderTarget1->SetWindow(m_bloomRect);
m_renderTarget2->SetWindow(m_bloomRect);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_offscreenTexture->ReleaseDevice();
m_renderTarget1->ReleaseDevice();
m_renderTarget2->ReleaseDevice();
```

In **Game.cpp**, add to **Render** just before the call to ``Present``:

```cpp
PostProcess();
```

In **Game.cpp**, modify **Clear** to use ``m_offscreenTexture`` instead of the swap chain render target view:

```cpp
auto renderTarget = m_offscreenTexture->GetRenderTargetView();
```

In **Game.cpp**, add the new method **PostProcess**

```cpp
void Game::PostProcess()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    ID3D11ShaderResourceView* null[] = { nullptr, nullptr };

    if (g_Bloom == None)
    {
        // Pass-through test
        context->CopyResource(m_deviceResources->GetRenderTarget(),
            m_offscreenTexture->GetRenderTarget());
    }
    else
    {
        // scene -> RT1 (downsample)
        auto rt1RT = m_renderTarget1->GetRenderTargetView();
        context->OMSetRenderTargets(1, &rt1RT, nullptr);
        m_spriteBatch->Begin(SpriteSortMode_Immediate,
            nullptr, nullptr, nullptr, nullptr,
            [=](){
                context->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
                context->PSSetShader(m_bloomExtractPS.Get(), nullptr, 0);
            });
        auto rtSRV = m_offscreenTexture->GetShaderResourceView();
        m_spriteBatch->Draw(rtSRV, m_bloomRect);
        m_spriteBatch->End();

        // RT1 -> RT2 (blur horizontal)
        auto rt2RT = m_renderTarget2->GetRenderTargetView();
        context->OMSetRenderTargets(1, &rt2RT, nullptr);
        m_spriteBatch->Begin(SpriteSortMode_Immediate,
            nullptr, nullptr, nullptr, nullptr,
            [=](){
                context->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
                context->PSSetConstantBuffers(0, 1,
                    m_blurParamsWidth.GetAddressOf());
            });
        m_spriteBatch->Draw(rt1SRV, m_bloomRect);
        m_spriteBatch->End();

        context->PSSetShaderResources(0, 2, null);

        // RT2 -> RT1 (blur vertical)
        context->OMSetRenderTargets(1, &rt1RT, nullptr);
        m_spriteBatch->Begin(SpriteSortMode_Immediate,
            nullptr, nullptr, nullptr, nullptr,
            [=](){
                context->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
                context->PSSetConstantBuffers(0, 1,
                    m_blurParamsHeight.GetAddressOf());
            });
        auto rt2SRV = m_renderTarget2->GetShaderResourceView();
        m_spriteBatch->Draw(rt2SRV, m_bloomRect);
        m_spriteBatch->End();

        // RT1 + scene
        auto renderTarget = m_deviceResources->GetRenderTargetView();
        context->OMSetRenderTargets(1, &renderTarget, nullptr);
        m_spriteBatch->Begin(SpriteSortMode_Immediate,
            nullptr, nullptr, nullptr, nullptr,
            [=](){
                context->PSSetShader(m_bloomCombinePS.Get(), nullptr, 0);
                context->PSSetShaderResources(1, 1, m_rt1SRV.GetAddressOf());
                context->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
            });
        m_spriteBatch->Draw(rtSRV, m_fullscreenRect);
        m_spriteBatch->End();
    }

    context->PSSetShaderResources(0, 2, null);
}
```

Build and run to see the bloom in action:

![Screenshot of Torus](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTorus2.PNG)

Change the value in **Game.cpp** for ``g_Bloom`` to "Saturated" instead of "Default":

```cpp
BloomPresets g_Bloom = Saturated;
```

Build and run to see a different set of bloom settings in action:

![Screenshot of Torus](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTorus3.PNG)

Change the value in **Game.cpp** for ``g_Bloom`` to "None" to render our original scene without bloom.

## CMake
For this tutorial, we make use of the built-in Visual Studio HLSL build rules which handles building our shaders automatically. If you are using CMake instead, then you need to build the shaders using a custom target.

```
# Build HLSL shaders
add_custom_target(shaders)

set(HLSL_SHADER_FILES BloomCombine.hlsl BloomExtract.hlsl GaussianBlur.hlsl)

set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderType "ps")
set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderModel "4_0_level_9_1")

foreach(FILE ${HLSL_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  list(APPEND CSO_SHADER_FILES ${CMAKE_BINARY_DIR}/${FILE_WE}.cso)
  get_source_file_property(shadertype ${FILE} ShaderType)
  get_source_file_property(shadermodel ${FILE} ShaderModel)
  add_custom_command(TARGET shaders
                     COMMAND fxc.exe /nologo /Emain /T${shadertype}_${shadermodel} $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo ${CMAKE_BINARY_DIR}/${FILE_WE}.cso /Fd ${CMAKE_BINARY_DIR}/${FILE_WE}.pdb ${FILE}
                     MAIN_DEPENDENCY ${FILE}
                     COMMENT "HLSL ${FILE}"
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     VERBATIM)
endforeach(FILE)

add_dependencies(${PROJECT_NAME} shaders)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${CSO_SHADER_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}>
                   COMMAND_EXPAND_LISTS)
```

## Technical notes
First the original scene is rendered to a hidden render target ``m_offscreenTexture`` as normal. The only change here was for ``Clear`` to use ``m_offscreenTexture``'s render target view rather than [[DeviceResources]] backbuffer render target view.

Our first past of post-processing is to render the original scene texture as a 'full-screen quad' onto our first half-sized render target using the custom shader in "BloomExtract.hlsl" into ``m_renderTarget1``.

```cpp
float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    float4 c = Texture.Sample(TextureSampler, texCoord);
    return saturate((c - BloomThreshold) / (1 - BloomThreshold));
}
```

![Screenshot of post-processed torus](https://github.com/Microsoft/DirectXTK/wiki/images/postprocessPass1.png)

We take the result of the extract & down-size and then blur it horizontally using "GausianBlur.hlsl" from ``m_renderTarget1`` to ``m_renderTarget2``.

```cpp
float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    float4 c = 0;

    // Combine a number of weighted image filter taps.
    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        c += Texture.Sample(TextureSampler, texCoord + SampleOffsets[i]) * SampleWeights[i];
    }

    return c;
}
```

![Screenshot of post-processed torus](https://github.com/Microsoft/DirectXTK/wiki/images/postprocessPass2.png)

We take that result in ``m_renderTarget2`` and then blur it vertically using the same shader--we are using a [Gaussian blur](https://wikipedia.org/wiki/Gaussian_blur) which is a separable filter which allows us to do the filter in two simple render passes one for each dimension--back into ``m_renderTarget1``.

![Screenshot of post-processed torus](https://github.com/Microsoft/DirectXTK/wiki/images/postprocessPass3.png)

And finally we take the result of both blur passes in ``m_renderTarget1`` and combine it with our original scene texture ``m_offscreenTexture`` using the "BloomCombine.hlsl" shader to get our final image into the presentation swapchain.

```cpp
// Helper for modifying the saturation of a color.
float4 AdjustSaturation(float4 color, float saturation)
{
    // The constants 0.3, 0.59, and 0.11 are chosen because the
    // human eye is more sensitive to green light, and less to blue.
    float grey = dot(color.rgb, float3(0.3, 0.59, 0.11));

    return lerp(grey, color, saturation);
}

float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    float4 base = BaseTexture.Sample(TextureSampler, texCoord);
    float4 bloom = BloomTexture.Sample(TextureSampler, texCoord);

    // Adjust color saturation and intensity.
    bloom = AdjustSaturation(bloom, BloomSaturation) * BloomIntensity;
    base = AdjustSaturation(base, BaseSaturation) * BaseIntensity;

    // Darken down the base image in areas where there is a lot of bloom,
    // to prevent things looking excessively burned-out.
    base *= (1 - saturate(bloom));

    // Combine the two images.
    return base + bloom;
}
```

> We use half-size-in-each-dimension render targets for the blur because it is a quarter the memory/bandwidth to render, and because we are blurring the image significantly there's no need for the 'full' resolution. Other kinds of post-process effects may require more fidelity in the temporary buffers.

One final note, because we are using a render target that is larger than our blur buffers, we do not need to use ``RSSetViewports`` as we change render targets. If our intermediate render targets were _larger_ than our backbuffer, then we'd need to call ``RSSetViewports`` after calling ``OMSetRenderTargets`` whenever we changed sizes.

# More to explore

* See [[PostProcess]] for a Bloom example using the built-in post-processing shaders. *If you are targeting Direct3D Hardware Feature Level 9.x, the PostProcess class is not supported for you so you should use the sprite batch solution above.*

* See  [[BasicPostProcess]] and [[DualPostProcess]] for additional built-in post-processing effects.

**Next lessons:** [[Authoring an Effect]]

# Further reading

DirectX Tool Kit docs [[SpriteBatch]]

[HLSL, FXC, and D3DCompile](https://walbourn.github.io/hlsl-fxc-and-d3dcompile/)  
[What's up with D3DCompiler_xx.dll](https://walbourn.github.io/whats-up-with-d3dcompiler_xx-dll)  
[Compiling Shaders](https://docs.microsoft.com/windows/win32/direct3dhlsl/dx-graphics-hlsl-part1)  

# Credits

I borrowed heavily from the _XNA Game Studio_ [Bloom Postprocess](https://github.com/SimonDarksideJ/XNAGameStudio/wiki/Bloom-Postprocess) sample for this lesson.
