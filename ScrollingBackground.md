This is an example of a C++ port of the C# XNA Game Studio sample class for drawing a scrolling background using [[SpriteBatch]].

**Related tutorial**: [[More tricks with sprites]]

[ScrollingBackground.h](https://github.com/Microsoft/DirectXTK/wiki/ScrollingBackground.h)

```cpp
class ScrollingBackground
{
public:
    ScrollingBackground() :
        mScreenHeight(0),
        mTextureWidth(0),
        mTextureHeight(0),
        mScreenPos{},
        mTextureSize{},
        mOrigin{}
    {
    }

    void Load( ID3D11ShaderResourceView* texture )
    {
        mTexture = texture;

        if ( texture )
        {
            Microsoft::WRL::ComPtr<ID3D11Resource> resource;
            texture->GetResource( resource.GetAddressOf() );

            D3D11_RESOURCE_DIMENSION dim;
            resource->GetType( &dim );

            if ( dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D )
                throw std::exception( "ScrollingBackground expects a Texture2D" );

            Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
            resource.As( &tex2D );

            D3D11_TEXTURE2D_DESC desc;
            tex2D->GetDesc( &desc );

            mTextureWidth = desc.Width;
            mTextureHeight = desc.Height;

            mTextureSize.x = 0.f;
            mTextureSize.y = float( desc.Height );

            mOrigin.x = desc.Width / 2.f;
            mOrigin.y = 0.f;
        }
    }

    void SetWindow( int screenWidth, int screenHeight )
    {
        mScreenHeight = screenHeight;

        mScreenPos.x = float( screenWidth ) / 2.f;
        mScreenPos.y = float( screenHeight ) / 2.f;
    }

    void Update( float deltaY )
    {
        mScreenPos.y += deltaY;
        mScreenPos.y = fmodf( mScreenPos.y, float(mTextureHeight) );
    }

    void Draw( DirectX::SpriteBatch* batch ) const
    {
        using namespace DirectX;

        XMVECTOR screenPos = XMLoadFloat2( &mScreenPos );
        XMVECTOR origin = XMLoadFloat2( &mOrigin );

        if ( mScreenPos.y < mScreenHeight )
        {
            batch->Draw( mTexture.Get(), screenPos, nullptr,
                         Colors::White, 0.f, origin, g_XMOne,
                         SpriteEffects_None, 0.f );
        }

        XMVECTOR textureSize = XMLoadFloat2( &mTextureSize );

        batch->Draw( mTexture.Get(), screenPos - textureSize, nullptr,
                     Colors::White, 0.f, origin, g_XMOne,
                     SpriteEffects_None, 0.f );

        if ( mTextureHeight < mScreenHeight )
        {
            batch->Draw( mTexture.Get(), screenPos + textureSize, nullptr,
                         Colors::White, 0.f, origin, g_XMOne,
                         SpriteEffects_None, 0.f );
        }
    }

private:
    int                                                 mScreenHeight;
    int                                                 mTextureWidth;
    int                                                 mTextureHeight;
    DirectX::XMFLOAT2                                   mScreenPos;
    DirectX::XMFLOAT2                                   mTextureSize;
    DirectX::XMFLOAT2                                   mOrigin;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    mTexture;
};
```

# Example

Displays a top-to-bottom scrolling background of a starfield ([starfield.dds](https://github.com/Microsoft/DirectXTK/wiki/media/starfield.dds))

![Starfield](https://github.com/Microsoft/DirectXTK/wiki/images/starfield.png)

```cpp
#include "DDSTextureLoader.h"
#include "SpriteBatch.h"
#include "ScrollingBackground.h"

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> space;
std::unique_ptr<DirectX::SpriteBatch> batch;
std::unique_ptr<ScrollingBackground> background;

// Create a texture for the background
hr = CreateDDSTextureFromFile( device, L"starfield.dds", nullptr,
    space.GetAddressOf() );
DX::ThrowIfFailed(hr);

...

// Create a sprite batch for drawing
batch = std::make_unique<SpriteBatch>( context );

// Create a ScrollingBackground helper class instance, and set it to our texture
background = std::make_unique<ScrollingBackground>();
background->Load( space.Get() );

...

// Inform the scroller the size of our rendering window
background->SetWindow( width, height );

..

// Update the position of the background based on current time
background->Update( timeDelta * 100 );

...

// Render the background using sprite batch
batch->Begin();
background->Draw( batch.get() );

// TODO - More sprites or text here
batch->End();
```
