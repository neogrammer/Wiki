A four by four row-major [matrix](https://en.wikipedia.org/wiki/Matrix_(mathematics)) modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Matrix``) math library.

A 4x4 *matrix* is commonly used in computer graphics for 2D and 3D [transformations](https://en.wikipedia.org/wiki/Transformation_matrix).

# Header
```cpp
#include <SimpleMath.h>
```

# Initialization

```cpp
using namespace DirectX::SimpleMath;

Matrix m;                  // Creates an identity matrix
Matrix m(1, 2, 3, 4,
         5, 6, 7, 8,
         9, 10, 11, 12,
         13, 14, 15, 16);  // Creates a matrix [1 2 3 4
                           //                   | 5 6 7 8
                           //                   | 9 10 11 12
                           //                   | 13 14 15 16 ]
Matrix m( Vector3(1,2,3),
          Vector3(4,5,6),
          Vector(7,8,9) ); // Creates a matrix [1 2 3 0
                           //                   | 4 5 6 0
                           //                   | 7 8 9 0
                           //                   | 0 0 0 1 ]
Matrix m( Vector4(1, 2, 3, 4),
          Vector4(5, 6, 7, 8),
          Vector4(9, 10, 11, 12),
          Vector4(13, 14, 15, 16) ); // Creates a matrix [1 2 3 4
                                     //                   | 5 6 7 8
                                     //                   | 9 10 11 12
                                     //                   | 13 14 15 16 ]
float arr[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
Matrix m( arr );           // Creates a matrix [1 2 3 0
                           //                   | 4 5 6 0
                           //                   | 7 8 9 0
                           //                   | 0 0 0 1 ]
```

# Fields
* *_11*, *_12*, *_13*, *_14*: First row of matrix
* *_21*, *_22*, *_23*, *_24*: Second row of matrix
* *_31*, *_32*, *_33*, *_34*: Third row of matrix
* *_41*, *_42*, *_43*, *_44*: Fourth row of matrix

# Properties
* **Up**: The up direction vector from the matrix
* **Down**: The down direction vector from the matrix
* **Right**: The right direction vector from the matrix
* **Left**: The left direction vector from the matrix
* **Forward**: The forward direction vector in right-handed (RH) coordinates from the matrix
* **Backward**: The backward direction vector in right-handed (RH) coordinates from the matrix
* **Translation**: Returns the translation in the matrix

# Methods

* Comparison operators: ``==`` and ``!=``

* Assignment operators: ``=``, ``+=``, ``-=``, ``*=``, ``/=``

* Unary operators: ``+``, ``-``

* Binary operators: ``+``, ``-``, ``*``, ``/``

* **Decompose**: Decomposes the matrix into rotation (a [[Quaternion]]), scaling, and translation components. Returns ``false`` if the matrix can't be decomposed.

* **Transpose**: Computes the [transpose](https://en.wikipedia.org/wiki/Transpose) of the matrix. Note that for a pure rotation matrix, this is the same as the inverse.

* **Invert**: Computes the inverse of the matrix using [Cramer's rule](https://en.wikipedia.org/wiki/Cramer%27s_rule). It returns ``false`` if the matrix is not invertible.

* **Determinant**: Computes the [determinant](https://en.wikipedia.org/wiki/Determinant) of the matrix.

* **ToEuler**: Computes rotation about y-axis (y), then x-axis (x), then z-axis (z) as angles in radians. The return value is compatible with one of the overloads of ``CreateFromYawPitchRoll``. This result is only valid if the input matrix's upper 3x3 contains only rotation (i.e. no scaling).

# Statics

* **CreateBillboard**: Creates a spherical billboard that rotates around a specified object position

* **CreateConstrainedBillboard**: Creates a cylindrical billboard that rotates around a specified axis

* **CreateTranslation**: Creates a matrix for translating by the given X, Y, Z offset. Note you can create the inverse of this matrix by passing -X, -Y, -Z.

* **CreateScale**: Creates a matrix for scaling by the given X, Y, and Z scale factors. Note you can create the inverse of this matrix by passing 1/X, 1/Y, 1/Z.

* **CreateRotationX**, **CreateRotationY**, **CreateRotationZ**: Creates a matrix for rotation about the x-axis / y-axis / z-axis by angle given in [radians](https://en.wikipedia.org/wiki/Radian).

* **CreateFromAxisAngle**: Creates a matrix a rotation about a given 3-vector axis and an angle given in radians.

* **CreatePerspective**, **CreatePerspectiveFieldOfView**, **CreatePerspectiveOffCenter**: Creates perspective projection in right-handed (RH) coordinates

* **CreateOrthographic**, **CreateOrthographicOffCenter**: Creates orthographic projection in right-handed (RH) coordinates

* **CreateLookAt**: Creates a look-at matrix in right-handed (RH) coordinates

> For Left-Handed (LH) viewing coordinates, you can use the [DirectXMath](https://docs.microsoft.com/en-us/windows/win32/dxmath/ovw-xnamath-reference-functions-matrix) functions directly which interop freely with ``Matrix``.

* **CreateWorld**

* **CreateFromQuaternion**: Converts a quaternion to a matrix.

* **CreateFromYawPitchRoll**: Rotates about y-axis (yaw), then x-axis (pitch), then z-axis (roll) by given angles in radians. There is an overload which takes [[Vector3]] which provides x, y, and z angles in radians.

> The original D3DXmath library took the rotations in the the Yaw, Pitch, Roll order and that order was replicated in *XNA Game Studio*. In DirectXMath, the order was normalized to Roll (X), Pitch (Y), Yaw (Z) for the parameters, but the application of the rotations is in the same order.

* **CreateShadow**: Creates transform that flattens geometry into a specified [[Plane]] as if casting a shadow from a specified light source

* **CreateReflection**: Creates transform that reflects the coordinate system about a specified [[Plane]].

* **Lerp**: Linearly interpolates two matrices element-wise (useful for blending transformations).

* **Transform**

# Constants

* **Identity**: An identity matrix

# Remark
Matrix can freely convert to and from a ``XMFLOAT4X4`` and ``XMMATRIX``.

You can copy or assign a Matrix from a ``XMFLOAT3X3`` or ``XMFLOAT4X3``.

# See also

[[Vector2]], [[Vector3]], [[Vector4]], [[Quaternion]]
