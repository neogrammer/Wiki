Three component vector class modeled after the [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.vector3.aspx) math library.

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
* **InBounds**
* **Length**
* **LengthSquared**
* **Dot**
* **Cross**
* **Normalize**
* **Clamp**

# Statics
* **Distance**
* **DistanceSquared**
* **Min**
* **Max**
* **Lerp**
* **SmoothStep**
* **Barycentric**
* **CatmullRom**
* **Hermite**
* **Reflect**
* **Refract**
* **Transform**
* **TransformNormal**

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
Vector3 can freely convert to and from a ``XMFLOAT3`` and ``XMVECTOR``

When assigning a color from ``DirectXColors.h`` to a Vector3, use the ``.v`` union element

    Vector3 color = Colors::White;

