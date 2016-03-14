This is a C++ implementation of a helper that will composite Xbox controller artwork into a string inspired by [this XNA tutorial](http://mort8088.com/2011/03/11/xna-4-0-tutorial-4-spritebatch-extended/).

![Controller font test](https://github.com/Microsoft/DirectXTK/wiki/images/ControllerFontExample.PNG)

For details on creating the ``xboxController.spritefont`` file, see [[MakeSpriteFont]].

[ControllerFont.h](https://github.com/Microsoft/DirectXTK/wiki/ControllerFont.h)

    enum class ControllerFont : wchar_t
    {
        LeftThumb = L' ',
        DPad = L'!',
        RightThumb = L'\"',
        Back = L'#',
        Guide = L'$',
        Start = L'%',
        XButton = L'&',
        AButton = L'\'',
        YButton = L'(',
        BButton = L')',
        RightShoulder = L'*',
        RightTrigger = L'+',
        LeftTrigger = L',',
        LeftShoulder = L'-',
    };

    inline void XM_CALLCONV DrawControllerString(_In_ DirectX::SpriteBatch* spriteBatch,
        _In_ DirectX::SpriteFont* textFont, _In_ DirectX::SpriteFont* butnFont,
        _In_z_ wchar_t const* text, DirectX::XMFLOAT2 const& position,
        DirectX::FXMVECTOR color = DirectX::Colors::White, float scale = 1)
    {
        using namespace DirectX;

        size_t textLen = wcslen(text);
        if (textLen >= 4096)
        {
            throw std::out_of_range("String is too long");
        }

        float buttonHeight = butnFont->GetLineSpacing();
        float buttonScale = (textFont->GetLineSpacing() * scale) / buttonHeight;
        float offsetY = buttonScale / 2.f;

        size_t j = 0;
        wchar_t strBuffer[4096] = {0};

        bool buttonText = false;

        XMFLOAT2 outPos = position;

        for (size_t ch = 0; ch < textLen; ++ch)
        {
            if (buttonText)
            {
                strBuffer[j++] = text[ch];

                if (text[ch] == L']')
                {
                    wchar_t button[2] = { 0 };

                    if (_wcsicmp(strBuffer, L"[A]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::AButton);
                    }
                    else if (_wcsicmp(strBuffer, L"[B]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::BButton);
                    }
                    else if (_wcsicmp(strBuffer, L"[X]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::XButton);
                    }
                    else if (_wcsicmp(strBuffer, L"[Y]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::YButton);
                    }
                    else if (_wcsicmp(strBuffer, L"[DPad]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::DPad);
                    }
                    else if (_wcsicmp(strBuffer, L"[Back]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::Back);
                    }
                    else if (_wcsicmp(strBuffer, L"[Start]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::Start);
                    }
                    else if (_wcsicmp(strBuffer, L"[Guide]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::Guide);
                    }
                    else if (_wcsicmp(strBuffer, L"[RThumb]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::RightThumb);
                    }
                    else if (_wcsicmp(strBuffer, L"[LThumb]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::LeftThumb);
                    }
                    else if (_wcsicmp(strBuffer, L"[RB]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::RightShoulder);
                    }
                    else if (_wcsicmp(strBuffer, L"[LB]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::LeftShoulder);
                    }
                    else if (_wcsicmp(strBuffer, L"[RT]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::RightTrigger);
                    }
                    else if (_wcsicmp(strBuffer, L"[LT]") == 0)
                    {
                        *button = static_cast<wchar_t>(ControllerFont::LeftTrigger);
                    }

                    if ( *button )
                    {
                        float bsize = XMVectorGetX(butnFont->MeasureString(button));
                        float offsetX = (bsize * buttonScale / 2.f);

                        outPos.x += offsetX;
                        outPos.y -= offsetY;
                        butnFont->DrawString(spriteBatch, button, outPos, Colors::White, 0.f, XMFLOAT2(0.f, 0.f), XMFLOAT2(buttonScale, buttonScale));
                        outPos.x += bsize * buttonScale;
                        outPos.y += offsetY;
                    }

                    memset(strBuffer, 0, sizeof(strBuffer));
                    j = 0;

                    buttonText = false;
                }
            }
            else
            {
                switch (text[ch])
                {
                case '\r':
                    break;

                case '[':
                    if (*strBuffer)
                    {
                        textFont->DrawString(spriteBatch, strBuffer, outPos, color, 0.f, XMFLOAT2(0.f, 0.f), XMFLOAT2(scale,scale));
                        outPos.x += XMVectorGetX(textFont->MeasureString(strBuffer)) * scale;
                        memset(strBuffer, 0, sizeof(strBuffer));
                        j = 0;
                    }
                    buttonText = true;
                    *strBuffer = L'[';
                    ++j;
                    break;

                case '\n':
                    if (*strBuffer)
                    {
                        textFont->DrawString(spriteBatch, strBuffer, outPos, color, 0.f, XMFLOAT2(0.f, 0.f), XMFLOAT2(scale, scale));
                        memset(strBuffer, 0, sizeof(strBuffer));
                        j = 0;
                    }
                    outPos.x = position.x;
                    outPos.y += textFont->GetLineSpacing() * scale;
                    break;

                default:
                    strBuffer[j++] = text[ch];
                    break;
                }
            }
        }

        if (*strBuffer)
        {
            textFont->DrawString(spriteBatch, strBuffer, outPos, color, 0.f, XMFLOAT2(0.f, 0.f), XMFLOAT2(scale, scale));
        }
    }

# Example

To use the controller, you need to create and initialize two font instances, and a [[SpriteBatch]]

    std::unique_ptr<DirectX::SpriteBatch> m_batch;
    std::unique_ptr<DirectX::SpriteFont> m_font;
    std::unique_ptr<DirectX::SpriteFont> m_ctrlFont;

    m_batch = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"myfont.spritefont" );
    m_ctrlFont = std::make_unique<SpriteFont>(device, L"xboxController.spritefont" );

In the ``Render`` function, you use the helper to draw the composed text:

    m_batch->Begin();

    DX::DrawControllerString(m_batch.get(), m_font.get(), m_ctrlFont.get(), L"Press [A] to select", pos);
    pos.y += m_font->GetLineSpacing();

    DX::DrawControllerString(m_batch.get(), m_font.get(), m_ctrlFont.get(), L"Press [B] to backup", pos);

    m_batch->End();
