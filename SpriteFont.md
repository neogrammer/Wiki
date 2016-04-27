This is a native Direct3D implementation of a bitmap font renderer, similar to the SpriteFont type from [XNA Game Studio](https://msdn.microsoft.com/en-us/library/microsoft.xna.framework.graphics.spritefont.aspx), plus a command line tool ([[MakeSpriteFont]]) for building fonts into bitmap format. It is less fully featured than Direct2D and DirectWrite, but may be useful for those who want something simpler and lighter weight.

> SpriteFont is particularly useful for the Windows phone 8.0 and Xbox One XDK platforms that lack support for Direct2D and DirectWrite.

![Sprite Font Example](https://github.com/Microsoft/DirectXTK/wiki/images/SpriteFontExample.png)

**Related tutorial:** [[Drawing text]]

# Header
    #include <SpriteFont.h>

# Initialization

The SpriteFont class requires a [[SpriteBatch]] instance and a ``.spritefont`` bitmap file.

    std::unique_ptr<SpriteBatch> spriteBatch;
    std::unique_ptr<SpriteFont> spriteFont;

    spriteBatch = std::make_unique<SpriteBatch>(deviceContext);
    spriteFont = std::make_unique<SpriteFont>(device, L"myfile.spritefont");

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

> You create one ``SpriteFont`` instance per font style & point-size you want to use to render with. You can use the same ``SpriteBatch`` or use several ``SpriteBatch`` instances if desired. Be sure to read the _Threading model_ notes for [[SpriteBatch]].

# Simple drawing

    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch.get(), L"Hello, world!", XMFLOAT2(x, y));
    spriteBatch->End();

The **DrawString** method has several overloads with parameters controlling color, rotation, origin point, scaling, horizontal or vertical mirroring, and layer depth. These work the same way as the equivalent ``SpriteBatch::Draw`` parameters.

# Constructors

SpriteFont has three constructors:

* Pass a filename string to read a binary file created by [[MakeSpriteFont]]
* Pass a buffer containing a MakeSpriteFont binary that was already loaded some other way
* Pass an array of Glyph structs if you prefer to entirely bypass MakeSpriteFont

# Helpers
In addition to ``DrawString`` with various overloads, SpriteFont includes the following helpers:

* **MeasureString** which returns the size of the given string in pixels.

_Note: The string size is computed from the origin to the rightmost pixel rendered by any character glyph. This has the effect of ignoring 'trailing spaces'._

* **ContainsCharacter** tests to see if a given character is defined in the font

* **FindGlyph** can be used to obtain size and other metadata for a character in the font. Note if the character is not defined in the font and there is no default character, this function will throw a C++ exception.

* **GetSpriteSheet** returns a reference to the texture used for the sprite font for custom rendering.

# Default character

If you try to ``DrawString`` or call ``MeasureString`` with a character that is not included in the font, by default you will get an exception. Use **SetDefaultCharacter** to specify some other character that will be automatically substituted in place of any that are missing. You can also use **GetDefaultCharacter** to obtain the current default which is also defined as part of the font.

# Special characters

SpriteFont will respect new line characters (``\n`` - ASCII character 10), and ignores carriage returns (``\r`` - ASCII character 13). The distance moved for a new line is defined in the font and can be accessed with **GetLineSpacing** / **SetLineSpacing**.

There is no special handling for the bell character (``\a`` - ASCII character 7), backspace (``\b`` - ASCII character 8), horizontal tab (``\t`` - ASCII character 9), vertical tab (ASCII character 11), form feed (``\f`` - ASCII character 12), or escape (ASCII character 27). These are all treated as standard characters and if it is missing from the ``.spritefont``, they will all render as the default character or generate an exception if there is no default character defined.

# Localization

This implementation supports sparse fonts, so if you are localizing into languages such as Chinese, Japanese, or Korean, you can build a ``.spritefont`` including only the specific characters needed by your program. This is usually a good idea for CJK languages, as a complete CJK character set is too large to fit in a Direct3D texture! If you need full CJK support, DrectWrite would be a better choice if available on your target platform. SpriteFont does not support combining characters or right-to-left (RTL) layout, so it will not work for languages with complex layout requirements such as Arabic or Thai.

> Note: This XNA Game Studio article [How to: Create a Localized Game](https://msdn.microsoft.com/en-us/library/ff966426.aspx) describes how to use the C# content pipeline to scan the game's strings and create a font from it. You have to use a different solution for DirectX Tool Kit but the idea is the same.

# ASCII
The default character region for [[MakeSpriteFont]] from 32 to 127 covers the standard 7-bit [ASCII](http://ascii-table.com/ascii.php) range. For example, here is a C++ Unicode string for the printable characters (this would be an ASCII string if you remove the L prefix).

    L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
    L"abcdefghijklmnopqrstuvwxyz{|}~"

# Extended ASCII

If you are wanting to render an [extended ASCII](http://ascii-table.com/ascii-extended-pc-list.php) string with SpriteFont, you need to capture the full set of characters which are not contiguous in Unicode (see [[MakeSpriteFont]] for details). You then need to convert your 'extended ASCII' string to Unicode using [Code page 437](http://en.wikipedia.org/wiki/Code_page_437) before calling ``DrawString``.

    char ascii[...];
    wchar_t unicode[...];
    if (!MultiByteToWideChar(437, MB_PRECOMPOSED,
        ascii, length-of-ascii-string,
        unicode, length-of-unicode-string))
    {
        // Error
    }
    spriteFont->DrawString(spriteBatch.get(), unicode, ...)

For example, here is a C++ Unicode string with the full extended ASCII IBM PC character set from 128 to 255:

    L"\x00C7\x00FC\x00E9\x00E2\x00E4\x00E0\x00E5\x00E7\x00EA
    \x00EB\x00E8\x00EF\x00EE\x00EC\x00C4\x00C5\x00C9\x00E6\x00C6\x00F4\x00F6\x00F2\x
    00FB\x00F9\x00FF\x00D6\x00DC\x00A2\x00A3\x00A5\x20A7\x0192\x00E1\x00ED\x00F3\x00
    FA\x00F1\x00D1\x00AA\x00BA\x00BF\x2310\x00AC\x00BD\x00BC\x00A1\x00AB\x00BB\x2591
    \x2592\x2593\x2502\x2524\x2561\x2562\x2556\x2555\x2563\x2551\x2557\x255D\x255C\x
    255B\x2510\x2514\x2534\x252C\x251C\x2500\x253C\x255E\x255F\x255A\x2554\x2569\x25
    66\x2560\x2550\x256C\x2567\x2568\x2564\x2565\x2559\x2558\x2552\x2553\x256B\x256A
    \x2518\x250C\x2588\x2584\x258C\x2590\x2580\x03B1\x00DF\x0393\x03C0\x03A3\x03C3\x
    00B5\x03C4\x03A6\x0398\x03A9\x03B4\x221E\x03C6\x03B5\x2229\x2261\x00B1\x2265\x22
    64\x2320\x2321\x00F7\x2248\x00B0\x2219\x00B7\x221A\x207F\x00B2\x25A0\x00A0"

_The Xbox One exclusive apps version of MultiByteToWideChar does not support code page 437._

# Applications

For code for rendering text in a classic-style scrolling console, see [[TextConsole]].

Code for rendering text mixed with the Xbox controller button spritefont, see [[ControllerFont]].

# Feature Level Notes

The Sprite Font implementation is compatible with all feature levels. The primary limitation is on the size of the sprite sheet texture which should fit into the limits for [known feature levels](https://msdn.microsoft.com/en-us/library/windows/desktop/ff476876.aspx) (i.e. to support all feature levels, it should be no larger than 2048 x 2048; if you target feature level 9.3 or later you can use 4096 x 4096, etc.).

> You are of course free to use a larger texture as long as you set your application's minimum supported feature level as appropriate. [[MakeSpriteFont]] will emit warnings if your captured font texture exceeds the various feature level size thresholds.

# Threading model

Creation is fully asynchronous, so you can instantiate multiple SpriteFont instances at the same time on different threads. Most SpriteFont methods are thread-safe with the exception of ``SetLineSpacing`` and ``SetDefaultCharacter``. Be sure to read the _Threading model_ notes for [[SpriteBatch]] as well.

# Further reading

[Bitmap fonts in XNA](http://blogs.msdn.com/b/shawnhar/archive/2007/04/26/bitmap-fonts-in-xna.aspx)  
[SpriteBatch billboards in a 3D world](http://blogs.msdn.com/b/shawnhar/archive/2011/01/12/spritebatch-billboards-in-a-3d-world.aspx)  
[Redistributable Font Pack](http://xbox.create.msdn.com/en-US/education/catalog/utility/font_pack)