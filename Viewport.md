Viewport class modeled after the [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.viewport.aspxaspx) graphics library.

# Header

    #include <SimpleMath.h>

# Initialization

    using namespace DirectX::SimpleMath;

    Viewport vp;                        // Creates the viewport [0,0,0,0,0,1]
    Viewport vp(0,0,640,480);           // Creates the viewport [0,0,640,480,0,1]
    Viewport vp(0,0,640,480,0.1f,0.9f); // Creates the viewport [0,0,640.480,0.1,0.9]

    RECT rct;
    rct.top = 0;
    rct.left = 0;
    rct.right = 640;
    rct.bottom = 480;
    Viewport vp(rct); // Creates the viewport [0,0,640,480,0,1]

# Fields
* *x*: Upper left x position of the viewport
* *y*: Upper left y position of the viewport
* *width*: The width of the viewport
* *height*: The height of the viewport
* *minDepth*: The minimum depth of the viewport
* *maxDepth*: The maximum depth of the viewport

# Methods
* Comparison operators: ``==`` and ``!=``
* Assignment operators: ``=``

* **AspectRatio**: Compute the aspect ratio. If the width or height is zero, it returns 0

* **Project**: Projects a 3D point using given world, view, and projection matrices an the viewport, moving from world coordinates to screen space.

* **Unproject**: Reverse projects a 3D point using given world, view, and projection matrices an the viewport moving from screen space to world coordinates.

# Statics
* **ComputeDisplayArea**: When using various scaling modes via ``DXGI_SWAP_CHAIN_DESC1.Scaling``, the backbuffer width and height is not necessarily the same size as the output window. Given the backbuffer width and height in pixels, the window output width and height in pixels, and the scaling setting, this function computes the pixel rectangle where the backbuffer content is visible within the window. This can be used to handle mouse/touch input and map back to the pixel location within the backbuffer.

> If using ICoreWindow, be sure to convert logicalWidth and logicalHeight from DIPs to pixels using the window's DPI property. ``int ConvertDipsToPixels(float dips, float dpi) { return int(dips * dpi / 96.f + 0.5f); }``

* **ComputeTitleSafeArea**: When displaying on televisions, edges of the screen can't always be seen by users. This function takes a backbuffer width and height in pixels and returns a [safe area](https://en.wikipedia.org/wiki/Safe_area_%28television%29) rectangle for the content to be displayed. This function uses the [SMPTE ST 2046-1](http://www.nab.org/xert/scitech/pdfs/tv031510.pdf) standard of 10% safe area intended for HDTVs, rather than the older SMPTE RP 8 intended for CRTs that used 20% safe area which is what the original XNA Game Studio math function implemented.

> Note that the XNA Game Studio equivalent of this function only returned the reduced safe area on Xbox, and returns the full rectangle otherwise on Windows phone and PC. Because the universal Windows app platform can share a single binary or set of binaries across many devices, it is up to the app to determine what device it is currently running under. Therefore, this function always returns the reduced safe area and it is up to the app to apply this in the correct circumstances.

# Remarks
The Viewport can be converted to and from a ``D3D11_VIEWPORT`` structure.

To use with Direct3D 11, you typically need an address of the viewport as a ``const D3D11_VIEWPORT*`` which is provided by ``Get11``:

    Viewport viewPort(0.0f, 0.0f, outputWidth, outputHeight);

    context->RSSetViewports(1, viewPort.Get11());

