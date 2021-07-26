This lesson introduces the reader to the SimpleMath game and graphics math library.

# Background

The *DirectX Tool Kit* tutorials assume a basic understanding of the classic "world -> view -> projection" [graphics pipeline](https://en.wikipedia.org/wiki/Graphics_pipeline), [transformation matricies](https://en.wikipedia.org/wiki/Transformation_matrix), and in particular the classic [affine transformations](https://en.wikipedia.org/wiki/Affine_transformation) of *translation*, *scaling*, and *rotation*.

A great resource for learning more about the essential math of graphics programming, see [Essential Math for Games Programmers](http://www.essentialmath.com/).

# Coordinate system
Consistent with the original XNA Game Framework C# math library, SimpleMath assumes a [right-handed](https://en.wikipedia.org/wiki/Right-hand_rule) coordinate system, with the positive z-axis pointing toward the observer when the positive x-axis is pointing to the right, and the positive y-axis is pointing up.

# Namespace
The operator overloads defined in the SimpleMath library are defined in the ``DirectX::SimpleMath`` C++ namespace. You therefore need to add ``using`` statements to get them to properly resolve:

```cpp
using namespace DirectX;
using namespace DirectX::SimpleMath;
```

Typically you put these statements at the top of your ``.cpp`` files just after all ``#include`` statements. You can also put the ``using`` statements in local scope for inline function bodies or if you only use SimpleMath in one specific function.

> Remember you should avoid putting ``using namespace`` statements at global scope in header files. C++ best practice is to use full name resolution in headers for types (i.e. ``DirectX::SimpleMath::Matrix m_proj;``).

# Vectors
SimpleMath provides the **Vector2**, **Vector3**, and **Vector4** classes for representing and manipulating vectors. A vector typically is used to represent a direction and magnitude. It can also be used to represent points in 2D, 3D, or 4D homogenous spaces.

Each vector class has methods for performing standard vector operations such as:
* Dot product
* Cross product
* Normalization
* Transformation
* Linear, Cubic, Catmull-Rom, or Hermite spline interpolation.

For example:

```cpp
Vector3 upVector( 0, 1.f, 0 );
Vector3 leftVector( 1.f, 0, 0 );
float dot = upVector.Dot( leftVector );
```

# Matrices
SimpleMath provides a **Matrix** class for transformation of geometry. The Matrix class uses *row-major* order to address matrices, which means that the row is specified before the column when describing an element of a two-dimensional matrix. The Matrix class provides methods for performing standard matrix operations such as calculating the determinate or inverse of a matrix. There also are helper methods for creating scale, rotation, and translation matrices.

```cpp
Matrix a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
Matrix b(1, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 23, 42, 0, 1);
Matrix c = a * b;
```

# Quaternions
The ***Quaternion*** structure to calculate the efficient rotation of a vector by a specified angle, and is particularly effective when interpolating between angles.

```cpp
Quaternion a(0.707107f, 0, 0, 0.707107f);
Quaternion (0, 0.707107f, 0, 0.707107f);
Quaternion c = Quaternion::Slerp(a, b, 0.25f);
```

# Homogenous Coordinates

Matrix is 4x4 single-precision floating-point and is normally assumed to be [homogenous coordinates](https://en.wikipedia.org/wiki/Homogeneous_coordinates) as per the standard 3D transformation pipeline:

```
| _11  _12  _13  _14 |
| _21  _22  _23  _24 |
| _31  _32  _33  _34 |
| _41  _42  _43  _44 |
```

The upper-3x3 matrix is used for 3D scaling (_11, _22, _33) & rotation transformations (as well as shearing, mirroring, etc.):

```
| _11  _12  _13  ? |
| _21  _22  _23  ? |
| _31  _32  _33  ? |
| ?    ?    ?    ? |
```

The last row's first three elements are for translation transformations:

```
| r    r    r   ? |
| r    r    r   ? |
| r    r    r   ? |
| _41  _42  _43 ? |
```

And the last column is used for projection transformations (i.e. perspective cameras, simple shadows):

```
| r   r   r   _14 |
| r   r   r   _24 |
| r   r   r   _34 |
| tx  ty  tz  _44 |
```

When transforming a vector, to get back to true '3D' realspace, you must divide through by the 'w' result to get it back to 1 for the x, y, z elements to have physical meaning--if the projection column is 0, 0, 0, 1 then the result is already guaranteed to be a ``w`` of 1.

```
[x y z w] = [x/w y/w z/w 1]
```

An analogous thing can be done in 2D with a 3x3 transformation matrix and 2 vectors. For this purpose, our existing Matrix class is used ignoring the 3rd row:

```
| r   r   0  p |
| r   r   0  p |
| 0   0   0  0 |
| tx  ty  0  p |
```

# Bounding Volumes
The **BoundingBox**, **BoudingOrientedBox**, **BoundingFrustum**, **BoundingSphere**, **Plane**, and **Ray** classes provides for representing simplified versions of geometry for the purpose of efficient collision and hit testing. These classes have methods for checking for intersection and containment with each other. See the [[Collision detection]] tutorial for more details.

# Precision and Performance
The SimpleMath types are single-precision. This means that the primitives and operations contained in this library use 32-bit floating-point numbers to achieve a balance between precision and efficiency when performing large numbers of calculations.

A 32-bit floating-point number ranges from ``–3.402823e38`` to ``+3.402823e38``. The 32 bits store the sign, mantissa, and exponent of the number that yields seven digits of floating-point precision. Some numbers—for example π, 1/3, or the square root of two—can be approximated only with seven digits of precision, so be aware of rounding errors when using a binary representation of a floating-point number.

**Next lesson:** [[Basic game math]]

# Further reading
DirectX Tool Kit docs [[SimpleMath]]  

[SimpleMath - a simplified wrapper for DirectXMath](http://www.shawnhargreaves.com/blog/simplemath-a-simplified-wrapper-for-directxmath.html)

[DirectXMath](https://github.com/Microsoft/DirectXMath/wiki) project wiki

[What Every Computer Scientist Should Know About Floating-Point Arithmetic](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html)
