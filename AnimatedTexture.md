This is an example of a C++ port of the C# [XNA Game Studio sample](http://msdn.microsoft.com/en-us/library/bb203866.aspx) class for drawing an animated 2D sprite using [[SpriteBatch]]. The sprite animation is assumed to be laid out horizontally.

[AnimatedTexture.h](https://github.com/Microsoft/DirectXTK/wiki/AnimatedTexture.h)

    class AnimatedTexture
    {
    public:
        AnimatedTexture()
        {
            ...
        }

        AnimatedTexture( const DirectX::XMFLOAT2& origin, float rotation, float scale,
            float depth )
        {
            ...
        }

        void Load( ID3D11ShaderResourceView* texture, int frameCount, int framesPerSecond )
        {
            if ( frameCount < 0 || framesPerSecond <= 0 )
                throw std::invalid_argument( "AnimatedTexture" );

            mPaused = false;
            mFrameCount = frameCount;
            mTimePerFrame = 1.f / float(framesPerSecond);
            mTotalElapsed = 0.f;
            mTexture = texture;

            if ( texture )
            {
                Microsoft::WRL::ComPtr<ID3D11Resource> resource;
                texture->GetResource( resource.GetAddressOf() );

                D3D11_RESOURCE_DIMENSION dim;
                resource->GetType( &dim );

                if ( dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D )
                    throw std::exception( "AnimatedTexture expects a Texture2D" );

                Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
                resource.As( &tex2D );

                D3D11_TEXTURE2D_DESC desc;
                tex2D->GetDesc( &desc );

                mTextureWidth = int( desc.Width );
                mTextureHeight = int( desc.Height );
            }
        }

        void Update( float elapsed )
        {
            if ( mPaused )
                return;

            mTotalElapsed += elapsed;

            if ( mTotalElapsed > mTimePerFrame )
            {
                ++mFrame;
                mFrame = mFrame % mFrameCount;
                mTotalElapsed -= mTimePerFrame;
            }
        }

        void Draw( DirectX::SpriteBatch* batch, const DirectX::XMFLOAT2& screenPos ) const
        {
            Draw( batch, mFrame, screenPos );
        }

        void Draw( DirectX::SpriteBatch* batch, int frame,
            const DirectX::XMFLOAT2& screenPos ) const
        {
            int frameWidth = mTextureWidth / mFrameCount;

            RECT sourceRect;
            sourceRect.left = frameWidth * frame;
            sourceRect.top = 0;
            sourceRect.right = sourceRect.left + frameWidth;
            sourceRect.bottom = mTextureHeight;

            batch->Draw( mTexture.Get(), screenPos, &sourceRect, DirectX::Colors::White,
                         mRotation, mOrigin, mScale, DirectX::SpriteEffects_None, mDepth );
        }

        void Reset()
        {
            mFrame = 0;
            mTotalElapsed = 0.f;
        }

        void Stop()
        {
            mPaused = true;
            mFrame = 0;
            mTotalElapsed = 0.f;
        }

        void Play() { mPaused = false; }
        void Paused() { mPaused = true; }

        bool IsPaused() const { return mPaused; }

    private:
        bool                                                mPaused;
        int                                                 mFrame;
        int                                                 mFrameCount;
        int                                                 mTextureWidth;
        int                                                 mTextureHeight;
        float                                               mTimePerFrame;
        float                                               mTotalElapsed;
        float                                               mDepth;
        float                                               mRotation;
        DirectX::XMFLOAT2                                   mOrigin;
        DirectX::XMFLOAT2                                   mScale;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    mTexture;
    };

# Example
This example uses a sprite sheet that has 4 frames of animation ([shipanimated.dds](https://github.com/Microsoft/DirectXTK/wiki/shipanimated.dds))

![Ship Animated](https://github.com/Microsoft/DirectXTK/wiki/images/shipanimated.png)

    #include "DDSTextureLoader.h"
    #include "SpriteBatch.h"
    #include "AnimatedTexture.h"

    // Create a texture using our sprite sheet
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ship;
    hr = CreateDDSTextureFromFile( device, L"shipanimated.dds", nullptr,
        ship.GetAddressOf() );
    DX::ThrowIfFailed(hr);

    ...

    // Create a sprite batch for drawing
    std::unique_ptr<DirectX::SpriteBatch> spriteBatch( new SpriteBatch( context ) );

    // Create an AnimatedTexture helper class instance and set it to use our texture
    // which is assumed to have 4 frames of animation with a FPS of 2 seconds
    std::unique_ptr<AnimatedTexture> sprite(
        new AnimatedTexture( XMFLOAT2(0,0), 0.f, 2.f, 0.5f ) );
    sprite->Load( ship.Get(), 4, 2 );

    ...

    // Update the sprite sheet animation based on current time
    sprite->Update( timeDelta );

    ...

    // Render the sprite using sprite batch
    spriteBatch->Begin();
    // TODO - background draw here

    sprite->Draw( spriteBatch.get(), screenPos );

    // TODO - More sprites or text here
    spriteBatch->End();

