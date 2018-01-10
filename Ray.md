Ray class modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Ray``) math library.

# Header

    #include <SimpleMath.h>

# Initialization

    using namespace DirectX::SimpleMath;

    Ray r;                                // Create a ray starting at the origin
                                          // pointing in positive Z
    Ray( Vector3(1,2,3), Vector3(0,1,0 ); // Creates a ray at 1,2,3
                                          // pointing in positive Y

# Fields
* *position*: Position of the ray
* *direction*: Direction vector of the ray

# Methods
* Comparison operators: ``==`` and ``!=``
* **Intersects**: Performs intersection tests of the ray against a collision type, a triangle, or a [[Plane]].

# Further reading

[Ray](https://en.wikipedia.org/wiki/Line_(geometry)#Ray)  
