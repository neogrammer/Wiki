|[[Getting Started]] for DX11|[Getting Started](https://github.com/microsoft/DirectXTK12/wiki/Getting-Started) for DX12|
|---|---|

This lesson covers the playing sounds with _DirectX Tool Kit for Audio_ using 3D positional audio effects.

# Setup
First create a new project using the instructions from the earlier lessons: [[Using DeviceResources]], then [[Adding the DirectX Tool Kit]], [[Adding the DirectX Tool Kit for Audio]], and finally [[Adding audio to your project]] which we will use for this lesson.

> If using *DirectX Tool Kit for DX12*, use [Using DeviceResources](https://github.com/microsoft/DirectXTK12/wiki/Using-DeviceResources), then [Adding the DirectX Tool Kit](https://github.com/microsoft/DirectXTK12/wiki/Adding-the-DirectX-Tool-Kit), add ``#include <Audio.h>`` to **pch.h**, and finally [[Adding audio to your project]]

# Background
Games provide an immersive visual and audio experience, and often use [3D audio effects](https://en.wikipedia.org/wiki/3D_audio_effect) to enhance the audio. The effect modifies the speaker position, pitch, and volume of a sound to provide audial cues to place it in the 3D environment. **XAudio2** itself provides the ability to specify a complex per-channel volume mapping, pitch modification, and other parameters to generate these 3D effects, but does not perform the computations. The X3DAudio library is used by *DirectX Tool Kit for Audio* to compute the virtual placement of the sound.

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/xaudio2/x3daudio-overview) for more information on **X3DAudio**.

# Sound

Save the this file to your new project's folder: [heli.wav](https://github.com/Microsoft/DirectXTK/wiki/media/heli.wav).  Use the top menu and select **Project / Add Existing Item**.... Select the ``heli.wav`` file and hit "OK".

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::SoundEffect> m_soundEffect;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"heli.wav");
```

Build and run. No sounds will be heard, but the audio file is loaded.

<details><summary><i>Click here for troubleshooting advice</i></summary>
<p>If you get a runtime exception, then you may have the <code>.wav</code> file in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on <code>std::make_unique&lt;SoundEffect&gt;</code> and step into the code to find the exact problem.</p></details>


# Play the sound

As we did in the [[Creating and playing sounds]] tutorial, we create an instance of our sound and start it looping. To ensure it reacts properly to "lost device" scenarios, we update it in a few places.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::SoundEffectInstance> m_soundSource;
```

In **Game.cpp**, add to the end of **Initialize**:

```cpp
m_soundSource = m_soundEffect->CreateInstance();
m_soundSource->Play(true);
```

In **Game.cpp**, modify the handling of ``m_retryAudio`` in **Update** as follows:

```cpp
...
if (m_retryAudio)
{
    m_retryAudio = false;

    if (m_audEngine->Reset())
    {
        // TODO: restart any looped sounds here
        if (m_soundSource)
             m_soundSource->Play(true);
    }
}
...
```

In **Game.cpp**, modify the destructor:

```cpp
Game::~Game()
{
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }

    m_soundSource.reset();
    ...
}
```

Build and run to hear the helicopter sound, which at the moment will be a bit too lound.

# Adding a simple scene

While not strictly required, the 3D audio effect illusion makes more sense with some visuals to go with it, so here we'll add a simple sphere.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::GeometricPrimitive> m_ball;

DirectX::SimpleMath::Matrix m_proj;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Vector3 m_position;
```

In **Game.cpp** file, modify the **Game** constructor to initialize our position variable:

```cpp
Game::Game() noexcept(false) :
    m_position(0, 0, -10.f)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(size.right) / float(size.bottom), 0.1f, 20.f);

m_view = Matrix::CreateLookAt(Vector3::Zero,
    Vector3::Forward, Vector3::Up);
```

Continue below depending on which version of _DirectX Tool Kit_ you are using.

## DirectX 11

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
auto context = m_deviceResources->GetD3DDeviceContext();
m_ball = GeometricPrimitive::CreateSphere(context);
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_ball.reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
XMMATRIX world = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
m_ball->Draw(world, m_view, m_proj, Colors::Yellow);
```

## DirectX 12

> This assumes you've already added *DirectX Tool Kit for 12* support including ``GraphicsMemory`` to your project.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::BasicEffect> m_ballEffect;
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
m_ball = GeometricPrimitive::CreateSphere();

RenderTargetState rtState(m_deviceResources->GetBackBufferFormat(),
    m_deviceResources->GetDepthBufferFormat());

{
    EffectPipelineStateDescription pd(
        &GeometricPrimitive::VertexType::InputLayout,
        CommonStates::Opaque,
        CommonStates::DepthDefault,
        CommonStates::CullNone,
        rtState);

    m_ballEffect = std::make_unique<BasicEffect>(device,
        EffectFlags::Lighting, pd);
    m_ballEffect->EnableDefaultLighting();
}
```

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

```cpp
m_ball.reset();
m_ballEffect.reset();
```

In **Game.cpp**, add to the TODO of **Render**:

```cpp
XMMATRIX world = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
m_ballEffect->SetMatrices(world, m_view, m_proj);
m_ballEffect->SetDiffuseColor(Colors::Yellow);
m_ballEffect->Apply(commandList);
m_ball->Draw(commandList);
```

Build and run, and you should get the following screen:

![Screenshot of yellow ball](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotYellowBall.PNG)

# Applying a 3D positional effect

Now we have our sound and a visual cue where to expect the sound from, we now add the 3D audio effect processing.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
DirectX::AudioListener m_listener;
DirectX::AudioEmitter  m_emitter;
```

Modify **Initialize** in **Game.cpp** so we enable 3D positioning for our instance. The [[AudioListener]] defaults the same position and view direction as our camera, so we don't have to add any explicit initialization.

```cpp
m_soundSource = m_soundEffect->CreateInstance(SoundEffectInstance_Use3D);
m_soundSource->Play(true);
```

In **Game.cpp**, add to the TODO of **Update**:

```cpp
m_emitter.Update(m_position, Vector3::Up, static_cast<float>(timer.GetElapsedSeconds()));
if (m_soundSource)
{
    m_soundSource->Apply3D(m_listener, m_emitter);
}
```

Build and run. The change won't be extreme, but the helicopter sounds will have a slightly lower volume.

Now, the final part of this is to add movement to the sound source. In **Game.cpp**, add to the TODO of **Update** before the call to ``AudioEmitter::Update``:

```cpp
// Move the object around in a circle.
double speed = timer.GetTotalSeconds() / 2;
m_position = Vector3(
    static_cast<float>(cos(speed)) * 5.f,
    0,
    static_cast<float>(sin(speed))) * -5.f;
```

Build and run, and the sound will move from left-to-right in front, then right-to-left behind. You'll only see the sphere while in front.

## Technical note

We do the audio engine processing and 3D audio computations in **Update** for simplicity, but if [[StepTimer]] were set for for fixed-update timesteps instead of variable-rate, then you may be getting more than one call to **Update** per render frame. In that case, it is better to move the "audio rendering" up into **Tick** so it only happens once per render frame.

# Environmental effects

Now that we have the basics of the effect running, it's time to turn on additional features including an environmental reverb effect.

At the top of **Game.cpp** after the ``using`` statements, add:

```cpp
namespace
{
    constexpr X3DAUDIO_CONE c_listenerCone = {
        X3DAUDIO_PI * 5.0f / 6.0f, X3DAUDIO_PI * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f
    };
    constexpr X3DAUDIO_CONE c_emitterCone = {
        0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f
    };

    constexpr X3DAUDIO_DISTANCE_CURVE_POINT c_emitter_LFE_CurvePoints[3] = {
        { 0.0f, 1.0f }, { 0.25f, 0.0f}, { 1.0f, 0.0f }
    };
    constexpr X3DAUDIO_DISTANCE_CURVE c_emitter_LFE_Curve = {
        const_cast<X3DAUDIO_DISTANCE_CURVE_POINT*>(&c_emitter_LFE_CurvePoints[0]), 3
    };

    constexpr X3DAUDIO_DISTANCE_CURVE_POINT c_emitter_Reverb_CurvePoints[3] = {
        { 0.0f, 0.5f}, { 0.75f, 1.0f }, { 1.0f, 0.0f }
    };
    constexpr X3DAUDIO_DISTANCE_CURVE c_emitter_Reverb_Curve = {
        const_cast<X3DAUDIO_DISTANCE_CURVE_POINT*>(&c_emitter_Reverb_CurvePoints[0]), 3
    };
}
```

Modify **Initialize** in **Game.cpp** both the audio engine creation:

```cpp
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_EnvironmentalReverb | AudioEngine_ReverbUseFilters;
#ifdef _DEBUG
    eflags |= AudioEngine_Debug;
#endif
    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audEngine->SetReverb(Reverb_Hangar);
```

and the sound effect instance creation:

```cpp
m_soundSource = m_soundEffect->CreateInstance(SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);
m_soundSource->Play(true);

m_listener.pCone = const_cast<X3DAUDIO_CONE*>(&c_listenerCone);

m_emitter.pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_LFE_Curve);
m_emitter.pReverbCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&c_emitter_Reverb_Curve);
m_emitter.CurveDistanceScaler = 14.f;
m_emitter.pCone = const_cast<X3DAUDIO_CONE*>(&c_emitterCone);
```

Build and run, and now you'll hear some reverb effects as the helicopter sound passes you by.

Try setting the reverb to different presets and see how it impacts the sound. This includes the full **Interactive 3D Audio Rendering Guidelines Level 2.0** set.

> You may want to add ``AudioEngine_UseMasteringLimiter`` to your audio engine flags to avoid potential problems with sounds clipping if they compute to an unusually large volume level.

## Technical note

The *DirectX Tool Kit for Audio* implementation uses a submix-voice to apply the reverb, and directs only the 3D audio sources to this voice. One-shot voices and other instances are therefore not impacted by the reverb settings.

# More to explore

* The [[AudioEmitter]] defaults assume your source sound is mono/single-channel. If you are using a multi-channel sound (stereo, etc.) then you need to set ``ChannelCount`` (i.e. something like ``m_emitter.ChannelCount = m_effect->GetChannelCount();``). Also note that ``m_emitter.pCone`` is ignored for multi-channel emitters. See the *AudioEmitter* ``EnableDefaultMultiChannel`` helper method as well.

* In addition to [[SoundEffectInstance]], you can also use the ``Apply3D`` method on [[SoundStreamInstance]] and [[DynamicSoundEffectInstance]].

# Further reading
DirectX Tool Kit docs [[AudioListener]], [[AudioEmitter]], [[SoundEffectInstance]]

[XAudio2Sound3D](https://github.com/walbourn/directx-sdk-samples/tree/main/XAudio2/XAudio2Sound3D), [SimplePlay3DSound (UWP)](https://github.com/microsoft/Xbox-ATG-Samples/tree/master/UWPSamples/Audio/SimplePlay3DSoundUWP), [SimplePlay3DSound (GDK)](https://github.com/microsoft/Xbox-GDK-Samples/tree/main/Samples/Audio/SimplePlaySoundStream) samples
