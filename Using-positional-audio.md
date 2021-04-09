This lesson covers the playing sounds with _DirectX Tool Kit for Audio_ using 3D positional audio effects.

# Setup
First create a new project. For this lesson, use the [[DeviceResources]] variant described in [[Using DeviceResources]], then use the instructions in [[Adding the DirectX Tool Kit]], then [[Adding the DirectX Tool Kit for Audio]], and finally [[Adding audio to your project]] which we will use for this lesson.

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
m_soundEffect = std::make_unique<SoundEffect>( m_audEngine.get(), L"heli.wav" );
```

Build and run. No sounds will be heard, but the audio file is loaded.

> _Troubleshooting:_ If you get a runtime exception, then you may have the ``.wav`` file in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``std::make_unique<SoundEffect>`` and step into the code to find the exact problem.

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
        if (  m_soundSource )
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
}
```

Build and run to hear the helicopter sound, which at the moment will be a bit too lound.

# Adding a simple scene

While not strictly required, the 3D audio effect illusion makes more sense with some visuals to go with it, so here we'll add a simple sphere rotation around the user. Later we will "attach" the sound to it's location.

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

```cpp
std::unique_ptr<DirectX::GeometricPrimitive> m_ball;
DirectX::SimpleMath::Matrix m_proj;
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Vector3 m_position;
```

At the top of **Game.cpp** after the ``using`` statements, add:

```cpp
namespace
{
   const XMVECTORF32 CAMERA_POSITION = { 0.f, 0.f, -10.f, 0.f };
}
```

In **Game.cpp**, add to the TODO of **CreateDeviceDependentResources**:

```cpp
auto context = m_deviceResources->GetD3DDeviceContext();
m_ball = GeometricPrimitive::CreateSphere(context);
```

In **Game.cpp**, add to the TODO of **CreateWindowSizeDependentResources**:

```cpp
auto size = m_deviceResources->GetOutputSize();
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(size.right) / float(size.bottom), 0.1f, 10.f);

m_view = Matrix::CreateLookAt(CAMERA_POSITION.v,
    Vector3::Zero, Vector3::UnitY);
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

> If using DirectX 12, you will also need a BasicEffect and CommonStates to render this scene. See the [3D Shapes](https://github.com/microsoft/DirectXTK12/wiki/3D-shapes) tutorial for more details.

# Applying a 3D positional effect

> **UNDER CONSTRUCTION**

# Further reading
DirectX Tool Kit docs [[AudioListener]], [[AudioEmitter]], [[SoundEffectInstance]]

[XAudio2Sound3D](https://github.com/walbourn/directx-sdk-samples/tree/master/XAudio2/XAudio2Sound3D), [SimplePlay3DSoundUWP](https://github.com/microsoft/Xbox-ATG-Samples/tree/master/UWPSamples/Audio/SimplePlay3DSoundUWP) samples