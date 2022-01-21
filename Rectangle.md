|[[DirectXTK]]|[[SimpleMath]]|
|---|---|

A 2D rectangle class modeled after the XNA Game Studio 4 (``Microsoft.Xna.Framework.Rectangle``) math library

> For Universal Windows Platform and Xbox One, similar functionality is provided by the [Windows::Foundation::Rect](https://docs.microsoft.com/en-us/uwp/api/Windows.Foundation.Rect) Windows Runtime structure

# Header
```cpp
#include <SimpleMath.h>
```

# Initialization

```cpp
using namespace DirectX::SimpleMath;

Rectangle r;                    // Creates an empty rectangle [0, 0, 0, 0]
Rectangle r(10, 20, 100, 200);  // Creates a rectangle with upper-left [10,20]
                                // 100 width, and 200 height
```

> For Windows desktop apps, you may find that ``Rectangle`` conflicts with the GDI function of the same name. You can resolve this three ways: Use ``#define NOGDI`` before you include ``<Windows.h>``; use ``SimpleMath::Rectangle``, or in local scope use ``using Rectangle = SimpleMath::Rectangle;``.

# Fields
* *x* location of the upper-left corner
* *y* location of the upper-left corner
* *width* of the rectangle
* *height* of the rectangle

# Methods
* Comparison operators: ``==`` and ``!=``
* Assignment operator: ``=``

* **Location**, **Center**: Returns the rectangle position as a [[Vector2]]

* **IsEmpty**: Returns true if the rectangle is [0, 0, 0, 0]

* **Contains**: Tests to see if a point or rectangle is contained by the rectangle

* **Inflate**: Grows the rectangle by a specified amount in the horizontal and vertical dimensions

* **Intersects**: Tests to see if another rectangle intersects with this rectangle

* **Offset**: Moves or offsets the rectangle by moving the upper-left position

# Statics
* **Intersect**: Computes the intersection of two rectangles or the empty rectangle [0, 0, 0, 0] if there is no intersection

* **Union**: Computes the union of two rectangles

# Remark
Rectangle will freely convert to and from a ``RECT``. In UWP and Xbox One apps, you can also convert it to a ``Windows::Foundation::Rect``.
