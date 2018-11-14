This is a C++ implementation of a simple console for displaying text. As more text is printed, the text display scrolls up.

![Text console test](https://github.com/Microsoft/DirectXTK/wiki/images/TextConsole.PNG)

[TextConsole.h](https://github.com/Microsoft/DirectXTK/wiki/TextConsole.h)

```cpp
class TextConsole
{
public:
    TextConsole();
    TextConsole(ID3D11DeviceContext* context, const wchar_t* fontName);

    TextConsole(TextConsole&&) = default;
    TextConsole& operator= (TextConsole&&) = default;

    TextConsole(TextConsole const&) = delete;
    TextConsole& operator= (TextConsole const&) = delete;

    void Render();

    void Clear();

    void Write(const wchar_t *str);
    void WriteLine(const wchar_t *str);
    void Format(const wchar_t* strFormat, ...);

    void SetWindow(const RECT& layout);

    void XM_CALLCONV SetForegroundColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat4(&m_textColor, color); }

    void ReleaseDevice();
    void RestoreDevice(ID3D11DeviceContext* context, const wchar_t* fontName);

    void SetViewport(const D3D11_VIEWPORT& viewPort);

    void SetRotation(DXGI_MODE_ROTATION rotation);   

private:
    void ProcessString(const wchar_t* str);
    void IncrementLine();

    RECT                                            m_layout;
    DirectX::XMFLOAT4                               m_textColor;

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
```

[TextConsole.cpp](https://github.com/Microsoft/DirectXTK/wiki/TextConsole.cpp)

```cpp
#include "TextConsole.h"

#include <assert.h>

using Microsoft::WRL::ComPtr;

using namespace DirectX;

TextConsole::TextConsole()
    : m_textColor(1.f, 1.f, 1.f, 1.f)
{
    Clear();
}

TextConsole::TextConsole(ID3D11DeviceContext* context, const wchar_t* fontName)
    : m_textColor(1.f, 1.f, 1.f, 1.f)
{
    RestoreDevice(context, fontName);

    Clear();
}

void TextConsole::Render()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    float lineSpacing = m_font->GetLineSpacing();

    float x = float(m_layout.left);
    float y = float(m_layout.top);

    XMVECTOR color = XMLoadFloat4(&m_textColor);

    m_batch->Begin();

    unsigned int textLine = unsigned int(m_currentLine - m_rows + m_rows + 1) % m_rows;

    for (unsigned int line = 0; line < m_rows; ++line)
    {
        XMFLOAT2 pos(x, y + lineSpacing * float(line));

        if (*m_lines[textLine])
        {
            m_font->DrawString(m_batch.get(), m_lines[textLine], pos, color);
        }

        textLine = unsigned int(textLine + 1) % m_rows;
    }

    m_batch->End();
}

void TextConsole::Clear()
{
    if (m_buffer)
    {
        memset(m_buffer.get(), 0, sizeof(wchar_t) * (m_columns + 1) * m_rows);
    }

    m_currentColumn = m_currentLine = 0;
}

void TextConsole::Write(const wchar_t *str)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    ProcessString(str);
}

void TextConsole::WriteLine(const wchar_t *str)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    ProcessString(str);
    IncrementLine();
}

void TextConsole::Format(const wchar_t* strFormat, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    va_list argList;
    va_start(argList, strFormat);

    size_t len = _vscwprintf(strFormat, argList) + 1;

    if (m_tempBuffer.size() < len)
        m_tempBuffer.resize(len);

    memset(m_tempBuffer.data(), 0, sizeof(wchar_t) * len);

    vswprintf_s(m_tempBuffer.data(), m_tempBuffer.size(), strFormat, argList);

    va_end(argList);

    ProcessString(m_tempBuffer.data());
}

void TextConsole::SetWindow(const RECT& layout)
{
    m_layout = layout;

    assert(m_font != 0);

    float lineSpacing = m_font->GetLineSpacing();
    unsigned int rows = std::max<unsigned int>(1, static_cast<unsigned int>(float(layout.bottom - layout.top) / lineSpacing));

    RECT fontLayout = m_font->MeasureDrawBounds(L"X", XMFLOAT2(0,0));
    unsigned int columns = std::max<unsigned int>(1, static_cast<unsigned int>(float(layout.right - layout.left) / float(fontLayout.right - fontLayout.left)));

    std::unique_ptr<wchar_t[]> buffer(new wchar_t[(columns + 1) * rows]);
    memset(buffer.get(), 0, sizeof(wchar_t) * (columns + 1) * rows);

    std::unique_ptr<wchar_t*[]> lines(new wchar_t*[rows]);
    for (unsigned int line = 0; line < rows; ++line)
    {
        lines[line] = buffer.get() + (columns + 1) * line;
    }

    if (m_lines)
    {
        unsigned int c = std::min<unsigned int>(columns, m_columns);
        unsigned int r = std::min<unsigned int>(rows, m_rows);

        for (unsigned int line = 0; line < r; ++line)
        {
            memcpy(lines[line], m_lines[line], c * sizeof(wchar_t));
        }
    }

    std::swap(columns, m_columns);
    std::swap(rows, m_rows);
    std::swap(buffer, m_buffer);
    std::swap(lines, m_lines);
}

void TextConsole::ReleaseDevice()
{
    m_batch.reset();
    m_font.reset();
    m_context.Reset();
}

void TextConsole::RestoreDevice(ID3D11DeviceContext* context, const wchar_t* fontName)
{
    m_context = context;

    m_batch = std::make_unique<SpriteBatch>(context);

    ComPtr<ID3D11Device> device;
    context->GetDevice(device.GetAddressOf());

    m_font = std::make_unique<SpriteFont>(device.Get(), fontName);

    m_font->SetDefaultCharacter(L' ');
}

void TextConsole::SetViewport(const D3D11_VIEWPORT& viewPort)
{
    if (m_batch)
    {
        m_batch->SetViewport(viewPort);
    }
}


void TextConsole::SetRotation(DXGI_MODE_ROTATION rotation)
{
    if (m_batch)
    {
        m_batch->SetRotation(rotation);
    }
}

void TextConsole::ProcessString(const wchar_t* str)
{
    if (!m_lines)
        return;

    float width = float(m_layout.right - m_layout.left);

    for (const wchar_t* ch = str; *ch != 0; ++ch)
    {
        if (*ch == '\n')
        {
            IncrementLine();
            continue;
        }

        bool increment = false;

        if (m_currentColumn >= m_columns)
        {
            increment = true;
        }
        else
        {
            m_lines[m_currentLine][m_currentColumn] = *ch;

            auto fontSize = m_font->MeasureString(m_lines[m_currentLine]);
            if (XMVectorGetX(fontSize) > width)
            {
                m_lines[m_currentLine][m_currentColumn] = L'\0';

                increment = true;
            }
        }

        if (increment)
        {
            IncrementLine();
            m_lines[m_currentLine][0] = *ch;
        }

        ++m_currentColumn;
    }
}

void TextConsole::IncrementLine()
{
    if (!m_lines)
        return;

    m_currentLine = (m_currentLine + 1) % m_rows;
    m_currentColumn = 0;
    memset(m_lines[m_currentLine], 0, sizeof(wchar_t) * (m_columns + 1));
}
```

# Example

To use the text console, add a variable to hold the instance:

```cpp
std::unique_ptr<DX::TextConsole> m_console;
```

Create it in your initialization:

```cpp
m_console = std::make_unique<DX::TextConsole>();

// Optionally set a color other than white with something like:
// m_console->SetForegroundColor(Colors::Yellow);
```

In the equivalent to ``CreateDevice`` / ``CreateDeviceDependentResources``:

```cpp
m_console->RestoreDevice(context, L"consolas.spritefont");
```

> For best results, consider using a monospaced (non-proportional) font. Proportional fonts will work, but might introduce additional newlines and will have a 'ragged' right edge.

In ``OnDeviceLost``:

```cpp
m_console->ReleaseDevice();
```

In the equivalent to ``CreateResources`` / ``CreateWindowSizeDependentResources``, provide the pixel rectangle where you want the console text rendered:

```cpp
RECT size = m_deviceResources->GetOutputSize();

m_console->SetWindow(SimpleMath::Viewport::ComputeTitleSafeArea(size.right, size.bottom));
```

Then in your ``Render`` function after clearing the screen and drawing whatever background image you want.

```cpp
m_console->Render();
```

Wherever you want to add text to the console, use ``Write``, ``WriteLine``, and/or ``Format``:

```cpp
m_console->WriteLine(L"This is a test");
m_console->WriteLine(L"Line 2");
m_console->Format(L"Time %u, %f ", timer.GetFrameCount(), timer.GetTotalSeconds());
```

If you want to empty the console text, call ``Clear``.

# Threading model

The text console class is thread-safe using a built-in mutex so that you can call ``Write``, ``WriteLine``, ``Format``, or ``Clear`` from other threads. Since it uses a ``SpriteBatch`` to render, the ``Render`` function itself must be run on the same thread that is using the ``context`` you provided.

# Xbox One
Since Xbox One XDK apps do not have 'lost device' scenarios, you can avoid using ``RestoreDevice`` and ``ReleaseDevice``, and just use the alternate constructor in ``CreateDevice`` / ``CreateDeviceDependentResources``:

```cpp
m_console = std::make_unique<DX::TextConsole>(context, L"consolas.spritefont");
```

The example above uses the [[Viewport]] function ``ComputeTitleSafeArea`` which is important when rendering text or other UI element on televisions. This is of course optional when rendering on a PC or mobile device and can use the full render viewport instead.

For 11.X Fast Semantics support, you need to call ``SetViewport`` before rendering. This is optional for other platforms.

```cpp
auto viewport = m_deviceResources->GetScreenViewport();
m_console->SetViewport(viewport);
```

# UWP
For UWP apps, be sure to set the rotation for mobile devices, laptops, tablets, etc.:

```cpp
m_console->SetRotation( m_deviceResources->GetRotation() );
```
