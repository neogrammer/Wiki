This lesson covers simple collision detection using rectangles, spheres, and boxes.

# 2D collision detection

The primary functionality here offered by the *DirectX Tool Kit* is in [[SimpleMath]]'s ``Rectangle`` class:

The **Contains** methods perform *rectangle contains point* and *rectangle contains rectangle* tests.

```cpp
bool Contains(long ix, long iy) const;
bool Contains(const Vector2& point) const;
bool Contains(const Rectangle& r) const;
bool Contains(const RECT& rct) const;
```

The **Intersects** methods perform *rectangle/rectangle* intersection tests.

```cpp
bool Intersects(const Rectangle& r) const;
bool Intersects(const RECT& rct) const;
static Rectangle Intersect(const Rectangle& ra, const Rectangle& rb);
static RECT Intersect(const RECT& rcta, const RECT& rctb);
```

The **Inflate** and **Union** methods can be useful for adjusting collision rectangles.

```cpp
void Inflate(long horizAmount, long vertAmount);
static Rectangle Union(const Rectangle& ra, const Rectangle& rb);
static RECT Union(const RECT& rcta, const RECT& rctb);
```

# 3D collision detection

For 3D collision implementation, the [DirectXMath](https://docs.microsoft.com/en-us/windows/win32/dxmath/directxmath-portal) bounding classes provide implementations of many 3D test. If desired, you can interop with these using [[SimpleMath]]'s types like ``Vector3``, ``Vector4``, ``Plane``, ``Ray``, etc.

## BoundingSphere class
This is the classic 'bounding sphere' collision primitive.

The **Contains** method provides *sphere contains point*, *sphere contains triangle*, *sphere contains sphere*, *sphere contains box*, and *sphere contains frustum* tests.

```cpp
ContainmentType Contains(FXMVECTOR Point) const;
ContainmentType Contains(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2) const;
ContainmentType Contains(const BoundingSphere& sh) const;
ContainmentType Contains(const BoundingBox& box) const;
ContainmentType Contains(const BoundingOrientedBox& box) const;
ContainmentType Contains(const BoundingFrustum& fr) const;
```

The **ContainedBy** method provides *sphere contained by frustum* test.

```cpp
ContainmentType ContainedBy(FXMVECTOR Plane0, FXMVECTOR Plane1, FXMVECTOR Plane2,
    GXMVECTOR Plane3, HXMVECTOR Plane4, HXMVECTOR Plane5) const;
```

The **Intersects** method provides *sphere/sphere*, *sphere/box*, *sphere/frustum*, *sphere/triangle*, *sphere/plane*, and *ray/sphere* intersection tests.

```cpp
bool Intersects(const BoundingSphere& sh) const;
bool Intersects(const BoundingBox& box) const;
bool Intersects(const BoundingOrientedBox& box) const;
bool Intersects(const BoundingFrustum& fr) const;
bool Intersects(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2) const;
PlaneIntersectionType Intersects(FXMVECTOR Plane) const;
bool Intersects(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist) const;
```
    
See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingsphere).

## BoundingBox class
This is an 'axis-aligned bounding box' collision primitive.

The **Contains** method provides *box contains point*, *box contains triangle*, *box contains sphere*, *box contains box*, and *box contains frustum* tests.

The **ContainedBy** method provides *box contained by frustum* test.

The **Intersects** method provides *box/box*, *box/sphere*, *box/frustum*, *box/triangle*, *box/plane* and *ray/box* intersection tests.

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingbox).

## BoundingOrientedBox class
This is an 'oriented bounding box' collision primitive. It provides the same **Contains**, **ContainedBy**, and **Intersects** methods as the axis-aligned bounding box.

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingorientedbox).

## BoundingFrustum class
This is a 'view frustum' collision primitive.

The **Contains** method provides *frustum contains point*, *frustum contains triangle*, *frustum contains sphere*, *frustum contains box*, and *frustum contains frustum* tests.

The **Intersects** method provides *frustum/sphere*, *frustum/box*, *frustum/frustum*, *frustum/triangle*, *frustum/plane*, and *ray/frustum* intersection tests.

See [MicrosoftDocs](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingfrustum).

## TriangleTests namespace
This namespace provides a number of triangle tests not covered by the other bounding classes.

These **Intersects** functions cover *ray/triangle*, *triangle/triangle*, and *plane/triangle* tests.

```cpp
bool Intersects(FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, GXMVECTOR V1, HXMVECTOR V2, _Out_ float& Dist);
bool Intersects(FXMVECTOR A0, FXMVECTOR A1, FXMVECTOR A2, GXMVECTOR B0, HXMVECTOR B1, HXMVECTOR B2);
PlaneIntersectionType Intersects(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2, GXMVECTOR Plane);
```

The *ContainedBy* function covers *frustum contains triangle*.

```cpp
ContainmentType ContainedBy(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2,
    GXMVECTOR Plane0, HXMVECTOR Plane1, HXMVECTOR Plane2,
    CXMVECTOR Plane3, CXMVECTOR Plane4, CXMVECTOR Plane5);
```

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/dxmath/ovw-xnamath-triangletests).

**Next lesson**: [[Picking]]

# Further reading

[DirectXMath Programmer's Reference](https://docs.microsoft.com/en-us/windows/desktop/dxmath/ovw-xnamath-reference)  
[DirectXMath Collision sample](https://github.com/walbourn/directx-sdk-samples/tree/master/Collision)
