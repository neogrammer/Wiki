This lesson covers performing picking in 3D.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Ray casting

> **UNDER CONSTRUCTION**

You can find an older tutorial on 3D picking using ray/object tests at [Rastertek](http://rastertek.com/dx11tut47.html) that uses the deprecated D3DXMath library. Note that instead of a 'roll your own' ``RaySphereIntersect`` you can use [BoundingSphere](https://docs.microsoft.com/en-us/windows/win32/api/directxcollision/ns-directxcollision-boundingsphere) from ``DirectXCollision.h`` with the following method to do the ray-sphere test:

```cpp
bool Intersects(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist) const;
```

**Next lesson**: [[Mixing SimpleMath and DirectXMath]]

# Further reading

[DirectMath Programmer's Guide](https://docs.microsoft.com/en-us/windows/desktop/dxmath/ovw-xnamath-reference)  
[DirectXMath Collision sample](https://code.msdn.microsoft.com/DirectXMath-Win32-Sample-f365b9e5)
