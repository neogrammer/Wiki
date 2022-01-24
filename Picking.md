|[[Getting Started]] for DX11|[Getting Started](https://github.com/microsoft/DirectXTK12/wiki/Getting-Started) for DX12|
|---|---|

This lesson covers performing picking in 3D.

# Ray casting

You can find an older tutorial on 3D picking using ray/object tests at [Rastertek](http://rastertek.com/dx11tut47.html) that uses the deprecated D3DXMath library. Note that instead of a 'roll your own' ``RaySphereIntersect`` you can use [BoundingSphere](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingsphere) from ``DirectXCollision.h`` with the following method to do the ray-sphere test:

```cpp
bool Intersects(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist) const;
```

Also remember that the default view coordinates for *DirectX Tool Kit* is "right-handed" viewing coordinates and the Rastertek tutorials follow the older DirectX style of "left-handed" viewing coordinates. This requires reversing the Z direction of the ray.

You can also review the Direct3D 10 era "picking" sample from the [legacy DirectX SDK](https://github.com/walbourn/directx-sdk-legacy-samples/tree/main/Direct3D10/Pick10).

**Next lesson**: [[Mixing SimpleMath and DirectXMath]]

# Further reading

[DirectMath Programmer's Guide](https://docs.microsoft.com/en-us/windows/desktop/dxmath/ovw-xnamath-reference)  
[DirectXMath Collision sample](https://code.msdn.microsoft.com/DirectXMath-Win32-Sample-f365b9e5)
