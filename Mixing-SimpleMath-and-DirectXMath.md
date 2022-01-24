|[[Getting Started]] for DX11|[Getting Started](https://github.com/microsoft/DirectXTK12/wiki/Getting-Started) for DX12|
|---|---|

This lesson discusses how to make use of more advanced math functions provided in the DirectXMath library using existing SimpleMath types.

# Coordinate system

The SimpleMath wrapper assumes you are using right-handed coordinates, as does the remainder of the _DirectX Tool Kit_ consistent with the default of _XNA Game Studio_. DirectXMath and Direct3D 11 generally, however, supports both right-handed and left-handed coordinates. If left-handed coordinates are desired, instead of using the handed Matrix methods, you use the DirectXMath LH equivalent function.

```cpp
m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
    Vector3::Zero, Vector3::UnitY);
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(backBufferWidth) / float(backBufferHeight), 0.1f, 10.f);
```

would instead become:

```cpp
m_view = XMMatrixLookAtLH(Vector3(2.f, 2.f, 2.f),
    Vector3::Zero, Vector3::UnitY);
m_proj = XMMatrixPerspectiveFovLH( XM_PI / 4.f,
    float(backBufferWidth) / float(backBufferHeight), 0.1f, 10.f);
```

> See [[SimpleMath]] for a complete list of Matrix methods that are 'handed'.

Many 'handed' _DirectX Tool Kit_ functions take a bool parameter _rhcoords_ that defaults to true. You should pass false when using left-handed view coordinates.

[Right-hand Rule](http://en.wikipedia.org/wiki/Right-hand_rule)

# Constants

A convenient way to build up constants with SimpleMath is to use the C++ constructers such as:

```cpp
Vector3 a(10.f, -.5f, 2.5f);
```

This compiles fine, but at runtime this actually executes a little bit of code to build the structure up. For a vector like this where all the values are literals and known at compile time, a more efficient way to code them is to use DirectXMath's ``XMVECTORF32`` and related types:

```cpp
static const XMVECTORF32 s_a = { 10.f, -.5f, 2.5f, 0.f };
```

This becomes a vector laid out properly in data memory in your program ready for immediate use.

Options here include using:
* ``XMVECTORF32`` which is 4x floats
* ``XMVECTORI32`` which is 4x 32-bit ints
* ``XMVECTORU32`` which is 4x 32-bit unsigned ints
* ``XMVECTORU8`` which is 16x 8-bit unsigned ints

SimpleMath will freely convert all four of these data types into Vector2, Vector3, Vector4, Plane, Quaternion, or Color.

```cpp
static const XMVECTORF32 s_lookat = { 2.f, 2.f, 2.f, 0.f };
m_view = Matrix::CreateLookAt( s_lookat, Vector3::Zero, Vector3::UnitY);
```

# Fresnel term

Because of the free conversion of SimpleMath types, you can easily mix existing SimpleMath code with DirectXMath functions. For example, the [XMFresnelTerm](https://docs.microsoft.com/en-us/windows/desktop/api/directxmath/nf-directxmath-xmfresnelterm) function doesn't have a SimpleMath equivalent:

```cpp
Vector3 fresnel = XMFresnelTerm( Vector4( a1, a2, a3, a4 ),
    Vector4( ri1, ri2, ri3, ri4 ) );
```

# PackedVector

DirectXMath includes a large collection of types and load/store methods in the **DirectX::PackedVector** namespace for converting to and from numerous specialized GPU types.

For example, if we want to create the initData for a texture in ``DXGI_FORMAT_R9G9B9E5_SHAREDEXP`` format:

```cpp
std::unique_ptr<DirectX::PackedVector::XMFLOAT3SE]]> data(
    new DirectX::PackedVector::XMFLOAT3SE[width * height] );

for ( size_t h = 0; h < height; ++h )
{
    for( size_t w = 0; w < width; ++w  )
    {
        Vector3 pixel; // our data from somewhere
        XMStoreFloat3SE( &data[ w * h ], pixel );
    }
}
```

[DirectXMath Library Internals](https://docs.microsoft.com/en-us/windows/desktop/dxmath/pg-xnamath-internals)

**Next lesson:** [[Adding the DirectX Tool Kit for Audio]]

# Further reading

[Introducing DirectXMath](https://walbourn.github.io/introducing-directxmath/)  
[DirectXMath 3.06](https://walbourn.github.io/directxmath-3-06/)  
[Known Issues: DirectXMath 3.06](https://walbourn.github.io/known-issues-directxmath-3-06/)  
[Spherical Harmonics Math](https://walbourn.github.io/spherical-harmonics-math/)  
[XDSP.H: Digital Signal Processing helper functions](https://walbourn.github.io/xdsp-h-digital-signal-processing-helper-functions/)  
[DirectXMath instruction extensions series](https://walbourn.github.io/directxmath-sse-sse2-and-arm-neon/)
