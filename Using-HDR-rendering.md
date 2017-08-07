This lesson covers the basics of HDR rendering, tone-mapping, and adding HDR10 wide color gamut rendering with _DirectX Tool Kit_.

# Setup
First create a new project. For this lesson, use the [[DeviceResources]] variant of the [[The basic game loop]], then use the instructions in [[Adding the DirectX Tool Kit]].

# Setting up an HDR render target

Save the files [RenderTexture.h](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.h) and [RenderTexture.cpp](https://github.com/Microsoft/DirectXTK/wiki/RenderTexture.cpp) to your new project's folder. Using to the top menu and select **Project** / **Add Existing Item....** Select "RenderTexture.h" and hit "OK". Repeat for "RenderTexture.cpp".

Add to the **Game.h** file to the ``#include`` section:

    #include "RenderTexture.h"

In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

    std::unique_ptr<DX::RenderTexture> m_hdrScene;
    std::unique_ptr<DirectX::ToneMapPostProcess> m_toneMap;

*UNDER DEVELOPMENT*

# Rendering a test scene

*UNDER DEVELOPMENT*

# Adding tone mapping

*UNDER DEVELOPMENT*

# Using HDR10 display output

*UNDER DEVELOPMENT*

**Next lessons:** [[Game controller input]], [[Using the SimpleMath library]], [[Adding the DirectX Tool Kit for Audio]]

# Further reading 

DirectX Tool Kit docs [[PostProcess]] and [[ToneMapPostProcess]]

