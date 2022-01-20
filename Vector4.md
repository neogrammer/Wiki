Four component vector class modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Vector4``) math library.

See also [[SimpleMath]]

# Header
```cpp
#include <SimpleMath.h>
```

# Initialization

```cpp
using namespace DirectX::SimpleMath;

Vector4 v;             // Creates a vector [0, 0, 0, 0]
Vector4 v(10);         // Creates a vector [10, 10, 10, 10]
Vector4 v(1, 2, 3, 4); // Creates a vector [1, 2, 3, 4]

float arr[4] = { 1, 2, 3, 4 };
Vector4 v(arr);        // Creates a vector [1, 2, 3, 4]
```

# Fields
* *x* component of vector
* *y* component of vector
* *z* component of vector
* *w* component of vector

# Methods
* Comparison operators: ``==`` and ``!=``
* Assignment operators: ``=``, ``+=``, ``-=``, ``*=``, ``/=``
* Unary operators: ``+``, ``-``
* Binary operators: ``+``, ``-``, ``*``, ``/``

* **InBounds**: Tests if the vector is within the bounds set by the positive and negative of the input bounds.

* **Length**, **LengthSquared**: Compute vector length.

* **Dot**: Returns the dot-product of two 4-vectors.

* **Cross**: Computes the geometric dot-product of 4-vectors. See [XMVector4Cross](https://docs.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmvector4cross).

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

* **Transform**: Transforms the vector by the matrix or a quaternion. There is also an overload for transforming an array of vectors.

# Constants

* **Zero**: Zero vector [0, 0, 0, 0]
* **One**: One vector [1, 1, 1, 1]
* **UnitX**: Unit X vector [1, 0, 0, 0]
* **UnitY**: Unit Y vector [0, 1, 0, 0]
* **UnitZ**: Unit Z vector [0, 0, 1, 0]
* **UnitW**: Unit W vector [0, 0, 0, 1]

# Remark
Vector4 can freely convert to and from a ``XMFLOAT4`` and ``XMVECTOR``

When assigning an RGBA color from ``DirectXColors.h`` to a Vector4, use the ``.v`` union element

```cpp
Vector4 color = Colors::White.v;
```

# See also

[[Vector2]], [[Vector3]], [[Matrix]], [[Quaternion]]
