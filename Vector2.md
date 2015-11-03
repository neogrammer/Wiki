Two component vector class modeled after the [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.vector2.aspx) math library.

# Header

    #include <SimpleMath.h>

# Initialization

    using namespace DirectX::SimpleMath;

    Vector2 v;        // Creates a vector [0, 0]
    Vector2 v(10);    // Creates a vector [10, 10]
    Vector2 v(1, 2);  // Creates a vector [1, 2]

    float arr[2] = { 1, 2 };
    Vector2 v(arr);   // Creates a vector [1, 2]

# Fields
* *x* component of vector
* *y* component of vector

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

* **Zero**: Zero vector [0, 0]
* **One**: One vector [1, 1]
* **UnitX**: Unit X vector [1, 0]
* **UnitY**: Unit Y vector [0, 1]

# Remark
Vector2 can freely convert to and from a ``XMFLOAT2`` and ``XMVECTOR``
