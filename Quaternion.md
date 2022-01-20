A rotation represented as a four component vector modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Quaternion``) math library.

A *quaternion* is a very efficient and compact method for working with 3D rotation. A *quaternion* is a 4-dimensional value and only has physical meaning when it's normalized. In computer graphics, they are used to represent 3D rotations as a 4-vector (i.e. 4 float values) instead of requiring a 3x3 matrix (i.e. 9 float values). They are extremely useful implementing cameras and animation where a *quaternion* can smoothly interpolate between 3D rotations while avoiding the [gimbal lock](https://en.wikipedia.org/wiki/Gimbal_lock) problem common to Euler angles.

# Header
```cpp
#include <SimpleMath.h>
```

# Initialization

```cpp
using namespace DirectX::SimpleMath;

Quaternion q;                     // Creates the identity quaternion [0, 0, 0, 1]
Quaternion q(0, 0, 0, 1);         // Creates a quaternion [0, 0, 0, 1]
Quaternion q( Vector3(0,0,0), 1); // Creates a quaternion [0, 0, 0, 1]
Quaternion q( Vector4(0,0,0,1) ); // Creates a quaternion [0, 0, 0, 1]

float arr[4] = { 0, 0, 0, 1 };
Quaternion q(arr);                // Creates a quaternion [0, 0, 0, 1]
```

# Fields
* *x* vector component of the quaternion
* *y* vector component of the quaternion
* *z* vector component of the quaternion
* *w* scalar component of the quaternion

# Methods
* Comparison operators: ``==`` and ``!=``

* Assignment operators: ``=``, ``+=``, ``-=``, ``*=``, ``/=``

* Unary operators: ``+``, ``-``

* Binary operators: ``+``, ``-``, ``*``, ``/``

* **Length**

* **LengthSquared**

* **Normalize**: Normalizes the quaternion. _Note that only normalized quaternions correspond to 3D rotations._

* **Conjugate**: Computes the conjugate of a quaternion. _This result is ``Quaternion(-x, -y, -z, w)``._

* **Inverse**

* **Dot**

* **ToEuler**: Computes rotation about y-axis (y), then x-axis (x), then z-axis (z). The return value is compatible with one of the overloads of ``CreateFromYawPitchRoll``.

# Statics

* **CreateFromAxisAngle**: Creates a quaternion representing a rotation of a given angle (in radians) around an arbitrary axis vector.

* **CreateFromYawPitchRoll**: Creates a quaternion representation a rotation from a yaw/pitch/roll.

> The original D3DXmath library took the rotations in the the Yaw, Pitch, Roll order and that order was replicated in *XNA Game Studio*. In DirectXMath, the order was normalized to Roll (X), Pitch (Y), Yaw (Z).

* **CreateFromRotationMatrix**

* **Concatenate**: Concatenates two quaternion rotations. _Note: ``Concatenate(q1,q2)`` is equivalent to ``q2*q1``_.

* **Lerp**: Linear interpolation

* **Slerp**: Spherical linear interpolation

> For interpolating between arbitrary 3D rotations, the *slerp* is the gold-standard. For small differences, however, *lerp* is much faster and almost identical.

# Constants
* **Identity**: The identity quaternion [0, 0, 0, 1]

# Remark
Quaternion can freely convert to and from a ``XMFLOAT4`` and ``XMVECTOR``

# Further Reading
[Quaternions and spatial rotation](https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation)  

Jonathan Blow, "Understanding Slerp, Then Not Using It", The Inner Product, April 2004 [link](http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/)

David Eberly, "Quaternion Algebra and Calculus" [link](http://www.geometrictools.com/Documentation/Quaternions.pdf)

Ken Shoemake, "Quaternions", Department of Computer and Information Science, University of Pennsylvania [link](http://www.cs.ucr.edu/~vbz/resources/quatut.pdf)
