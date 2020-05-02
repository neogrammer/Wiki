[![MakeSpriteFont.exe](https://img.shields.io/github/downloads/Microsoft/DirectXTK/latest/MakeSpriteFont.exe?style=for-the-badge)](https://github.com/microsoft/DirectXTK/releases/latest/download/MakeSpriteFont.exe)

The MakeSpriteFont tool can process any TrueType font that is installed on your system (using GDI+ to rasterize them into a bitmap) or it can import character glyphs from a specially formatted bitmap file. This latter option allows you to create multicolored fonts, drawing special effects such as gradients or drop shadows directly into your glyph textures.

See [[SpriteFont]]

**Note:** The command-line tools in the DirectXTK package are only built by the _DirectXTK_Desktop_201x_ solutions since they are Win32 desktop applications.

**Related tutorial:** [[Drawing text]]

# TrueType
Open a [Command Prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq), and change to the directory containing ``MakeSpriteFont.exe`` (i.e. ``...\DirectXTK\MakeSpriteFont\bin\Release``)

Enter the following command-line after changing to the appropriate directory:

    MakeSpriteFont "Comic Sans MS" myfile.spritefont /FontSize:16

The file _myfile.spritefont_ is generated from the installed TrueType font.

You can also use the [Bitmap Font Maker](https://github.com/SimonDarksideJ/XNAGameStudio/wiki/Bitmap-Font-Maker) tool to capture a TrueType font as a BMP, then edit the image before using it to create a spritefont via the _Bitmap import_ procedure below

# Bitmap Import
For importing the font from a bitmap file, characters should be arranged in a grid ordered from top left to bottom right. Monochrome fonts should use white for solid areas and black for transparent areas. To include multicolored characters, add an alpha channel to the bitmap and use that to control which parts of each character are solid. The spaces between characters and around the edges of the grid should be filled with bright pink (red=255, green=0, blue=255). It doesn't matter if your grid includes lots of wasted space, because the converter will rearrange characters, packing everything as tightly as possible.

For example, this is the sprite font for the Xbox 360 Common Controller buttons.

![Xbox Controller Sprite Font](https://github.com/Microsoft/DirectXTK/wiki/images/xboxControllerSpriteFont.png)

Open a _Command Prompt_, and change to the directory containing ``MakeSpriteFont.exe`` (i.e. ``...\DirectXTK\MakeSpriteFont\bin\Release``)

Enter the following command-line after changing to the appropriate directory

    MakeSpriteFont xboxControllerSpriteFont.png xboxController.spritefont

The file _xboxController.spritefont_ is generated from the [xboxControllerSpriteFont.png](https://github.com/Microsoft/DirectXTK/wiki/images/xboxControllerSpriteFont.png) image.

**Note:** _The MakeSpriteFont tool only supports importing from ``.bmp``, ``.png``, or ``.gif`` images. If you are using a ``.tga`` source file, then you use should DirectXTex's **texconv** utility to convert it:_

```
texconv -ft PNG originalSpriteFont.tga
```

# Commandline options

**/CharacterRegion:_region_**
* Specifies which Unicode (UTF-16) codepoints to include in the font. Can be repeated to include more than one region. If not specified, the default ASCII range 32 (``<space>``) - 126 (``~``) is used.

Examples:
> /CharacterRegion:a-z

> /CharacterRegion:0x1200-0x1250

> /CharacterRegion:0x1234

**/DefaultCharacter:_value_**
* Fallback character substituted in place of codepoints that are not included in the font. If zero, missing characters throw exceptions.

**/FontSize:_value_**
* Size for TrueType fonts. Defaults to 23. Ignored when converting a bitmap font.

**/FontStyle:_value_**
* Style for TrueType fonts: ``Regular``, ``Bold``, ``Italic``, ``Strikeout``, or ``Underline``. Defaults to Regular. Ignored when converting a bitmap font.

**/LineSpacing:_value_**

**/CharacterSpacing:_value_**
* Spacing overrides. Zero is default spacing, negative closer together, positive further apart.

**/Sharp**
* Selects sharp antialiasing mode for TrueType rasterization. Otherwise a smoother looking (but more blurry) antialiasing mode is used.

**/TextureFormat:_value_**
* What format should the output texture be? Options:
 * ``Auto`` - The default. Chooses between CompressedMono and Rgba32 depending on whether the font data is monochromatic or multicolored.
 * ``Rgba32`` - High quality and supports multicolored fonts, but wastes space.
 * ``Bgra4444`` - Good choice for color fonts on the Univesal Windows Platform (UWP) or Xbox One, as this format requires the DirectX 11.1 Runtime and a WDDM 1.2 driver.
 * ``CompressedMono`` - The smallest format, and works on all D3D platforms, but it only supports monochromatic font data. This uses a special BC2 encoder: see comments in SpriteFontWriter.cs for details.

**/NoPremultiply**
* By default, font textures use premultiplied alpha format. Pass this flag if you want interpolative/straight alpha instead.

**/FeatureLevel:_value_**
* Provides the target feature level to control texture size warnings. Defaults to ``FL9_1``.

**/FastPack**
* By default, the individual glyphs are packed tightly in the resulting texture. This algorithm works well, but can be extremely slow when applied to large character sets. Pass this flag if you want to use a simplified grid-based packing which is much faster to compute for large character sets.

**/DebugOutputSpriteSheet:_filename_**
* Dumps the generated texture to a bitmap file (useful when debugging the MakeSpriteFont tool, not so much if you are just trying to use it).

# Localization
Since all glyphs specified are captured into a texture, the SpriteFont solution is very effective for smaller character sets. For large character sets such as Chinese, Japanese, or Korean, capturing every possible glyph is extremely slow, and the resulting texture is extremely large possibly exceeding the size supported by your target [Direct3D hardware feature level](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-devices-downlevel-intro). For these large character sets, using DirectWrite to render glyphs on-the-fly is a better solution.

For cases where DirectWrite is not supported (such as Windows phone 8.0 and Xbox One exclusive apps) and/or when rendering a set of static localized text, another solution is to scan all your translated text and capture only those character regions actually used by your application's specific display strings.

> If using MakeSpriteFont for these large fonts, you should try using the ``/FastPack`` option.

# Extended ASCII

If you are wanting to render an 'extended ASCII' string with SpriteFont, you need to capture the full set of characters which are not contiguous in Unicode.

    MakeSpriteFont "Consolas" /characterregion:0x0-0xFF
    /characterregion:0x192 /characterregion:0x393 /characterregion:0x398
    /characterregion:0x3A3 /characterregion:0x3A6 /characterregion:0x3A9
    /characterregion:0x3B1 /characterregion:0x3B4-0x3B5 /characterregion:0x3C0
    /characterregion:0x3C3-0x3C6 /characterregion:0x207F /characterregion:0x20A7
    /characterregion:0x2219 /characterregion:0x221A /characterregion:0x221E
    /characterregion:0x2229 /characterregion:0x2248 /characterregion:0x2261
    /characterregion:0x2264-0x2265 /characterregion:0x2310 /characterregion:0x2320
    /characterregion:0x2321 /characterregion:0x2500 /characterregion:0x2502
    /characterregion:0x250C /characterregion:0x2510 /characterregion:0x2514
    /characterregion:0x2518 /characterregion:0x251C /characterregion:0x2524
    /characterregion:0x252C /characterregion:0x2534 /characterregion:0x253C
    /characterregion:0x2550-0x256C /characterregion:0x2580 /characterregion:0x2584
    /characterregion:0x2588 /characterregion:0x258C /characterregion:0x2590-0x2593
    /characterregion:0x25A0 consolas.spritefont

[ASCII Extended PC](http://ascii-table.com/ascii-extended-pc-list.php)

# Feature Levels

The MakeSpriteFont tool will generate warnings if the resulting sprite sheet texture is too large for the texture sizes mandated for a known Direct3D feature level (i.e., 2048 x 2048 for 9.1 and 9.2; 4096 x 4096 for 9.3; 8192 x 8192 for 10.x; and 16384 x 16384 for 11.x).

# SpriteFont files

To aid in debugging, here is a [simple console program](https://raw.githubusercontent.com/wiki/Microsoft/DirectXTK/spritefontdump.cpp) for dumping out the content of a ``.spritefont`` header in a human-readable form along with a ``DDS`` file for the sprite sheet texture. It requires the [dds.h](https://raw.githubusercontent.com/Microsoft/DirectXTK/master/Src/dds.h) header in the DirectXTK library to build.

# Further reading

[Knapsack problem](http://en.wikipedia.org/wiki/Knapsack_problem)
