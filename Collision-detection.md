This lesson covers simple collision detection using rectangles, spheres, and boxes.

# 2D collision detection

The primary functionality here offered by the *DirectX Tool Kit* is in [[SimpleMath]]'s ``Rectangle`` class:

The **Contains** methods perform *rectangle contains point* and *rectangle contains rectangle* tests:

```cpp
bool Contains(long ix, long iy) const;
bool Contains(const Vector2& point) const;
bool Contains(const Rectangle& r) const;
bool Contains(const RECT& rct) const;
```

The **Intersects** methods perform *rectangle/rectangle* intersection tests:

```cpp
bool Intersects(const Rectangle& r) const;
bool Intersects(const RECT& rct) const;
static Rectangle Intersect(const Rectangle& ra, const Rectangle& rb);
static RECT Intersect(const RECT& rcta, const RECT& rctb);
```

The **Inflate** and **Union** methods can be useful for adjusting collision rectangles:

```cpp
void Inflate(long horizAmount, long vertAmount);
static Rectangle Union(const Rectangle& ra, const Rectangle& rb);
static RECT Union(const RECT& rcta, const RECT& rctb);
```

# 3D collision detection

For 3D collision implementation, the [DirectXMath](https://docs.microsoft.com/en-us/windows/win32/dxmath/directxmath-portal) bounding classes provide implementations of many 3D test. If desired, you can interop with these using [[SimpleMath]]'s types like ``Vector3``, ``Vector4``, ``Plane``, ``Ray``, etc.

## BoundingSphere class

The **Contains** method provides *sphere contains point*, *sphere contains triangle*, *sphere contains sphere*, *sphere contains box*, and *sphere contains frustum* tests:

```cpp
ContainmentType Contains(FXMVECTOR Point) const;
ContainmentType Contains(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2) const;
ContainmentType Contains(const BoundingSphere& sh) const;
ContainmentType Contains(const BoundingBox& box) const;
ContainmentType Contains(const BoundingOrientedBox& box) const;
```
    

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingsphere).

## BoundingBox class

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingbox).

## BoundingOrientedBox class

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingorientedbox).

## BoundingFrustum class

See [MicrosoftDocs](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingfrustum).

## TriangleTests namespace

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/dxmath/ovw-xnamath-triangletests).

**Next lesson**: [[Picking]]

# Further reading

[DirectXMath Programmer's Reference](https://docs.microsoft.com/en-us/windows/desktop/dxmath/ovw-xnamath-reference)  
[DirectXMath Collision sample](https://github.com/walbourn/directx-sdk-samples/tree/master/Collision)
