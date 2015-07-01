AudioListener is a helper object derived from [X3DAUDIO_LISTENER](http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.x3daudio.x3daudio_listener.aspx) used by [[SoundEffectInstance]]'s / [[DynamicSoundEffectInstance]] **Apply3D** method.

See [[AudioEmitter]].

# Header
    #include <Audio.h>

# Initialization

The default constructor creates a listener located at 0,0,0 facing ``+z`` and an up vector of ``+y``.

# Helpers

In addition to setting the members of ``X3DAUDIO_LISTENER`` directly, these helper functions are provided:

* **SetPosition** (FXVMECTOR) or (XMFLOAT3): Sets the Position of the listener.

* **SetVelocity** (FXMVECTOR) or (XMFLOAT3): Sets the Velocity of the listener.

* **SetOrientation** (FXMVECTOR forward, FXVMECTOR up) or (XMFLOAT3 forward, XMFLOAT3 up)
* **SetOrientationFromQuaternion** ( FXMVECTOR ): Sets the OrientFront/OrientTop of the listener.

* **Update** ( FXMVECTOR newPos, XMVECTOR upDir, float dt ): Computes a direction and velocity for the listener based on the existing Position and the newPos, updating the OrientFront/OrientTop to match, and then setting the Position to the newPos. If dt is 0, the update is skipped.

# Further reading
[X3DAudio](http://msdn.microsoft.com/en-us/library/windows/desktop/ee415714.aspx)  
[X3DAudioCalculate](http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.x3daudio.x3daudiocalculate.aspx)
