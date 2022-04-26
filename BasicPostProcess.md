|[[DirectXTK]]|[[PostProcess]]|
|---|---|

Performs basic post-processing with a built-in set of shaders which operate using one input texture.

# Header
```cpp
#include "PostProcess.h"
```

# Initialization
Construction requires a Direct3D 11 device.

```cpp
std::unique_ptr<BasicPostProcess> postProcess;
postProcess = std::make_unique<BasicPostProcess>(device);
```

For exception safety, it is recommended you make use of the C++ [RAII](http://wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Shader selection

BasicPostProcess can be configured to use various pixel shaders by calling ``SetEffect``:

* **Copy**: Performs a simple read of the input texture, outputting the same color.
* **Monochrome**: Converts the input texture pixels to a luminance value.
* **Sepia**: Implements a sepia-tone effect.
* **DownScale_2x2**: Downscales each 2x2 block of pixels to an average. This is intended to write to a render target that is half the size of the source texture in each dimension.
* **DownScale_4x4**: Downscales each 4x4 block of pixels to an average. This is intended to write to a render target that is a quarter the size of the source texture in each dimension.
* **GaussianBlur_5x5**: Performs a Gaussian blur with a 5x5 kernel
* **BloomExtract**: Performs a bloom extract, which is the first pass in implementing a bloom effect
* **BloomBlur**: Performs a horizontal or vertical blur which is typically the second and third passes of implementing a bloom effect.

# Properties

* ``SetSourceTexture`` is used to set the input texture as a Shader Resource View.

* ``SetGaussianParameter`` sets the multiplier for the *GaussianBlur_5x5* shader.

* ``SetBloomExtractParameter`` sets the extract parameter for the *BloomExtract* shader.

* ``SetBloomBlurParameters`` sets the horizontal vs. vertical mode as well as the size and brightness factors for the *BloomBlur* shader.

# Example

If drawing a single texture which fills the render viewport, then using the **Copy** effect above is faster than using [[SpriteBatch]]--assuming you have Direct3D Hardware Feature Level 10 or later.

```cpp
std::unique_ptr<BasicPostProcess> postProcess;
postProcess = std::make_unique<BasicPostProcess>(device);

...
postProcess->SetSourceTexture(m_texture.Get());
postProcess->SetEffect(BasicPostProcess::Copy);

// Set RSSetViewports to the rectangle you want to fill with the texture.

// If the viewport is not the full render target, be sure to call RSSetScissors as well.

postProcess->Process(context);
```

# Remarks

This class in **Copy** mode can be used for GPU-based format conversion by using a render-to-texture render-target of the desired target format. This can also be used to achieve GPU-based texture resizing as well. The main limitations are that the source format must be supported as a 2D texture for your Direct3D Hardware Feature Level, and the target format must be supported as a render-target. Since this class already requires Direct3D Hardware Level 10.0, there is always a fairly robust list of supported format combinations. See [Microsoft Docs](https://docs.microsoft.com/windows/win32/direct3ddxgi/format-support-for-direct3d-feature-level-10-0-hardware).


