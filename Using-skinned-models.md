In this lesson we learn how to render models using skinned vertices for smooth animations.

# Setup
First create a new project using the instructions from the previous lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Background
In the [[previous lesson|Animating using model bones]], we learned the use of [[ModelBone]] as general metadata and for doing rigid-body animation of meshes. The other common use for model bones is as a 'skeleton' to control vertices for [vertex skinning](https://en.wikipedia.org/wiki/Skeletal_animation) animation.

Like *XNA Game Studio*, the *DirectX Tool Kit* does not provide a specific implementation of an animation control system. There are numerous approaches, and each have their own strengths and weaknesses. In this lesson, we'll cover the animation system in the Visual Studio Mesh Content Pipeline (``CMO``) and the legacy DirectX SDK sample animation system (``SDKMESH`` v1), providing some basic code for using them. Neither solution is ideal, but they do a good job of demonstrating the techniques and some trade-offs.

# Rendering a skinned model

Start by saving [teapot.cmo](https://github.com/Microsoft/DirectXTK/wiki/media/teapot.cmo) into your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "teapot.cmo" and click "OK".

> If you are using a Universal Windows Platform app or Xbox project rather than a Windows desktop app, you need to manually edit the Visual Studio project properties on the ``teapot.cmo`` file and make sure "Content" is set to "Yes" so the data file will be included in your packaged build.

Now download [Animation.cpp](https://github.com/Microsoft/DirectXTK/wiki/Animation.cpp) and [Animation.h](https://github.com/Microsoft/DirectXTK/wiki/Animation.h) into your project directory. Use **Project** / **Add Existing Item...** to add them to your project. *This is a simple helper class we'll describe more later on in the lesson.*

In the **Game.h** file, add to the list of header includes:

```cpp
#include "Animation.h"
```

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::SimpleMath::Matrix m_world;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;

std::unique_ptr<DirectX::CommonStates> m_states;
std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
std::unique_ptr<DirectX::Model> m_model;

DirectX::ModelBone::TransformArray m_drawBones;

DX::AnimationCMO m_animation;
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
m_states = std::make_unique<CommonStates>(device);

m_fxFactory = std::make_unique<EffectFactory>(device);

size_t animsOffset;
m_model = Model::CreateFromCMO(device, L"teapot.cmo",
    *m_fxFactory,
    ModelLoader_CounterClockwise | ModelLoader_IncludeBones,
    &animsOffset);

DX::ThrowIfFailed(
    m_animation.Load(L"teapot.cmo", animsOffset)
);
m_animation.Bind(*m_model);

m_drawBones = ModelBone::MakeArray(m_model->bones.size());

m_model->UpdateEffects([&](IEffect* effect)
    {
        auto skin = dynamic_cast<SkinnedEffect*>(effect);
        if (skin)
        {
            skin->SetPerPixelLighting(true);
        }
    });

m_world = Matrix::Identity;
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
static const XMVECTORF32 c_cameraPos = { 100.f, 100.f, 200.f, 0.f };
static const XMVECTORF32 c_lookAt = { 0.f, 25.f, 0.f, 0.f };

auto size = m_deviceResources->GetOutputSize();
m_view = Matrix::CreateLookAt(c_cameraPos.v, c_lookAt.v, Vector3::UnitY);
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(size.right) / float(size.bottom), 0.1f, 1000.f);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_states.reset();
m_fxFactory.reset();
m_model.reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
size_t nbones = m_model->bones.size();

m_animation.Apply(*m_model, nbones, m_drawBones.get());

m_model->DrawSkinned(context, *m_states, nbones, m_drawBones.get(),
    m_world, m_view, m_proj);
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
float time = float(timer.GetTotalSeconds());

m_world = XMMatrixRotationY(time);
```

Build and render to see a dancing teapot:

![Screenshot dancing teapot](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotAnimatingTeapot.PNG)

# CMO animation

> **UNDER CONSTRUCTION**

# SDKMESH animation

Now save [soldier.sdkmesh](https://github.com/Microsoft/DirectXTK/wiki/media/soldier.sdkmesh), [head_diff.dds](https://github.com/Microsoft/DirectXTK/wiki/media/head_diff.dds), [head_norm.dds](https://github.com/Microsoft/DirectXTK/wiki/media/head_norm.dds), [jacket_diff.dds](https://github.com/Microsoft/DirectXTK/wiki/media/jacket_diff.dds), [jacket_norm.dds](https://github.com/Microsoft/DirectXTK/wiki/media/jacket_norm.dds), [pants_diff.dds](https://github.com/Microsoft/DirectXTK/wiki/media/pants_diff.dds), [pants_norm.dds](https://github.com/Microsoft/DirectXTK/wiki/media/pants_norm.dds), [upBody_diff.dds](https://github.com/Microsoft/DirectXTK/wiki/media/upBody_diff.dds), and [upbody_norm.dds](https://github.com/Microsoft/DirectXTK/wiki/media/upbody_norm.dds) into your new project's directory. From the top menu select **Project** / **Add Existing Item...**, then select "soldier.sdkmesh" and the eight  ``DDS`` files followed by "OK".

> If you are using a Universal Windows Platform app or Xbox project rather than a Windows desktop app, you need to manually edit the Visual Studio project properties on the ``soldier.sdkmesh`` file and make sure "Content" is set to "Yes" so the data file will be included in your packaged build.

> **UNDER CONSTRUCTION**

# More to explore

* Vertex skinning is also supported by [[SkinnedNormalMapEffect|NormalMapEffect]], [[SkinnedPBREffect|PBREffect]], and [[SkinnedDGSLEffect|DGSLEffect]] using the ``IEffectSkinning`` interface just as with ``SkinnedEffect``.

**Next lesson:** [[Using advanced shaders]]
