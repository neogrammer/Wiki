Three component vector class modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Vector3``) math library.

# Header

    #include <SimpleMath.h>

# Initialization

    using namespace DirectX::SimpleMath;

    Vector3 v;          // Creates a vector [0, 0, 0]
    Vector3 v(10);      // Creates a vector [10, 10, 10]
    Vector3 v(1, 2, 3); // Creates a vector [1, 2, 3]

    float arr[3] = { 1, 2, 3 };
    Vector3 v(arr);     // Creates a vector [1, 2, 3]

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

* **Length**, **LengthSquared**: Compute vector length
* **Dot**
* **Cross**
* **Normalize**
* **Clamp**

# Statics
* **Distance**, **DistanceSquared**: Compute distance between two vectors

* **Min**, **Max**: Determine minimum/maximum vector elements

* **Lerp**: Performs a linear interpolation
* **SmoothStep**: Performs a cube interpolation
* **Barycentric**: Returns a point in Barycentric coordinates
* **CatmullRom**: Performs a Catmull-Rom interpolation
* **Hermite**: Performs a Hermite spline interpolation

* **Reflect**: Reflects an incident vector across a normal vector
* **Refract**: Refracts an incident vector across a normal vector

* **Transform**
* **TransformNormal**: Transforms the vector by the rotation and scale in the matrix, ignoring translation (row 3).

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

When assigning a color from ``DirectXColors.h`` to a Vector3, use the ``.v`` union element

    Vector3 color = Colors::White.v;

