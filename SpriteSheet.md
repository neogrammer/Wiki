This is an example of a C++ port of the C# MonoGame sprite loader and renderer for using sprite sheets created by the CodeAndWeb tool [TexturePacker](https://www.codeandweb.com/texturepacker/monogame) using the built-in MonoGame project type.  This makes use of [[SpriteBatch]] to render.

> You can change this code to work with other sprite sheet creation tools by modifying the metadata reading code in Load.

[SpriteSheet.h](https://github.com/Microsoft/DirectXTK/wiki/SpriteSheet.h)

```cpp
class SpriteSheet
{
public:
    struct SpriteFrame
    {
        RECT                sourceRect;
        DirectX::XMFLOAT2   size;
        DirectX::XMFLOAT2   origin;
        bool                rotated;
    };

    void Load( ID3D11ShaderResourceView* texture, const wchar_t* szFileName )
    {
        mSprites.clear();

        mTexture = texture;

        if (szFileName)
        {
            //
            // This code parses the 'MonoGame' project txt file that is produced
            // by CodeAndWeb's TexturePacker.
            // https://www.codeandweb.com/texturepacker
            //
            // You can modify it to match whatever sprite-sheet tool you are
            // using
            //

            std::wifstream inFile( szFileName );
            if( !inFile )
                throw std::exception( "SpriteSheet failed to load .txt data" );

            wchar_t strLine[1024];
            for(;;)
            {
                inFile >> strLine;
                if( !inFile )
                    break;

                if( 0 == wcscmp( strLine, L"#" ) )
                {
                    // Comment
                }
                else
                {
                    static const wchar_t* delim = L";\n";

                    wchar_t* context = nullptr;
                    wchar_t* name = wcstok_s(strLine, delim, &context);
                    if ( !name || !*name )
                        throw std::exception();

                    if ( mSprites.find( name ) != mSprites.cend() )
                        throw std::exception();

                    wchar_t* str = wcstok_s(nullptr, delim, &context);
                    if ( !str )
                        throw std::exception();

                    SpriteFrame frame;
                    frame.rotated = (_wtoi(str) == 1);

                    str = wcstok_s(nullptr, delim, &context);
                    if ( !str )
                        throw std::exception();
                    frame.sourceRect.left = _wtol(str);

                    str = wcstok_s(nullptr, delim, &context);
                    if ( !str )
                        throw std::exception();
                    frame.sourceRect.top = _wtol(str);

                    str = wcstok_s(nullptr, delim, &context);
                    if ( !str )
                        throw std::exception();
                    LONG dx = _wtol(str);
                    frame.sourceRect.right = frame.sourceRect.left + dx;

                    str = wcstok_s(nullptr, delim, &context);
                    if ( !str )
                        throw std::exception();
                    LONG dy = + _wtol(str);
                    frame.sourceRect.bottom = frame.sourceRect.top + dy;

                    str = wcstok_s(nullptr, delim, &context);
                    if ( !str )
                        throw std::exception();
                    frame.size.x = static_cast<float>( _wtof(str) );

                    str = wcstok_s(nullptr, delim, &context);
                    if ( !str )
                        throw std::exception();
                    frame.size.y = static_cast<float>( _wtof(str) );

                    str = wcstok_s(nullptr, delim, &context);
                    if ( !str )
                        throw std::exception();
                    float pivotX = static_cast<float>( _wtof(str) );

                    str = wcstok_s(nullptr, delim, &context);
                    if ( !str )
                        throw std::exception();
                    float pivotY = static_cast<float>( _wtof(str) );

                    if (frame.rotated)
                    {
                        frame.origin.x = dx * (1.f - pivotY);
                        frame.origin.y = dy * pivotX;
                    }
                    else
                    {
                        frame.origin.x = dx * pivotX;
                        frame.origin.y = dy * pivotY;
                    }

                    mSprites.insert( std::pair<std::wstring,SpriteFrame>(
                        std::wstring(name), frame) );
                }

                inFile.ignore( 1000, '\n' );
            }
        }
    }

    const SpriteFrame* Find(const wchar_t* name) const
    {
        auto it = mSprites.find(name);
        if (it == mSprites.cend())
            return nullptr;

        return &it->second;
    }

    void XM_CALLCONV Draw(DirectX::SpriteBatch* batch, const SpriteFrame& frame,
        DirectX::XMFLOAT2 const& position,
        DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0,
        float scale = 1,
        DirectX::SpriteEffects effects = DirectX::SpriteEffects_None,
        float layerDepth = 0) const
    {
        assert(batch != 0);
        using namespace DirectX;

        if (frame.rotated)
        {
            rotation -= XM_PIDIV2;
            switch(effects)
            {
            case SpriteEffects_FlipHorizontally:
                effects = SpriteEffects_FlipVertically;
                break;
            case SpriteEffects_FlipVertically:
                effects = SpriteEffects_FlipHorizontally;
                break;
            }
        }

        XMFLOAT2 origin = frame.origin;
        switch (effects)
        {
        case SpriteEffects_FlipHorizontally:
                origin.x = frame.sourceRect.right
                    - frame.sourceRect.left - origin.x;
                break;
        case SpriteEffects_FlipVertically:
                origin.y = frame.sourceRect.bottom
                    - frame.sourceRect.top - origin.y;
                break;
        }

        batch->Draw(mTexture.Get(), position, &frame.sourceRect,
                color, rotation,
                origin, scale, effects, layerDepth );
    }

    void XM_CALLCONV Draw(DirectX::SpriteBatch* batch, const SpriteFrame& frame,
        DirectX::XMFLOAT2 const& position,
        DirectX::FXMVECTOR color, float rotation, DirectX::XMFLOAT2 const& scale,
        DirectX::SpriteEffects effects = DirectX::SpriteEffects_None,
        float layerDepth = 0) const
    {
        ...
    }

    // Draw overloads specifying position and scale via the first two components of
    // an XMVECTOR.
    void XM_CALLCONV Draw(DirectX::SpriteBatch* batch, const SpriteFrame& frame,
        DirectX::FXMVECTOR position,
        DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0,
        float scale = 1,
        DirectX::SpriteEffects effects = DirectX::SpriteEffects_None,
        float layerDepth = 0) const
    {
        ...
    }

    void XM_CALLCONV Draw(DirectX::SpriteBatch* batch, const SpriteFrame& frame,
        DirectX::FXMVECTOR position,
        DirectX::FXMVECTOR color, float rotation, DirectX::GXMVECTOR scale,
        DirectX::SpriteEffects effects = DirectX::SpriteEffects_None,
        float layerDepth = 0) const
    {
        ...
    }

    // Draw overloads specifying position as a RECT.
    void XM_CALLCONV Draw(DirectX::SpriteBatch* batch, const SpriteFrame& frame,
        RECT const& destinationRectangle,
        DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0,
        DirectX::SpriteEffects effects = DirectX::SpriteEffects_None,
        float layerDepth = 0) const
    {
        ...
    }

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    mTexture;
    std::map<std::wstring, SpriteFrame>                 mSprites;
};
```

# Example

This example uses a sprite sheet created from the content in the original C# XNA Game Studio [SpriteSheetSample](http://xbox.create.msdn.com/en-US/education/catalog/sample/sprite_sheet).

The resulting sprite sheet is [SpriteSheetSample.png](https://github.com/Microsoft/DirectXTK/wiki/images/SpriteSheetSample.png)

![Sprite Sheet](https://github.com/Microsoft/DirectXTK/wiki/images/SpriteSheetSample.png)

and the resulting metadata [SpriteSheetSample.txt](https://github.com/Microsoft/DirectXTK/wiki/SpriteSheetSample.txt)

    #
    # Sprite sheet data for MonoGame.
    #
    # To learn how to import these sprites into your MonoGame project visit:
    # http://www.codeandweb.com/texturepacker/monogame
    #
    # Sprite sheet: SpriteSheetSample.png (204 x 200)
    # $TexturePacker:SmartUpdate:0368b25d54aeeee939136b8605cf8eb5:68e5ed36fde793856b1138fb021ac1d9:f18d998871cf5eafdd9feca9b014994a$
    #

    cat;0;2;2;66;100;100;100;0.5151515151515151;0.5
    glow1;0;2;104;64;64;64;64;0.5;0.5
    glow2;0;70;2;64;64;64;64;0.5;0.5
    glow3;0;136;2;64;64;64;64;0.5;0.5
    glow4;0;70;68;64;64;64;64;0.5;0.5
    glow5;0;68;134;64;64;64;64;0.5;0.5
    glow6;0;134;134;64;64;64;64;0.5;0.5
    glow7;0;136;68;64;64;64;64;0.5;0.5

And here is some example code to render using this sprite sheet:

```cpp
#include "SpriteBatch.h"
#include "SpriteSheet.h"
#include "WICTextureLoader.h"

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
std::unique_ptr<SpriteSheet> sprites;

// Create a texture using our sprite sheet
hr = CreateWICTextureFromFile( device, L"SpriteSheetSample.png",
    nullptr, texture.GetAddressOf() );
DX::ThrowIfFailed(hr);

...

// Create a SpriteBatch for rendering
spriteBatch = std::make_unique<SpriteBatch>( context );

// Create an SpriteSheet helper class instance
sprites = std::make_unique<SpriteSheet>();
sprites->Load( texture.Get(), L"SpriteSheetSample.txt" );

...

// Render a sprite from the sheet
spriteBatch->Begin();

auto frame = sprites->Find( L"cat" );
assert( frame != 0 );

sprites->Draw( spriteBatch.get(), *frame, screenPos );

// TODO - More sprites or text here
spriteBatch->End();
```

# TexturePacker notes

If you are making use of CodeAndWeb's **TexturePacker** tool, you will be writing out the sprite sheet texture as a PNG which you will be using at runtime along with the 'data file' .txt that is defined as part of the "MonoGame" project. _Note that you will not be making use of the '.cs' file TexturePacker generates as part of the "MonoGame" project type._

If you are using premultiplied alpha for your blending (the default for SpriteBatch), you should under _Texture \ show advanced_ set the _Premultiply alpha_ check box option.

The default _Pixel format_ of RGBA8888 is suitable for all feature levels, and will be loaded at runtime as ``DXGI_FORMAT_B8G8R8A8_UNORM``. Due to limitations of the [built-in Windows PNG codec](https://docs.microsoft.com/en-us/windows/desktop/wic/-wic-codec-native-pixel-formats#png__native_codec), using other pixel formats does not reduce video memory usage; only disk space usage. For more flexible control over pixel format at runtime, you should convert to a ``.DDS`` instead (see below).

The default _Max size_ of 2048 x 2048 under _Layout_ is suitable for all feature levels. If your _minimum_ supported feature level is not 9.1 or 9.2, you can set it higher to make use of larger supported texture sizes for more efficient packing.
* Feature Level 9.1, 9.2 -> 2048 x 2048
* Feature Level 9.3 -> 4096 x 4096
* Feature Level 10.0 or 10.1 -> 8192 x 8192
* Feature Level 11.0 or 11.1 -> 16384 x 16384

## DDS

You can optionally convert the ``PNG`` to a ``DDS`` using DirectXTex's [texconv](https://github.com/Microsoft/DirectXTex/wiki/Texconv) tool or the Visual Studio texture content processor, ideally using BC2 or BC3 for runtime compression. This will result in a larger ``DDS`` file on disk than a ``PNG``, but will use less video memory when loaded. You would use [[DDSTextureLoader]] instead of [[WICTextureLoader]] in this case.

If using ``DDS`` rather than ``PNG`` and premultiplied alpha for your blending, you should leave the **TexturePacker** _Premultiply alpha_ option unchecked and add the *-pmalpha* option to DirectXTex's **texconv** to get the premultipled alpha conversion as part of creating the ``DDS``.

In most cases you won't need mipmap levels generated so specify *-m 1* when converting the ``DDS``. If you do want mipmaps levels and want to support all feature levels (i.e. Feature Levesl 9.x), then in your **TexturePacker** project under _Layout / Size constraints_ set it to "POW (Power of 2)" and do not use *-m 1*.

Open a [Command Prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq), and change to the directory containing Texconv.exe (i.e. ``...\DirectXTex\Texconv\Release``)

Enter the following command-line after changing to the appropriate directory:

    texconv.exe SpriteSheetSample.png -f BC3_UNORM -m 1 -pmalpha

# Further Development

The more sprites that can be packed into a single texture, the more efficient the draw operations can be. ``SpriteBatch`` only needs to flush when the source texture is changed, so as long as you draw from the same sprite sheet the performance is very good.

You can extend this further on Direct3D hardware feature level 10.0 or better by packing multiple sprite sheets into a single 2D texture array, but this would require modifying ``SpriteBatch`` and the ``SpriteBatch.fx`` shaders to use a per-vertex texture array index.
