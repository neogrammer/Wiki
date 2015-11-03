A four by four row-major matrix modeled after the [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.matrix.aspx) math library.

# Header

    #include <SimpleMath.h>

# Initialization

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
* **Decompose**: Decompose the matrix into rotation, scaling, and translation components
* **Transpose**
* **Invert**
* **Determinant**

# Statics
* **CreateBillboard**: Creates a spherical billboard that rotates around a specified object position
* **CreateConstrainedBillboard**: Creates a cylindrical billboard that rotates around a specified axis 
* **CreateTranslation**
* **CreateScale**
* **CreateRotationX**
* **CreateRotationY**
* **CreateRotationZ**
* **CreateFromAxisAngle**
* **CreatePerspective**, **CreatePerspectiveFieldOfView**, **CreatePerspectiveOffCenter**: Creates perspective projection in right-handed (RH) coordinates
* **CreateOrthographic**, **CreateOrthographicOffCenter**: Creates orthographic projection in right-handed (RH) coordinates
* **CreateLookAt**: Creates a look-at matrix in right-handed (RH) coordinates
* **CreateWorld**
* **CreateFromQuaternion**
* **CreateFromYawPitchRoll**
* **CreateShadow**
* **CreateReflection**
* **Lerp**
* **Transform**

# Constants

* **Identity**: An identity matrix

# Remark
Matrix can freely convert to and from a ``XMFLOAT4X4`` and ``XMMATRIX``.

You can copy or assign a Matrix from a ``XMFLOAT3X3`` or ``XMFLOAT4X3``.
