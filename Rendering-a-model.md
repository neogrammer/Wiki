This lesson loads and draws models in 3D.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Creating a model
Source assets for models are often stored in Autodesk FBX, Wavefront OBJ, or similar formats. A build process is used to convert them to a more run-time friendly format that is easier to load and render.

Visual Studio has a built-in system for converting a Wavefront OBJ or Autodesk FBX as part of the build process to a CMO, which you can read about [here](https://docs.microsoft.com/en-us/visualstudio/designers/using-3-d-assets-in-your-game-or-app).

For this tutorial, we will instead make of use of the [DirectXMesh](http://go.microsoft.com/fwlink/?LinkID=324981) **meshconvert** command-line tool.  Start by saving [cup._obj](https://github.com/Microsoft/DirectXTK/wiki/cup._obj), [cup.mtl](https://github.com/Microsoft/DirectXTK/wiki/cup.mtl), and [cup.jpg](https://github.com/Microsoft/DirectXTK/wiki/images/cup.jpg) into your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "cup.jpg" and click "OK".

1. Download the [Meshconvert.exe](https://github.com/Microsoft/DirectXMesh/releases/download/aug2018/meshconvert.exe) from the _DirectXMesh_ site save the EXE into your project's folder.
1. Open a [command-prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq) and then change to your project's folder.

Run the following command-line

    meshconvert cup._obj -cmo -nodds -flipz -y

Then from the top menu in Visual Studio select **Project** / **Add Existing Item...**. Select [cup.cmo](https://github.com/Microsoft/DirectXTK/wiki/cup.cmo) and click "OK".

## Technical notes
* The switch ``-cmo`` selects the Visual Studio Compiled Mesh Object runtime format as the output. The **meshconvert** command-line tool also supports ``-sdkmesh`` and ``-vbo``. See [Geometry formats](https://directxmesh.codeplex.com/wikipage?title=Geometry%20formats) for more information.
* The switch ``-nods`` causes any texture file name references in the material information of the source file to stay in their original file format (such as ``.png`` or ``.jpg``). Otherwise, it assumes you will be converting all the needed texture files to a ``.dds`` instead.
* The switch ``-flipz`` inverts the z axis. Since [[SimpleMath]] and these tutorials assume we are using _right-handed viewing coordinates_ and the model was created using _left-handed viewing coordinates_ we have to flip them to get the text in the texture to appear correctly.
* The switch `-y` indicates that it is ok to overwrite the output file in case you run it multiple times.

# Drawing a model

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::SimpleMath::Matrix m_world;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;

std::unique_ptr<DirectX::CommonStates> m_states;
std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
std::unique_ptr<DirectX::Model> m_model;
```

> In most cases you'd want to use ``std::unique_ptr<DirectX::EffectFactory> m_fxFactory;`` instead of ``std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;``, but we keep a pointer to the abstract interface in this tutorial instead of to the concrete class to streamline things later on.

In **Game.cpp**, add to the TODO of **CreateDevice**:

```cpp
m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

m_fxFactory = std::make_unique<EffectFactory>(m_d3dDevice.Get());

m_model = Model::CreateFromCMO(m_d3dDevice.Get(), L"cup.cmo", *m_fxFactory);

m_world = Matrix::Identity;
```

In **Game.cpp**, add to the TODO of **CreateResources**:

```cpp
m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
    Vector3::Zero, Vector3::UnitY);
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(backBufferWidth) / float(backBufferHeight), 0.1f, 10.f);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_states.reset();
m_fxFactory.reset();
m_model.reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
m_model->Draw(m_d3dContext.Get(), *m_states, m_world, m_view, m_proj);
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
float time = float(timer.GetTotalSeconds());

m_world = Matrix::CreateRotationZ(cosf(time) * 2.f);
```

Build and run and you will see our cup model rendered with default lighting:

![Screenshot of cup model](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotCup.PNG)

> _Troubleshooting:_ If you get a runtime exception, then you may have the "cup.jpg" or "cup.cmo" in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``Model::CreateFromCMO`` and step into the code to find the exact problem.

# Updating effects settings in a model

The Model class creates effects automatically for the loaded materials which are set to default lighting parameters. To update them, you use the **Model::UpdateEffects** method. Because the effects system is flexible, we must first enable [C++ Run-Time Type Information](https://docs.microsoft.com/en-us/cpp/cpp/run-time-type-information) (RTTI) in order to safely discover the various interfaces supported at runtime. From the drop-down menu, select **Project** / **Properties**. Set to "All Configurations" / "All Platforms". On the left-hand tree view select **``C/C++``** / **Language**. Then set "Enable Run-Time Type Information" to "Yes". Click "OK".

![Screenshot C++ RTTI settings](https://github.com/Microsoft/DirectXTK/wiki/images/settingsRTTI.PNG)

In **Game.cpp**, add to the TODO of **CreateDevice**:

```cpp
m_model->UpdateEffects([](IEffect* effect)
{
    auto lights = dynamic_cast<IEffectLights*>(effect);
    if (lights)
    {
        lights->SetLightingEnabled(true);
        lights->SetPerPixelLighting(true);
        lights->SetLightEnabled(0, true);
        lights->SetLightDiffuseColor(0, Colors::Gold);
        lights->SetLightEnabled(1, false);
        lights->SetLightEnabled(2, false);
    }

    auto fog = dynamic_cast<IEffectFog*>(effect);
    if (fog)
    {
        fog->SetFogEnabled(true);
        fog->SetFogColor(Colors::CornflowerBlue);
        fog->SetFogStart(3.f);
        fog->SetFogEnd(4.f);
    }
});
```

Build and run to get our cup with a colored light, per-pixel rather than vertex lighting, and fogging enabled.

![Screenshot of fogged cup model](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotCupFog.PNG)

## Technical notes
Here we've made use to two C++ concepts:
* The ``dynamic_cast`` operator allows us to safely determine if an effect in the model supports [[lighting|IEffectLights]] and/or [[fog|IEffectFog]] using the C++ run-time type checking. If the [[IEffect]] instance does not support the desired interface, the cast returns a nullptr and our code will skip those settings.
* This also used a _lambda function_ (aka anonymous function) of the form ``[](IEffect* effect){ ... }`` which is a [new C++11 syntax](http://en.cppreference.com/w/cpp/language/lambda). We could have used a C-style call-back function, but this syntax is more direct.

# Using DGSL shaders for the model

For this lesson, we made use of [[EffectFactory]] which will create [[BasicEffect]], [[DualTextureEffect]] or [[SkinnedEffect]] instances depending on the content of the loaded model file. With CMOs and the built-in Visual Studio 3D pipeline, you can instead make use of [[DGSLEffect]] by changing ``EffectFactory`` to ``DGSLEffectFactory``.

In **Game.cpp** in the TODO section of **CreateDevice**, change

```cpp
m_fxFactory = std::make_unique<EffectFactory>(m_d3dDevice.Get());
```

to

```cpp
m_fxFactory = std::make_unique<DGSLEffectFactory>(m_d3dDevice.Get());
```

> This is why we used the abstract interface ``IEffectFactory`` rather than using ``std::unique_ptr<EffectFactory>`` in **Game.h** so that the variable could refer to either type of factory.

Build and run. If you still have the **UpdateEffects** code in place, you'll see the fog no longer appears although the light is colored since ``DGSLEffect`` does not support the fog interface.

![Screenshot of DSGL cup model](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotCupNoFog.PNG)

The lack of fog is because our "cup.cmo" makes use of the default built-in DGSL shaders _lambert_ and _phong_ which do not implement fog. The ``DGSLEffectFactory`` allows you to use ``Model`` and the [[IEffect]] interface with the more complex custom DGSL shaders supported by the Visual Studio CMO pipeline where you use a [visual editor to build pixel shaders](https://docs.microsoft.com/en-us/visualstudio/designers/working-with-shaders).

**Next lesson:** [[Using skinned models]]

# Further reading

DirectX Tool Kit docs [[EffectFactory]], [[Effects]], [[Model]]
