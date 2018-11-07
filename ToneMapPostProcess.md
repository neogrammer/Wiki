These built-in post-processing shaders are designed to perform tone-mapping of High Dynamic Range (HDR) images for presentation on traditional monitors, as well as the signal generation for HDR10 when outputting to 4k UHD wide gamut displays.

See also [[PostProcess]]

# Header
```cpp
#include "PostProcess.h"
```

# Initialization
Construction requires a Direct3D 11 device.

```cpp
std::unique_ptr<ToneMapPostProcess> toneMap;
toneMap = std::make_unique<ToneMapPostProcess>(device);
```

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Shader selection

The shader selection is based on the combination of the tonemap operator and the electro-optical transfer function.

The tonemap operator is controlled by calling ``SetOperator``:

* **None**: No tonemap operator is applied, and the input is passed through the shader to the render target unchanged.
* **Saturate**: A simple clamp to the range 0 to 1 is applied after multiplying by the exposure level.
* **Reinhard**: The classic Reinhard local tonemap operator is applied after multiplying by the exposure level.
* **ACESFilmic**: A tonemap operator consistent with the recommended curve for use by the [
Academy Color Encoding System](https://github.com/ampas/aces-dev).

The electro-optical transfer function is controlled by calling ``TransferFunction``:

* **Linear**: A simple linear transfer function is applied.
* **SRGB**: The approximate sRGB display curve (Gamma 2.2) is applied.
* **ST2084**: A Rec.709 to Rec.2020 color primary rotation and ST.2084 display curve is applied for HDR10 output.

> For Xbox One, there is also a ``SetMRTOutput`` boolean for selecting special shaders that generate both the HDR10 output for display as well as a tone-mapped GameDVR signal using Multiple Render Targets.

# Properties

* ``SetHDRSourceTexture`` is used to set the linear texture Shader Resource View that provides the original High Dynamic Range (HDR) signal, which is typically in a floating-point format.

* ``SetExposure`` sets the exposure value for the tonemap operators. The default is 0.0 (which is a linear multiplier of 1.0)

* ``SetST2084Parameter`` sets the nits level for paper white when applying the ST.2084 display curve. Reference paper white is 80 nits for a cinema setup, but defaults to 200.0 which is good for typical CRT viewing conditions.

# Further reading

Reinhard et al. "Photographic Tone Reproduction for Digital Images" [PDF](http://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf)  
[Tone mapping](https://en.wikipedia.org/wiki/Tone_mapping)  
[High-dynamic-range video](https://en.wikipedia.org/wiki/High-dynamic-range_video)  
