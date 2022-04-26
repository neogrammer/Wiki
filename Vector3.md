|[[DirectXTK]]|[[SimpleMath]]|
|---|---|

Three component vector class modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Vector3``) math library.

# Header
```cpp
#include <SimpleMath.h>
```

# Initialization

```cpp
using namespace DirectX::SimpleMath;

Vector3 v;          // Creates a vector [0, 0, 0]
Vector3 v(10);      // Creates a vector [10, 10, 10]
Vector3 v(1, 2, 3); // Creates a vector [1, 2, 3]

float arr[3] = { 1, 2, 3 };
Vector3 v(arr);     // Creates a vector [1, 2, 3]
```

# Fields
* *x* component of vector
* *y* component of vector
* *z* component of vector

# Methods
* Comparison operators: ``==`` and ``!=``
* Assignment operators: ``=``, ``+=``, ``-=``, ``*=``, ``/=``
* Unary operators: ``+``, ``-``
* Binary operators: ``+``, ``-``, ``*``, ``/``

* **InBounds**: Tests if the vector is within the bounds set by the positive and negative of the input bounds.

* **Length**, **LengthSquared**: Compute vector length.

* **Dot**: Returns the dot-product of two 3-vectors.

* **Cross**: Computes the cross product of two 3-vectors.

* **Normalize**: Computes a unit-length vector.

* **Clamp**: Clamps the values of the vector per component between a minimum and a maximum.

# Statics
* **Distance**, **DistanceSquared**: Compute distance between two vectors.

* **Min**, **Max**: Determine minimum/maximum vector elements.

* **Lerp**: Performs a linear interpolation.
* **SmoothStep**: Performs a cube interpolation.
* **Barycentric**: Returns a point in Barycentric coordinates.
* **CatmullRom**: Performs a Catmull-Rom interpolation.
* **Hermite**: Performs a Hermite spline interpolation.

* **Reflect**: Reflects an incident vector across a normal vector.
* **Refract**: Refracts an incident vector across a normal vector.

* **Transform**: Transform the vector by a matrix or a quaternion. See [XMVector3Transform](https://docs.microsoft.com/windows/win32/api/directxmath/nf-directxmath-xmvector3transform). There are overloads for returning the homogenous normalized ``Vector3`` result (i.e. ``w = 1``) or the ``Vector4`` result. There is also an overload for transforming an array of vectors.

* **TransformNormal**: Transforms the vector by matrix, ignoring translation (row 3). There is also an overload for transforming an array of vectors. See [XMVector3TransformNormal](https://docs.microsoft.com/windows/win32/api/directxmath/nf-directxmath-xmvector3transformnormal).

# Constants

* **Zero**: Zero vector [0, 0, 0]
* **One**: One vector [1, 1, 1]
* **UnitX**: Unit X vector [1, 0, 0]
* **UnitY**: Unit Y vector [0, 1, 0]
* **UnitZ**: Unit Z vector [0, 0, 1]
* **Up**: Up direction vector [0, 1, 0]
* **Down**: Down direction vector [0, -1, 0]
* **Right**: Right direction vector [1, 0, 0]
* **Left**: Left direction vector [-1, 0, 0]
* **Forward**: Forward direction vector for right-handed (RH) coordinates [0, 0, -1]
* **Backward**: Backward direction vector for right-handed (RH) coordinates [0, 0, 1]

# Remark
Vector3 can freely convert to and from a ``XMFLOAT3`` and ``XMVECTOR`` (the w component is lost).

When assigning an RGB color from ``DirectXColors.h`` to a Vector3, use the ``.v`` union element

```cpp
Vector3 color = Colors::White.v;
```
# See also

[[Vector2]], [[Vector4]], [[Matrix]], [[Quaternion]]
