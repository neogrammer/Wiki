Four component color vector class modeled after the [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.color.aspx) math library.

# Header

    #include <SimpleMath.h>

# Initialization

    using namespace DirectX::SimpleMath;

    Color c;                      // Creates opaque black
    Color c(1, 1, 1);             // Creates opaque white
    Color c(1, 1, 1, 0.5f);       // Creates white with 50% alpha (straight-alpha)
    Color c( Vector3(1,1,1) );    // Creates opaque white
    Color c( Vector4(1,1,1,0.5f); // Creates white with 50% alpha (straight-alpha)

    float arr[4] = { 1, 1, 1, 1 };
    Color c(arr);                  // Creates opaque white

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
* **BGRA**: Converts to a ``DirectX::PackedVector::XMCOLOR`` Direct3D 9 style color
* **RGBA**: Converts to a ``DirectX::PackedVector::XMUBYTEN4`` XNA Game Studio style color
* **ToVector3**
* **ToVector4**
* **Negate**
* **Saturate**: Clamps the color to the 0 to 1 range
* **Premultiply**: Converts straight-alpha color to premultiplied alpha
* **AdjustSaturation**
* **AdjustContrast**

# Statics
* **Modulate**
* **Lerp**

# Remark
Color can freely convert to and from a ``XMFLOAT4`` and ``XMVECTOR``

When assigning a color from ``DirectXColors.h`` to a Color, use the ``.v`` union element

    Color color = Colors::White.v;

Color can be copy and assigned from ``DirectX::PackedVector::XMCOLOR`` and ``DirectX::PackedVector::XMUBYTEN4``

Color component values can exceed the 0 to 1 range for High Dynamic Range (HDR) lighting. Alpha values greater than 1 typically don't have a valid meaning.
