Four component color vector class modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Color``) math library.

See also [[SimpleMath]]

# Header
```cpp
#include <SimpleMath.h>
```

# Initialization

```cpp
using namespace DirectX::SimpleMath;

Color c;                      // Creates opaque black
Color c(1, 1, 1);             // Creates opaque white
Color c(1, 1, 1, 0.5f);       // Creates white with 50% alpha (straight-alpha)
Color c( Vector3(1,1,1) );    // Creates opaque white
Color c( Vector4(1,1,1,0.5f); // Creates white with 50% alpha (straight-alpha)

float arr[4] = { 1, 1, 1, 1 };
Color c(arr);                  // Creates opaque white
```

# Fields
* *x* is the red component of the color
* *y* is the green component of the color
* *z* is the blue component of the color
* *w* is the alpha component of the color

# Properties
* **R**: Red component
* **G**: Green component
* **B**: Blue component
* **A**: Alpha component

# Methods
* Comparison operators: ``==`` and ``!=``
* Assignment operators: ``=``, ``+=``, ``-=``, ``*=``, ``/=``
* Unary operators: ``+``, ``-``
* Binary operators: ``+``, ``-``, ``*``, ``/``
* **BGRA**: Converts to a ``DirectX::PackedVector::XMCOLOR`` Direct3D 9 style color equivalent to ``D3DCOLOR``.
* **RGBA**: Converts to a ``DirectX::PackedVector::XMUBYTEN4`` XNA Game Studio style packed color
* **ToVector3**
* **ToVector4**
* **Negate**: Negates or inverses the color. _The result is ``Color( 1 - x, 1 - y, 1 - z, w )``._
* **Saturate**: Clamps the color to the 0 to 1 range
* **Premultiply**: Converts straight-alpha color to premultiplied alpha
* **AdjustSaturation**: A parameter of 0 returns the grey-scale of the color, 1 returns the original color
* **AdjustContrast**: A parameter of 0 returns 50% grey of the color, 1 returns the original color

# Statics
* **Modulate**: Blends two colors together
* **Lerp**: Linearly interpolates between two colors

# Remark
Color can freely convert to and from a ``XMFLOAT4`` and ``XMVECTOR``

When assigning a color from ``DirectXColors.h`` to a Color, use the ``.v`` union element

```cpp
Color color = Colors::White.v;
```

Color can be copy and assigned from ``DirectX::PackedVector::XMCOLOR`` (BGRA Direct3D 9 D3DCOLOR packed color) and ``DirectX::PackedVector::XMUBYTEN4`` (RGBA XNA Game Studio packed color).

Color component values can exceed the 0 to 1 range for High Dynamic Range (HDR) lighting. Alpha values greater than 1 typically don't have a valid meaning.
