//
// TextConsole.h
//
// Renders a simple on-screen console where you can output text information
//

#pragma once

#include "SpriteBatch.h"
#include "SpriteFont.h"

#include <mutex>
#include <vector>

namespace DX
{
    class TextConsole
    {
    public:
        TextConsole();
        TextConsole(ID3D11DeviceContext* context, const wchar_t* fontName);

        void Render();

        void Clear();

        void Write(_In_z_ const wchar_t *str);
        void WriteLine(_In_z_ const wchar_t *str);
        void Format(_In_z_ _Printf_format_string_ const wchar_t* strFormat, ...);

        void SetWindow(const RECT& layout);

        void XM_CALLCONV SetForegroundColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat4(&m_textColor, color); }

        void SetDebugOutput(bool debug) { m_debugOutput = debug; }

        void ReleaseDevice();
        void RestoreDevice(ID3D11DeviceContext* context, const wchar_t* fontName);

    private:
        void ProcessString(const wchar_t* str);
        void IncrementLine();

        RECT                                            m_layout;
        DirectX::XMFLOAT4                               m_textColor;

        bool                                            m_debugOutput;

        unsigned int                                    m_columns;
        unsigned int                                    m_rows;
        unsigned int                                    m_currentColumn;
        unsigned int                                    m_currentLine;

        std::unique_ptr<wchar_t[]>                      m_buffer;
        std::unique_ptr<wchar_t*[]>                     m_lines;
        std::vector<wchar_t>                            m_tempBuffer;

        std::unique_ptr<DirectX::SpriteBatch>           m_batch;
        std::unique_ptr<DirectX::SpriteFont>            m_font;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_context;

        std::mutex                                      m_mutex;
    };
}