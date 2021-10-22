In this lesson we learn how to use model bones for rigid-body animation.

# Setup

First create a new project using the instructions from the previous lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Background

Most 3D modeling packages (a.k.a. Digital Content Creation DCC tools) create a relationship between the different parts of 3D models. The process of assigning this hierarchy is  called *rigging* the model for animation. This hierarchy can be exported to runtime geometry formats like ``SDKMESH``,then loaded into *DirectX Tool Kit* as an array of [[ModelBone]] structures that link to the parent, children, and siblings of each bone. There's also a local transformation matrix associated with each bone.

These 'bone' locations can be used as simple metadata for placing dynamic lights, collision geometry, or other game-related logic in an art-driven fashion. They can also be used to implement rigid-body animation, which is what we will implement in this lesson.

# Rendering a test scene

Start by saving [tank.sdkmesh](https://github.com/Microsoft/DirectXTK/wiki/media/tank.sdkmesh), [engine_diff_tex.dds](https://github.com/Microsoft/DirectXTK/wiki/media/engine_diff_tex.dds), and [turret_alt_diff_tex.dds](https://github.com/Microsoft/DirectXTK/wiki/media/turret_alt_diff_tex.dds) into your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "tank.sdkmesh" and click "OK". Then repeat for the two ``DDS`` texture files.

> If you are using a Universal Windows Platform app or Xbox project rather than a Windows desktop app, you need to manually edit the Visual Studio project properties on the ``tank.sdkmesh`` file and make sure "Content" is set to "Yes" so the data file will be included in your packaged build.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::SimpleMath::Matrix m_world;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;

std::unique_ptr<DirectX::CommonStates> m_states;
std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
std::unique_ptr<DirectX::Model> m_model;
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
m_states = std::make_unique<CommonStates>(device);

m_fxFactory = std::make_unique<EffectFactory>(device);

m_model = Model::CreateFromSDKMESH(device, L"tank.sdkmesh",
    *m_fxFactory,
    ModelLoader_CounterClockwise | ModelLoader_IncludeBones);

m_world = Matrix::Identity;
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_view = Matrix::CreateLookAt(Vector3(1000, 500, 0),
    Vector3(0, 150, 0), Vector3::UnitY);
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(size.right) / float(size.bottom), 0.1f, 10000.f);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_states.reset();
m_fxFactory.reset();
m_model.reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
m_model->Draw(context, *m_states, m_world, m_view, m_proj);
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
float time = float(timer.GetTotalSeconds());

m_world = XMMatrixRotationY(time * 0.1f);    
```

Build an run to see... well, something:

![Screenshot of messed up tank model](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTank1.png)

> This is more or less the same setup as [[Rendering a model]] with the addition of ``ModelLoader_IncludeBones``.

# Rendering with model bones

The reason the tank doesn't look right is that the individual meshes are set up for rendering with the model bone information. The default ``Draw`` method does not use them, so everything is just dropped around the scene's origin.

So now let's add rendering using the model bones. In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::ModelBone::TransformArray m_drawBones;
DirectX::ModelBone::TransformArray m_animBones;
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources** (after you have loaded the model):

```cpp
const size_t nbones = m_model->bones.size();

m_drawBones = ModelBone::MakeArray(nbones);
m_animBones = ModelBone::MakeArray(nbones);

m_model->CopyBoneTransformsTo(nbones, m_animBones.get());
```

In **Game.cpp**, modify the TODO of **Render** to replace the ``Draw`` call above with the following:

```cpp
size_t nbones = m_model->bones.size();

m_model->CopyAbsoluteBoneTransforms(nbones,
    m_animBones.get(), m_drawBones.get());

m_model->Draw(context, *m_states, nbones, m_drawBones.get(),
    m_world, m_view, m_proj);
```

You can build and run now, but the result will be a bit disappointing: it's just a blank CornflowerBlue scene. We'll solve this problem next.

# Modifying model bones

The reason the scene is blank is that the model itself is shifted out of view. One of the model bones is translating it away from our center of view. While there are a number of ways to resolve this, since we are using model bones to render we can fix it directly.

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources** (after you have loaded the model and created the bone arrays):

```cpp
uint32_t index = 0;
for (const auto& it : m_model->bones)
{
    if (_wcsicmp(it.name.c_str(), L"tank_geo") == 0)
    {
        // Need to recenter the model.
        m_animBones[index] = XMMatrixIdentity();
    }

    ++index;  
}
```

Now if you build and run, the model is back in the center and actually looks like a tank.

![Screenshot of tank model](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTank2.png)

# Moving individual bones

Now that we have our tank drawing correctly with each mesh controlled by individual model bones, we can add the animation. Since it's a hexarchy, we only need to worry about the local changes (i.e. how far is the turret rotated) without having to worry about the global location (i.e. where is the tank body located).

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
uint32_t m_leftBackWheelBone;
uint32_t m_rightBackWheelBone;
uint32_t m_leftFrontWheelBone;
uint32_t m_rightFrontWheelBone;
uint32_t m_leftSteerBone;
uint32_t m_rightSteerBone;
uint32_t m_turretBone;
uint32_t m_cannonBone;
uint32_t m_hatchBone;
```

In **Game.cpp**, update the constructor:

```cpp
Game::Game() noexcept(false) :
    m_leftBackWheelBone(ModelBone::c_Invalid),
    m_rightBackWheelBone(ModelBone::c_Invalid),
    m_leftFrontWheelBone(ModelBone::c_Invalid),
    m_rightFrontWheelBone(ModelBone::c_Invalid),
    m_leftSteerBone(ModelBone::c_Invalid),
    m_rightSteerBone(ModelBone::c_Invalid),
    m_turretBone(ModelBone::c_Invalid),
    m_cannonBone(ModelBone::c_Invalid),
    m_hatchBone(ModelBone::c_Invalid)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}
```

Since we already have a loop searching through the bones, we'll just update it to find all our needed bones. Modify in **Game.cpp** in **CreateDeviceDependentResources**:

```cpp
uint32_t index = 0;
for (const auto& it : m_model->bones)
{
    if (_wcsicmp(it.name.c_str(), L"tank_geo") == 0)
    {
        // Need to recenter the model.
        m_animBones[index] = XMMatrixIdentity();
    }
    else if (_wcsicmp(it.name.c_str(), L"l_back_wheel_geo") == 0) { m_leftBackWheelBone = index; }
    else if (_wcsicmp(it.name.c_str(), L"r_back_wheel_geo") == 0) { m_rightBackWheelBone = index; }
    else if (_wcsicmp(it.name.c_str(), L"l_front_wheel_geo") == 0) { m_leftFrontWheelBone = index; }
    else if (_wcsicmp(it.name.c_str(), L"r_front_wheel_geo") == 0) { m_rightFrontWheelBone = index; }
    else if (_wcsicmp(it.name.c_str(), L"l_steer_geo") == 0) { m_leftSteerBone = index; }
    else if (_wcsicmp(it.name.c_str(), L"r_steer_geo") == 0) { m_rightSteerBone = index; }
    else if (_wcsicmp(it.name.c_str(), L"turret_geo") == 0) { m_turretBone = index; }
    else if (_wcsicmp(it.name.c_str(), L"canon_geo") == 0) { m_cannonBone = index; }
    else if (_wcsicmp(it.name.c_str(), L"hatch_geo") == 0) { m_hatchBone = index; }

    ++index;
}
```

In **Game.cpp**, add to the TODO of **Update** (after setting the ``m_world`` variable):

```cpp
float wheelRotation = time * 5.f;
float steerRotation = sinf(time * 0.75f) * 0.5f;
float turretRotation = sinf(time * 0.333f) * 1.25f;
float cannonRotation = sinf(time * 0.25f) * 0.333f - 0.333f;
float hatchRotation = std::min(0.f, std::max(sinf(time * 2.f) * 2.f, -1.f));

XMMATRIX mat = XMMatrixRotationX(wheelRotation);
m_animBones[m_leftFrontWheelBone] = XMMatrixMultiply(mat,
    m_model->boneMatrices[m_leftFrontWheelBone]);
m_animBones[m_rightFrontWheelBone] = XMMatrixMultiply(mat,
    m_model->boneMatrices[m_rightFrontWheelBone]);
m_animBones[m_leftBackWheelBone] = XMMatrixMultiply(mat,
    m_model->boneMatrices[m_leftBackWheelBone]);
m_animBones[m_rightBackWheelBone] = XMMatrixMultiply(mat,
    m_model->boneMatrices[m_rightBackWheelBone]);

mat = XMMatrixRotationX(steerRotation);
m_animBones[m_leftSteerBone] = XMMatrixMultiply(mat,
    m_model->boneMatrices[m_leftSteerBone]);
m_animBones[m_rightSteerBone] = XMMatrixMultiply(mat,
    m_model->boneMatrices[m_rightSteerBone]);

mat = XMMatrixRotationY(turretRotation);
m_animBones[m_turretBone] = XMMatrixMultiply(mat,
    m_model->boneMatrices[m_turretBone]);

mat = XMMatrixRotationX(cannonRotation);
m_animBones[m_cannonBone] = XMMatrixMultiply(mat,
    m_model->boneMatrices[m_cannonBone]);

mat = XMMatrixRotationX(hatchRotation);
m_animBones[m_hatchBone] = XMMatrixMultiply(mat,
    m_model->boneMatrices[m_hatchBone]);
```

Now if you build and run, the tank wheels are rolling, the turret is swinging back & forth, and other parts of the model animate as well.

![Screenshot of moving tank model](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotTank3.png)

### Technical notes

* The ``ModelBone::TransformArray`` type is just a ``std::unique_ptr`` owning an array of ``XMMATRIX``. This function ``ModelBone::MakeArray`` function ensures the allocated memory is 16-byte aligned to support aligned SIMD operations.

* The ``m_model->boneMatrices`` is a **ModelBone::TransformArray** in the Model that contains the original local transforms loaded from the file. The length of the array is the same as the number of bones (i.e. ``m_model->bones.size()``).

* The ``CopyBoneTransformsTo`` method is a simple helper for copying the entire ``m_model->boneMatrices`` to a another bones matrices array.

* The ``CopyAbsoluteBoneTransforms`` method computes the final matrices using the bone hierarchy given the input local matrices for each bone.

**Next lesson:** [[Using skinned models]]

# Credits

This tutorial lesson is based heavily on the XNA Game Studio [Simple Animation](https://github.com/SimonDarksideJ/XNAGameStudio/wiki/Simple-Animation) sample, including using the Tank assets (MS-PL).

The ``tank.sdkmesh`` model was exported using ``-flipz-`` to keep the text on the tank texture to appear correct in the right-handed view set up for this lesson.
