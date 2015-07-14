This lesson covers drawing text using bitmap fonts and the sprite renderer.

# Setup
First create a new project using the instructions from the first two lessons: [[The basic game loop]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Creating a font

The text renderer in _DirectX Tool Kit_ makes use of bitmap fonts, so the first step is to create a ``.spritefont`` file by capturing a system TrueType font using the **makespritefont** command-line utility.

* Download the [MakeSpriteFont.exe](https://github.com/Microsoft/DirectXTK/releases/download/jul2015/MakeSpriteFont.exe) from the _DirectX Tool Kit_ site save the EXE into your project's folder.
* Open a [command-prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq) and then change to your project's folder.

Run the following command-line

    MakeSpriteFont "Courier New" myfile.spritefont /FontSize:32

Then from the top menu in Visual Studio select **Project** / **Add Existing Item...**. Select [myfile.spritefont](https://github.com/Microsoft/DirectXTK/wiki/myfile.spritefont) and click "OK".

> If you are using a universal Windows app, Windows Store, or Xbox One project rather than a Windows desktop app, you need to manually edit the Visual Studio project properties on the ``myfile.spritefont`` file and make sure "Content" is set to "Yes" so the data file will be included in your packaged build.

# Loading a bitmap font
In the **Game.h** file, add the following variable to the bottom of the Game class's private declarations:

    std::unique_ptr<DirectX::SpriteFont> m_font;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    m_font.reset(new SpriteFont(m_d3dDevice.Get(), L"myfile.spritefont"));

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_font.reset();

Build and run the application which will still not be displaying anything but the cornflower blue window, but will have a font loaded.

> _Troubleshooting:_ If you get a runtime exception, then you may have the ``myfile.spritefont`` in the wrong folder, have modified the "Working Directory" in the "Debugging" configuration settings, or otherwise changed the expected paths at runtime of the application. You should set a break-point on ``new SpriteFont`` and step into the code to find the exact problem.

# Drawing text using a font

In the **Game.h** file, add the following variables to the bottom of the Game class's private declarations:

    DirectX::SimpleMath::Vector2 m_fontPos;
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

In **Game.cpp**, add to the TODO of **CreateDevice**:

    m_spriteBatch.reset(new SpriteBatch(m_d3dContext.Get()));

In **Game.cpp**, add to the TODO of **CreateResources**:

    m_fontPos.x = backBufferWidth / 2.f;
    m_fontPos.y = backBufferHeight / 2.f;

In **Game.cpp**, add to the TODO of **OnDeviceLost**:

    m_spriteBatch.reset();

In **Game.cpp**, modify the TODO section of **Render** to be:

    m_spriteBatch->Begin();

    const wchar_t* output = L"Hello World";

    Vector2 origin = m_font->MeasureString(output) / 2.f;

    m_font->DrawString(m_spriteBatch.get(), output,
        m_fontPos, Colors::White, 0.f, origin);

    m_spriteBatch->End();

Build and run to see our text string centered in the middle of the rendering window:

![Screenshot Hello World](https://github.com/Microsoft/DirectXTK/wiki/images/screenshotHelloWorld.PNG)

# Using ``std::wstring`` for text

In **Game.cpp**, modify the TODO section of **Render** to be:

    std::wstring output = std::wstring(L"Hello") + std::wstring(L" World")

    m_spriteBatch->Begin();

    Vector2 origin = m_font->MeasureString(output.c_str()) / 2.f;

    m_font->DrawString(m_spriteBatch.get(), output.c_str(),
        m_fontPos, Colors::White, 0.f, origin);

    m_spriteBatch->End();

Build and run to see our text string centered in the middle of the rendering window.

# Using ASCII text

In **pch.h** after the other ``#include`` statements, add:

    #include <locale>
    #include <codecvt>

In **Game.cpp**, modify the TODO section of **Render** to be:

    const char *ascii = "Hello World";
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring output = converter.from_bytes(ascii);

    m_spriteBatch->Begin();

    Vector2 origin = m_font->MeasureString(output.c_str()) / 2.f;

    m_font->DrawString(m_spriteBatch.get(), output.c_str(),
        m_fontPos, Colors::White, 0.f, origin);

    m_spriteBatch->End();

Build and run to see our text string centered in the middle of the rendering window.

**Next lesson:** [[Simple rendering]]

# Further reading

DirectX Tool Kit docs [[SpriteBatch]], [[SpriteFont]], [[MakeSpriteFont]]
