An infinite 2D plane represented as a four component vector modeled after the [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.plane.aspx) math library.

# Header

    #include <SimpleMath.h>

# Initialization

    using namespace DirectX::SimpleMath;

    Plane p;                      // Creates a plane in XZ at the origin
    Plane p(0, 1, 0, 0);          // Creates a plane in XZ at the origin
    Plane p( Vector3(0,1,0), 0 ); // Creates a plane in XZ at the origin
    Plane p( Vector3(1,1,1),
             Vector3(2,2,2),
             Vector3(3,3,3));     // Creates a plane containing the three points
    Plane p( Vector3(1,2,3),
             Vector3(0,0,1) );    // Creates a plane at [1,2,3] with the normal [0,0,1]
    
    float arr[4] = { 0, 1, 0, 0 };
    Plane p(arr);                 // Creates a plane in XZ at the origin

# Fields
* *x* component of the plane normal (A)
* *y* component of the plane normal (B)
* *z* component of the plane normal (D)
* *w* scalar distance to the plane from the origin (D)

> This is the *Cartesian form of the equation of a plane*: A*x + B*y + C*z + D

# Properties
* **Normal**: Returns the normal vector for the plane
* **D**: Returns the distance to the plane

# Methods
* Comparison operators: ``==`` and ``!=``
* Assignment operators: ``=``
* **Normalize**
* **Dot**: Performs a 4D dot product between a 4 vector and the plane
* **DotCoordinate**: Performs a point test against the plane, which returns the distance between the point and the plane.
* **DotNormal**: Performs a dot product against just the normal of the plane.

# Statics
* **Transform**

> Planes should be transformed by the inverse transpose of the matrix, which for pure rotations results in the same matrix as the original.

# Remark
Plane can freely convert to and from a ``XMFLOAT4`` and ``XMVECTOR``

# Further reading
[Plane](https://en.wikipedia.org/wiki/Plane_(geometry))  
