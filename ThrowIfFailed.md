When programming COM APIs like Direct3D, it is important to always check the ``HRESULT`` return value for success or failure. This can be done using the ``SUCCEEDED`` or ``FAILED`` macros, but can get tedious when making lots of calls

> Not all Direct3D functions return ``HRESULT``. Many of them return ``void`` because they can't fail, fail silently, or the failure will be reported on the next ``Present``.

``DX::ThrowIfFailed`` should be used whenever a failure is fatal and should result in 'fast-fail' of the application. Otherwise, traditional ``if FAILED(hr)`` or ``if SUCCEEDED(hr)`` patterns should be used to handle failures that the application can recover from (i.e. are not fatal).

    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc,
        nullptr, &depthStencil));

If you want to handle a specific HRESULT, then you might do something like:

    HRESULT hr = m_d3dDevice->CreateTexture2D(&depthStencilDesc,
        nullptr, &depthStencil);
    if (hr == E_INVALIDARG)
    {
        // Do something here in response to this specific error.
    }
    DX::ThrowIfFailed(hr);

For a case where you want to do the error-handling for an HRESULT yourself, be sure to use the ``SUCCEEDED`` or ``FAILED`` macro:

    HRESULT hr = m_d3dDevice->CreateTexture2D(&depthStencilDesc,
        nullptr, &depthStencil);
    if (FAILED(hr))
        // Error handling

> The legacy [DXUT](https://github.com/Microsoft/DXUT) framework makes use of macros like ``V`` and ``V_RETURN`` as a pattern for dealing with ``HRESULT`` values, but these make assumptions about the surrounding functions and are really only suited to samples development.

# Basic version

The ``ThrowIfFailed`` helper is not part of the _DirectX Tool Kit_. The C++ DirectX templates for universal Windows apps, Windows 8 Store, Windows phone 8, Xbox One, and the [Direct3D Win32 Game](http://blogs.msdn.com/b/chuckw/archive/2015/01/06/direct3d-win32-game-visual-studio-template.aspx) templates all make use of the ``DX::ThrowIfFailed`` helper--you'll typically find it declared in the ``pch.h``.

    #include <exception>

    namespace DX
    {
        inline void ThrowIfFailed(HRESULT hr)
        {
            if (FAILED(hr))
            {
                // Set a breakpoint on this line to catch DirectX API errors
                throw std::exception();
            }
        }
    }

# Enhancements

The templates all include the basic implementation above, but production use might want to utilize a slightly improved version as follows (which are included in the [[DeviceResources]] variants of the [Direct3D Win32 Game](http://blogs.msdn.com/b/chuckw/archive/2015/12/17/direct3d-game-visual-studio-templates-redux.aspx) templates).

    #include <exception>

    namespace DX
    {
        // Helper class for COM exceptions
        class com_exception : public std::exception
        {
        public:
            com_exception(HRESULT hr) : result(hr) {}

            virtual const char* what() const override
            {
                static char s_str[64] = { 0 };
                sprintf_s(s_str, "Failure with HRESULT of %08X", result);
                return s_str;
            }

        private:
            HRESULT result;
        };

        // Helper utility converts D3D API failures into exceptions.
        inline void ThrowIfFailed(HRESULT hr)
        {
            if (FAILED(hr))
            {
                throw com_exception(hr);
            }
        }
    }

# Further reading

* [C++ Exception Handling](http://msdn.microsoft.com/en-us/library/4t3saedz.aspx)
* [Managing Exceptions with the Debugger](https://msdn.microsoft.com/en-us/library/x85tt0dd.aspx)
* [Error Handling in COM](https://msdn.microsoft.com/en-us/library/windows/desktop/ff485842.aspx)