AudioEmitter is a helper object derived from [X3DAUDIO_EMITTER](https://docs.microsoft.com/en-us/windows/desktop/api/x3daudio/ns-x3daudio-x3daudio_emitter) used by [[SoundEffectInstance]]'s / [[DynamicSoundEffectInstance]]'s **Apply3D** method.

See [[AudioListener]].

**Related tutorial:** [[Using positional audio]]

# Header
```cpp
#include <Audio.h>
```

# Initialization

The default constructor creates a point omnidirectional emitter located at 0,0,0  facing ``-z`` and an up vector of ``+y`` for a mono source.

*CurveDistanceScaler* and *DopplerScaler* are set to 1 by default. *InnerRadius* is set to 0, and *InnerRadiusAngle* is set to ``X3DAUDIO_PI/4.0`` (45 degrees).

> When using left-handed coordinates, be sure to set ``OrientFront``

# Helpers
In addition to setting the members of ``X3DAUDIO_EMITTER`` directly, these helper functions are provided:

* **SetPosition** (XMVECTOR) or (XMFLOAT3): Sets the Position of the emitter.

* **SetVelocity** (XMVECTOR) or (XMFLOAT3): Sets the Velocity of the emitter.

* **SetOrientation** (XMVECTOR forward, XMVECTOR up) or (XMFLOAT3 forward, XMFLOAT3 up)
* **SetOrientationFromQuaternion** (XMVECTOR): Sets the OrientFront/OrientTop of the emitter.

* **Update** (XMVECTOR newPos, XMVECTOR upDir, float dt): Computes a direction and velocity for the emitter based on the existing Position and the newPos, updating the OrientFront/OrientTop to match, and then setting the Position to the newPos. If dt is 0, the update is skipped.

# Multi-channel 3D Audio
X3DAudio does support multi-channel sound sources for 3D audio (i.e. stereo, quad, etc.). The default constructor for AudioEmitter sets the source up for mono (i.e. single-channel), so to use multi-channel sources, you should set the **ChannelCount** member to match the number of channels in your source, and adjust **ChannelRadius** and the **EmitterAzimuths** array as desired.

_Note: AudioEmitter includes a EmitterAzimuths array which is pointed to by pChannelAzimuths and defaults to all 0. This is because pChannelAzimuths cannot be a nullptr for multi-channel sound emitters._

# Directional Emitters
AudioEmitter defaults to an omnidirectional emitter. To create a sound-cone, set the **pCone** member to point to a ``X3DAUDIO_CONE`` structure. The pointer must point to valid memory at the time Apply3D is called.

# Custom distance curves
The AudioEmitter constructor sets pVolumeCurve, pLFECurve, pLPFDirectCurve, pLPFReverbCurve, and pReverbCurve to nullptr which uses XAudio2 default curves.  Any custom curve definitions must have a pointer to valid memory at the time Apply3D is called.

# Further reading
[X3DAudio](https://docs.microsoft.com/en-us/windows/desktop/xaudio2/x3daudio)  
[X3DAudioCalculate](https://docs.microsoft.com/en-us/windows/desktop/api/x3daudio/nf-x3daudio-x3daudiocalculate)
