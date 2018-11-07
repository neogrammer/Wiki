``SimpleMath.h`` wraps the [DirectXMath](http://blogs.msdn.com/b/chuckw/archive/2012/03/27/introducing-directxmath.aspx) SIMD vector/matrix math API with an easier to use C++ interface. It provides the following types, with similar names, methods, and operator overloads to the XNA Game Studio math API:

* [[Vector2]]
* [[Vector3]]
* [[Vector4]]
* [[Matrix]]
* [[Quaternion]]
* [[Plane]]
* [[Ray]]
* [[Rectangle]]
* [[Color]]
* [[Viewport]]

**Related tutorials:** [[Using the SimpleMath library]], [[Basic game math]], [[Collision detection]], [[Mixing SimpleMath and DirectXMath]]

# Header
```cpp
#include <d3d11.h>
#include "SimpleMath.h"
```

> If using _DirectX Tool Kit for DirectX 12_, you'd use ``#include <d3d12.h>`` instead of ``#include <d3d11.h>``.

# Namespace

All the functions in SimpleMath are in the **DirectX::SimpleMath** C++ namespace.

```cpp
using namespace DirectX::SimpleMath;
```

# Why wrap DirectXMath?

DirectXMath provides highly optimized vector and matrix math functions, which take advantage of SSE SIMD intrinsics when compiled for x86/x64, or the ARM NEON instruction set when compiled for an ARM platform such as Windows RT or Windows Phone. The downside of being designed for efficient SIMD usage is that DirectXMath can be somewhat complicated to work with. Developers must be aware of correct type usage (understanding the difference between SIMD register types such as ``XMVECTOR`` vs. memory storage types such as ``XMFLOAT4``), must take care to maintain correct alignment for SIMD heap allocations, and must carefully structure their code to avoid accessing individual components from a SIMD register. This complexity is necessary for optimal SIMD performance, but sometimes you just want to get stuff working without so much hassle!

# Enter SimpleMath...

These types derive from the equivalent DirectXMath memory storage types (for instance ``Vector3`` is derived from ``XMFLOAT3``), so they can be stored in arbitrary locations without worrying about SIMD alignment, and individual components can be accessed without bothering to call SIMD accessor functions. But unlike ``XMFLOAT3``, the ``Vector3`` type defines a rich set of methods and overloaded operators, so it can be directly manipulated without having to first load its value into an ``XMVECTOR``. ``Vector3`` also defines an operator for automatic conversion to ``XMVECTOR``, so it can be passed directly to methods that were written to use the lower level DirectXMath types.

If that sounds horribly confusing, the short version is that the SimpleMath types pretty much "Just Work" the way you would expect them to.

By now you must be wondering, where is the catch? And of course there is one. SimpleMath hides the complexities of SIMD programming by automatically converting back and forth between memory and SIMD register types, which tends to generate additional load and store instructions. This can add significant overhead compared to the lower level DirectXMath approach, where SIMD loads and stores are under explicit control of the programmer.

You should use SimpleMath if you are:

* Looking for a C++ math library with similar API to the C# XNA types
* Porting existing XNA code from C# to C++
* Wanting to optimize for programmer efficiency (simplicity, readability, development speed) at the expense of runtime efficiency

You should go straight to the underlying DirectXMath API if you:

* Want to create the fastest possible code
* Enjoy the lateral thinking needed to express algorithms in raw SIMD

This need not be a global either/or decision. The SimpleMath types know how to convert themselves to and from the corresponding DirectXMath types, so it is easy to mix and match. You can use SimpleMath for the parts of your program where readability and development time matter most, then drop down to DirectXMath for performance hotspots where runtime efficiency is more important.

# Collision types

The SimpleMath wrapper does not include classes for bounding volumes because they are already implemented in the DirectXMath library, and are included by the ``SimpleMath.h`` header.

This includes:

* [BoundingSphere](https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.directxcollision.boundingsphere.aspx) class
* [BoundingBox](https://docs.microsoft.com/en-us/windows/desktop/api/directxcollision/ns-directxcollision-boundingbox) class
* [BoundingOrientedBox](https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.directxmath.boundingorientedbox.aspx) class
* [BoundingFrustum](https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.directxmath.boundingfrustum.aspx) class
* [TriangleTests](https://docs.microsoft.com/en-us/windows/desktop/dxmath/ovw-xnamath-triangletests) namespace

These were designed to already be similar to the XNA Game Studio math API bounding types.

> Note: Currently the ``BoundingFrustum`` class is not compatible with Right-Handed coordinates.

# Coordinate Systems

Because SimpleMath Is intended to look a lot like like XNA Game Studio's math library, it uses _right-handed coordinates_ by convention for the following Matrix methods:

* CreatePerspectiveFieldOfView
* CreatePerspective
* CreatePerspectiveOffCenter
* CreateOrthographic
* CreateOrthographicOffCenter
* CreateLookAt

If you want to use _left-handed coordinates_, use the DirectXMath methods directly. For example

```cpp
// Here's a RH example
Vector3 eye, target, up;
...
Matrix mView = Matrix::CreateLookAt( eye, target, up );

// Here' is a LH example of same thing which relies on
// Vector3 and Matrix conversion operators
Vector3 eye, target, up;
...
Matrix mView = XMMatrixLookAtLH( eye, target, up );
```

With the methods ``Matrix::CreateBillboard`` and ``Matrix::CreateConstrainedBillboard`` the handedness is inherent in the default vectors for _cameraForward_, and _objectForward_ which are _right-handed_. You can make them behave _left-handed_ by providing appropriate vectors.

```cpp
Vector3 objectPosition, cameraPosition, rotateAxis;

Matrix mView = Matrix::CreateBillboard( objectPosition,
    cameraPosition, Vector3::Up, Vector3::Backward );

Matrix mView = Matrix::CreateConstrainedBillboard( objectPosition,
    cameraPosition, rotateAxis, Vector3::Backward, Vector3::Backward );
```

# Using with HLSL
SimpleMath as with DirectXMath uses [row-major ordering](https://en.wikipedia.org/wiki/Row-major_order) for matrices. This means that elements are stored in memory in the following order:``_11``, ``_12``, ``_13``, ``_14``, ``_21``, ``_22``, etc.

HLSL as noted on [Microsoft Docs](https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx-graphics-hlsl-per-component-math#Matrix_Ordering) defaults to using _column-major ordering_ as this makes for slightly more efficient shader matrix multiplies. Therefore, if a ``Matrix`` is going to be copied into a HLSL _constant buffer_, it is usually [transposed](https://en.wikipedia.org/wiki/Transpose) to flip the ordering to ``_11``, ``_21``, ``_31``, ``_41_``, ``_12``, ``_22``, etc. as part of updating the constant buffer.

With the built-in [[Effects]] this is done internally, but if writing your own shaders and managing your own constant buffers, you will need to ensure you pass in your matrix data in the correct order for your HLSL shader settings. This means sticking with the HLSL default by transposing your matrices as you update the constant buffer, using `` #pragma pack_matrix(row_major)`` in your HLSL shader source, or compiling your shaders with ``D3DCOMPILE_PACK_MATRIX_ROW_MAJOR`` / ``/Zpr``.

# Windows Store apps
A number of Windows Store samples include a ``BasicMath.h`` header that includes some very simplistic math types:

* Vector2
* Vector3
* Vector4
* Matrix4x4

It is fairly easy to use SimpleMath in place of BasicMath, but keep in mind that the BasicMath.h header types are actually templates so they are typically used as "Vector2<float>". SimpleMath types are always single-precision float numbers so do not make use of template syntax.

``BasicMath.h`` also includes typedef aliases for these types to make them use the same naming as HLSL vector types, although the usage syntax and operations are quite different:

* float2
* float3
* float4
* float4x4

If you like the HLSL-like type names, you could use SimpleMath in place of BasicMath.h by adding a [type alias](http://en.cppreference.com/w/cpp/language/type_alias):

```cpp
using float2 = DirectX::SimpleMath::Vector2;
using float3 = DirectX::SimpleMath::Vector3;
using float4 = DirectX::SimpleMath::Vector4;
using float4x4 = DirectX::SimpleMath::Matrix;
```

**Note:** The actual operations would look different than the way they would using BasicMath.

# XMVECTOR and XMMATRIX

The DirectXMath library is intended as a light-weight wrapper over the underlying SIMD instructions supported by modern CPUs. SIMD stands for 'Single-Instruction-Multiple-Data' and is a specific form of parallelism. The idea is pretty straight-forward: the registers X and Y contain more than one element (commonly 4 32-bit floating point values) in each. You then do an instruction, such as X * Y, and the output is more than one result (commonly 4 values from X are multiplied with 4 values from Y resulting in 4 values in the result).

DirectXMath is by design a 'leaky abstraction' in that it exposes the architectural restrictions to the developer. The 'work-horse' types that essentially every DirectXMath function takes as input parameters and produces as results is either ``XMVECTOR`` (4 32-bit floating-point values) or ``XMMATRIX`` (a 4x4 matrix of 32-bit floating-point values). These types have a strict requirement of being aligned to 16-byte boundaries in memory. If this restriction is not met, the program will crash with a mis-alignment exception at runtime.

Using ``XMVECTOR`` or ``XMMATRIX`` as local variables on the stack or as a global or static variable in memory will automatically respect the alignment requirements, but this is not necessarily true of memory allocated from the 'heap' (i.e. ``new`` for C++ programs or ``malloc`` for C programs). It happens that x64 native (64-bit) applications use 16-byte alignment for the heap by default, but this is not true of x86 (32-bit) or ARM applications. As such, using ``XMVECTOR`` or ``XMMATRIX`` directly in a struct or class allocated on the heap can be dangerous or at least less trivial than it first appears. There are ways to ensure that heap-allocated memory is always aligned, but generally it's more trouble than it's really worth in practice. There are also subtle ways that C++ constructs can misalign structures that you thought were properly aligned.

As such, DirectXMath includes a number of memory analogs for ``XMVECTOR`` and ``XMMATRIX``, and explicit load & store operations for moving the data into and out of them. This is actually quite efficient, particularly if you are able to move them from memory into an ``XMVECTOR`` or ``XMMATRIX``, then perform a long chain of operations on that data before storing the result back to memory.

One of SimpleMath's primary functions is to _hide_ the existence of ``XMVECTOR`` and ``XMMATRIX``. Instead, you can use ``Vector3`` or ``Matrix`` without any alignment requirements, and the C++ compiler will automatically call the required load and store functions. This makes working with DirectXMath much more forgiving, at the potential cost of extra load and stores taking place 'invisible' to the programmer.

Since this is implemented using C++ conversion operators, you can at any point use a SimpleMath type directly with a DirectXMath function that takes ``XMVECTOR`` and/or ``XMMATRIX``. They are also completely equivalent to a standard memory type in the DirectXMath library:

* Vector2 <-> ``XMFLOAT2``, ``XMVECTOR``
* Vector3 <-> ``XMFLOAT3``, ``XMVECTOR``
* Vector4 <-> ``XMFLOAT4``, ``XMVECTOR``
* Quaternion <-> ``XMFLOAT4``, ``XMVECTOR``
* Color <-> ``XMFLOAT4``, ``XMVECTOR``
* Plane <-> ``XMFLOAT4``, ``XMVECTOR``
* Matrix <-> ``XMFLOAT4X4``, ``XMMATRIX``

You can therefore freely mix SimpleMath types with DirectXMath functions at any point.

# Further Reading
[DirectXMath](https://github.com/Microsoft/DirectXMath/wiki) project wiki

[SimpleMath - a simplified wrapper for DirectXMath](http://blogs.msdn.com/b/shawnhar/archive/2013/01/08/simplemath-a-simplified-wrapper-for-directxmath.aspx)
