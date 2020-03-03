When programming COM APIs like Direct3D, it is important to always check the ``HRESULT`` return value for success or failure. This can be done using the ``SUCCEEDED`` or ``FAILED`` macros, but can get tedious when making lots of calls especially for proper cleanup on exit of every function.

```cpp
hr = m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil));
if (FAILED(hr))
{
    // Clean up for partial success before here
    return hr; // Must keep passing the error code back all the way to the main loop
}
```

> Not all Direct3D functions return ``HRESULT``. Many of them return ``void`` because they can't fail, fail silently, or the failure will be reported on the next ``Present``.

## Fast Fail
A simple way to handle always fatal errors is to use C++ exceptions (``/EHsc`` is the default for Visual C++, and has no code impact for x64 native, ARM, or ARM64 platforms). The ``DX::ThrowIfFailed`` helper can be used whenever a failure is fatal and should result in 'fast-fail' of the application. 

```cpp
DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc,
    nullptr, &depthStencil));
```

> A "fast-fail" failure is an error case you never expect to happen in the real world, but if it does happen you want to make sure the process dies quickly so as to avoid 'hiding' the real bug, hardware failure, or other failure that your program just can't handle. In the case above, for example, you are assuming you aren't passing invalid parameters to the function (``E_INVALIDARG``) because you've tested it on a range of supported Direct3D feature level devices, and that you won't run out of memory (``E_OUTOFMEMORY``) because you scale your content appropriately.

## Runtime error handling
Otherwise, traditional ``if FAILED(hr)`` or ``if SUCCEEDED(hr)`` patterns should be used to handle failures that the application can recover from (i.e. are not fatal). If you want to handle a specific HRESULT, then you might do something like:

```cpp
HRESULT hr = m_d3dDevice->CreateTexture2D(&depthStencilDesc,
    nullptr, &depthStencil);
if (hr == E_INVALIDARG)
{
    // Do something here in response to this specific error.
}
DX::ThrowIfFailed(hr);
```

For a case where you want to do the error-handling for an HRESULT yourself, be sure to use the ``SUCCEEDED`` or ``FAILED`` macro:

```cpp
HRESULT hr = m_d3dDevice->CreateTexture2D(&depthStencilDesc,
    nullptr, &depthStencil);
if (FAILED(hr))
    // Error handling
```

> The legacy [DXUT](https://github.com/Microsoft/DXUT) framework makes use of macros like ``V`` and ``V_RETURN`` as a pattern for dealing with ``HRESULT`` values, but these make assumptions about the surrounding functions and are really only suited to sample development.

# Basic version

The ``ThrowIfFailed`` helper is not part of the _DirectX Tool Kit_; it's declared in some global header in your application. The C++ DirectX templates for Universal Windows Platform apps, Windows 8.x Store, Windows phone 8.x, Xbox One, and the [Direct3D Win32 Game](https://walbourn.github.io/direct3d-win32-game-visual-studio-template/) templates all make use of the ``DX::ThrowIfFailed`` helper--you'll typically find it declared in the ``pch.h`` header.

```cpp
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
```

# Enhancements

The templates all include the basic implementation above, but production use might want to utilize a slightly improved version as follows (which are included in the [[DeviceResources]] variants of the [Direct3D Win32 Game](https://walbourn.github.io/direct3d-game-visual-studio-templates-redux/) templates).

```cpp
#include <exception>

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) : result(hr) {}

        const char* what() const override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X",
                static_cast<unsigned int>(result));
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
```

# Further reading

* [C++ Exception Handling](https://docs.microsoft.com/en-us/cpp/cpp/cpp-exception-handling)
* [Managing Exceptions with the Debugger](https://docs.microsoft.com/en-us/visualstudio/debugger/managing-exceptions-with-the-debugger)
* [Error Handling in COM](https://docs.microsoft.com/en-us/windows/desktop/LearnWin32/error-handling-in-com)
