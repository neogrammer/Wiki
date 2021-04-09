This lesson covers the playing sounds with _DirectX Tool Kit for Audio_ using 3D positional audio effects.

# Setup
First create a new project. For this lesson, use the [[DeviceResources]] variant described in [[Using DeviceResources]], then use the instructions in [[Adding the DirectX Tool Kit]], then [[Adding the DirectX Tool Kit for Audio]], and finally [[Adding audio to your project]] which we will use for this lesson.

# Background
Games provide an immersive visual and audio experience, and often use [3D audio effects](https://en.wikipedia.org/wiki/3D_audio_effect) to enhance the audio. The effect modifies the speaker position, pitch, and volume of a sound to provide audial cues to place it in the 3D environment. **XAudio2** itself provides the ability to specify a complex per-channel volume mapping, pitch modification, and other parameters to generate these 3D effects, but does not perform the computations. The X3DAudio library is used by *DirectX Tool Kit for Audio* to compute the virtual placement of the sound.

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/xaudio2/x3daudio-overview) for more information on **X3DAudio**.

# Applying a 3D positional effect

> **UNDER CONSTRUCTION**

# Further reading
DirectX Tool Kit docs [[AudioListener]], [[AudioEmitter]], [[SoundEffectInstance]]

[XAudio2Sound3D](https://github.com/walbourn/directx-sdk-samples/tree/master/XAudio2/XAudio2Sound3D), [SimplePlay3DSoundUWP](https://github.com/microsoft/Xbox-ATG-Samples/tree/master/UWPSamples/Audio/SimplePlay3DSoundUWP) samples