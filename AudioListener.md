AudioListener is a helper object derived from [X3DAUDIO_LISTENER](https://docs.microsoft.com/en-us/windows/desktop/api/x3daudio/ns-x3daudio-x3daudio_listener) used by [[SoundEffectInstance]]'s / [[DynamicSoundEffectInstance]] **Apply3D** method.

See [[AudioEmitter]].

# Header
```cpp
#include <Audio.h>
```

# Initialization

The default constructor creates a listener located at 0,0,0 facing ``-z`` and an up vector of ``+y``.

> When using left-handed coordinates, be sure to set ``OrientFront``

# Helpers

In addition to setting the members of ``X3DAUDIO_LISTENER`` directly, these helper functions are provided:

* **SetPosition** (XMVECTOR) or (XMFLOAT3): Sets the Position of the listener.

* **SetVelocity** (XMVECTOR) or (XMFLOAT3): Sets the Velocity of the listener.

* **SetOrientation** (XMVECTOR forward, XMVECTOR up) or (XMFLOAT3 forward, XMFLOAT3 up)
* **SetOrientationFromQuaternion** (XMVECTOR): Sets the OrientFront/OrientTop of the listener.

* **Update** (XMVECTOR newPos, XMVECTOR upDir, float dt ): Computes a direction and velocity for the listener based on the existing Position and the newPos, updating the OrientFront/OrientTop to match, and then setting the Position to the newPos. If dt is 0, the update is skipped.

# Further reading
[X3DAudio](https://docs.microsoft.com/en-us/windows/desktop/xaudio2/x3daudio)  
[X3DAudioCalculate](https://docs.microsoft.com/en-us/windows/desktop/api/x3daudio/nf-x3daudio-x3daudiocalculate)
