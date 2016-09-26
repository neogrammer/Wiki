In this lesson, we will explore more things you can do with sprites.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Animating sprites
In addition to moving the sprite's position, sprites themselves can be made to move using 'flip-book' style animation.

First, start by saving the file [AnimatedTexture.h](https://github.com/Microsoft/DirectXTK/wiki/AnimatedTexture.h) to your new project's directory--see [[AnimatedTexture]] for more information. Then go to the top menu and select **Project** / **Add Existing Item...**. Select "AnimatedTexture.h" and click "OK".

Next save [shipanimated.png](https://raw.githubusercontent.com/wiki/Microsoft/DirectXTK/images/shipanimated.png) into your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "shipanimated.png" and click "OK".

Add to the top of **Game.h** the following after the other ``#include`` statements:

    #include "AnimatedTexture.h"

Then add the following variables to the bottom of the Game class's private declarations:

    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    std::unique_ptr<AnimatedTexture> m_ship;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
    DirectX::SimpleMath::Vector2 m_shipPos;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"shipanimated.png",
        nullptr, m_texture.ReleaseAndGetAddressOf()));

    m_ship = std::make_unique<AnimatedTexture>();
    m_ship->Load(m_texture.Get(), 4, 20);

In **Game.cpp**, add to the TODO of **CreateResources**:

    m_shipPos.x = float(backBufferWidth / 2);
    m_shipPos.y = float((backBufferHeight / 2) + (backBufferHeight / 4));

> If using the UWP template, you also need to add ``m_spriteBatch->SetRotation(m_outputRotation);`` to handle display orientation changes.

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_ship.reset();
    m_spriteBatch.reset();
    m_texture.Reset();

In **Game.cpp**, add to the TODO of **Render**:

    m_spriteBatch->Begin();

    m_ship->Draw(m_spriteBatch.get(), m_shipPos );
    
    m_spriteBatch->End();

In **Game.cpp**, add to the TODO of **Update**:

    m_ship->Update(elapsedTime);

Build and run to see our ship at the bottom with animated thrust.

![Screenshot of Ship](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotShip1.png)

# Creating a scrolling background

What really 'sells the shot' for a side-scroller style game is the moving background to give the ship a sense of movement.

First, start saving the file [ScrollingBackground.h](https://github.com/Microsoft/DirectXTK/wiki/ScrollingBackground.h) to your new project's directory--see [[ScrollingBackground]] for more information. Then go to the top menu and select **Project** / **Add Existing Item...**. Select "ScrollingBackground.h" and click "OK".

Next save [starfield.png](https://github.com/Microsoft/DirectXTK/wiki/images/starfield.png) into your new project's directory, and then from the top menu select **Project** / **Add Existing Item...**. Select "starfield.png" and click "OK".

Add to the top of **Game.h** the following after the other ``#include`` statements:

    #include "ScrollingBackground.h"

Then add the following variables to the bottom of the Game class's private declarations:

    std::unique_ptr<ScrollingBackground> m_stars;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backgroundTex;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"starfield.png",
        nullptr, m_backgroundTex.ReleaseAndGetAddressOf()));

    m_stars = std::make_unique<ScrollingBackground>();
    m_stars->Load(m_backgroundTex.Get());

In **Game.cpp**, add to the TODO of **CreateResources**:

    m_stars->SetWindow(backBufferWidth, backBufferHeight);

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_stars.reset();
    m_backgroundTex.Reset();

In **Game.cpp**, modify the TODO of **Render**:

    m_spriteBatch->Begin();

    m_stars->Draw(m_spriteBatch.get());
    m_ship->Draw(m_spriteBatch.get(), m_shipPos );
    
    m_spriteBatch->End();

In **Game.cpp**, add to the TODO of **Update**:

    m_stars->Update(elapsedTime * 500 );

Build and run to see our ship in space!

![Screenshot of Ship in Space](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotShip2.png)

# More to explore

These lessons on SpriteBatch only begin to show the many techniques you can implement. Here are some other ideas to consider:

* SpriteBatch can perform sorting of the sprites in a number of different modes. We've used the default *sortMode* of ``SpriteSortMode_Deferred`` for ``Begin``. If drawing with lots of different textures, you should try ``SpriteSortMode_Texture``. If you are making use of the *layerDepth* parameter to sort your sprites correctly, use ``SpriteSortMode_BackToFront`` or ``SpriteSortMode_FrontToBack``. For all these modes, the actual drawing does not happen until you call ``End``--or you exceed the internal vertex buffer size. If you use ``SpriteSortMode_Immediate``, then the sprite is drawn as soon as you call ``Draw``.

* SpriteBatch can 'flip' the texture image at runtime using ``SpriteEffects_FlipHorizontally``, ``SpriteEffects_FlipVertically``, or ``SpriteEffects_FlipBoth``. The default *effects* parameter for ``Draw`` is ``SpriteEffects_None``.

* When drawing lots of different sprites, having to load hundreds of individual texture files is inefficient and potentially wastes video memory.  One solution is to pack the sprite images into a single (or a few) textures and then render parts of them using *sourceRectangle* for ``Draw``. For more on implementing this technique, see [[SpriteSheet]].

* The SpriteBatch class has a 'rotation mode' setting which can be used to handle device orientation changes for Windows Store apps and Windows phone apps, or just for special effects. You call **SetRotation** with one of the ``DXGI_MODE_ROTATION`` settings which controls the final view transformation to flip the output an extra 90 degrees, 180 degrees, or 270 degrees. It defaults to no extra rotation with ``DXGI_MODE_ROTATION_IDENTITY``.

* With the *setCustomShaders* parameter to Begin you can override the shader settings and other Direct3D state to implement things like post-processing and normal-map based effects with SpriteBatch.

* The *transformationMatrix* parameter to ``Begin`` lets you provide a custom transformation for the sprite renderering. This is combined with the viewport-based final view transform, but if you use ``SetRotation( DXGI_MODE_ROTATION_UNSPECIFIED )`` this will be disabled. The default value for this parameter is the _identity matrix_.

**Next lesson:** [[Drawing text]]

# Further reading

DirectX Tool Kit docs - [[SpriteBatch]]
