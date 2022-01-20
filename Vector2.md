Two component vector class modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Vector2``) math library.

# Header
```cpp
#include <SimpleMath.h>
```

# Initialization

```cpp
using namespace DirectX::SimpleMath;

Vector2 v;        // Creates a vector [0, 0]
Vector2 v(10);    // Creates a vector [10, 10]
Vector2 v(1, 2);  // Creates a vector [1, 2]

float arr[2] = { 1, 2 };
Vector2 v(arr);   // Creates a vector [1, 2]
```

# Fields
* *x* component of vector
* *y* component of vector

# Methods
* Comparison operators: ``==`` and ``!=``
* Assignment operators: ``=``, ``+=``, ``-=``, ``*=``, ``/=``
* Unary operators: ``+``, ``-``
* Binary operators: ``+``, ``-``, ``*``, ``/``

* **InBounds**: Tests if the vector is within the bounds set by the positive and negative of the input bounds.

* **Length**, **LengthSquared**: Compute vector length.

* **Dot**: Returns the dot-product between two 2-vectors.

* **Cross**: Computes the geometric cross-product of two 2-vectors. See [XMVector2Cross](https://docs.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmvector2cross).

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

* **Transform**: Transform the vector by the rotation and scale in the matrix or a quaternion. There is also an overload for transforming an array of vectors. See [XMVector2Transform](https://docs.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmvector2transform).

* **TransformNormal**: Transforms the vector by the rotation and scale in the matrix, ignoring translation (rows 2 & 3). There is also an overload for transforming an array of vectors. See [XMVector2TransformNormal](https://docs.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmvector2transformnormal).

# Constants

* **Zero**: Zero vector [0, 0]
* **One**: One vector [1, 1]
* **UnitX**: Unit X vector [1, 0]
* **UnitY**: Unit Y vector [0, 1]

# Remark
Vector2 can freely convert to and from a ``XMFLOAT2`` and ``XMVECTOR`` (the z and w components are lost).

# See also

[[Vector3]], [[Vector4]], [[Matrix]]
