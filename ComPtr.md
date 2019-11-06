``Microsoft::WRL::ComPtr`` is a C++ template smart-pointer for COM objects that is used extensively in Windows Runtime (WinRT) C++ programming. It works in Win32 desktop applications as well, and will work on Windows 7. It is similar to ATL's ``CComPtr`` with some useful improvements. ``Microsoft::WRL:::ComPtr`` is in the Windows 8.x SDK and Windows 10 SDK, which, unlike ATL, is available when using the Express versions of Visual Studio. It is used extensively in _DirectX Tool Kit_ to properly handle COM reference counting maintenance.

# Header

```cpp
#include <wrl/client.h>
```
> If you use ``#include <wrl.h>`` then all the WRL types will be included which is a reasonable choice for UWP apps. If you just plan to use ``ComPtr`` then the client header is all you need.

# Namespace
In keeping with C++ best practice, you should use fully-qualified names in ``.h`` header files.

```cpp
Microsoft::WRL::ComPtr<T> variable;
```

In ``.cpp`` source files, you can add the following to your module to make it less verbose to use ComPtr:

```cpp
using Microsoft::WRL::ComPtr;

ComPtr<T> variable;
```

# Calling Direct3D functions
Whenever you need to obtain a raw pointer from a ComPtr such as calling a Direct3D function that takes a ``*`` (i.e. a pointer), use **Get**()

```cpp
ComPtr<ID3D11RasterizerState> rasterState;

device->RSSetState( rasterState.Get() );
```

> If you want to know why we have to use ``.get()`` and ``.Get()`` to convert smart-pointers to standard 'raw' pointers, see [this post](http://herbsutter.com/2012/06/21/reader-qa-why-dont-modern-smart-pointers-implicitly-convert-to/)

A number of Direct3D APIs actually take an array of pointers to COM objects because they can be passed one or more objects at once. When you have just one to pass, you'll often see **GetAddressOf**() used because you are passing to a function parameter that is ``**`` (i.e. pointer-to-a-pointer). Because ``operator&`` maps to **ReleaseAndGetAddressOf**(), when calling Direct3D functions that take a pointer to an array of inputs and you are passing a single pointer, use the explicit **GetAddressOf**() method instead:

```cpp
context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(),
    m_depthStencilView.Get());
// or
auto rt = m_renderTargetView.Get();
context->OMSetRenderTargets(1, &rt, m_depthStencilView.Get());
````

For example, if you used ``context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView.Get());`` the ``m_renderTargetView`` variable would get released before the call to ``OMSetRenderTargets`` and the result would likely crash.

If building an array of Direct3D interface pointers, you should build one as a local variable as there's no direct way to convert an array of ``ComPtr<T>`` to an array of ``T*``.

```cpp
ID3D11SamplerState* samplers[] = { sampler1.Get(), sampler2.Get() };
context->PSSetSamplers(0, _countof(samplers), samplers);
```

# Initialization
Generally, if you are creating a fresh ComPtr and then using a COM creation function or factory, you can use **GetAddressOf**() since you know the ComPtr is initially null.

```cpp
ComPtr<ID3D11Device> device;

hr = D3D11CreateDevice( ..., device.GetAddressOf(), ... );
// or
hr = D3D11CreateDevice( ..., &device, ... );
```

If you are using a ComPtr as a class member, then you should use **ReleaseAndGetAddressOf**() to ensure any existing object reference is properly released. The overload for ``operator&`` does the same thing as **ReleaseAndGetAddressOf**(), but the explicit use of the method name is more clear in code.

```cpp
ComPtr<ID3D11Device> m_device;

hr = D3D11CreateDevice( ..., m_device.ReleaseAndGetAddressOf(), ... );
// or
hr = D3D11CreateDevice( ..., &m_device, ... );
```

> Note that ATL's ``CComPtr`` asserted that the object was always null for ``operator&``, which had a potential to cause resource leaks.

# Parameters
Keep in mind is that passing ComPtr variables by value will result in incrementing/decrementing the reference count. You can avoid this by either passing the smart-pointer by const reference or passing a raw pointer instead. Raw pointers are generally the more flexible option, but require the use of **Get**() at the call-site.

```cpp
void func( const Microsoft::WRL::ComPtr<T>& param );
// or
void func( T* param );
```

When passing ComPtr variables to constructors, you usually want the new class to increase the reference count, which is handled automatically by assigning the raw pointer to a new ``ComPtr<T>`` or by copying one ``ComPtr<T>`` to another.

# Obtaining new interfaces
ComPtr provides a much simpler syntax for doing ``QueryInterface`` calls on COM Objects

```cpp
ComPtr<ID3D11Device> device;

hr = D3D11CreateDevice( ..., device.GetAddressOf(), ... );
if (SUCCEEDED(hr))
   // device created

ComPtr<ID3D11Device1> device1;
hr = device.As( &device1 );
if (SUCCEEDED(hr))
{
    // was able to obtain the ID3DDevice1 interface
    // (we are on a DirectX 11.1+ system)
    device1->...
}

ComPtr<ID3D11Debug> d3dDebug;
hr = device.As( &d3dDebug );
if (SUCCEEDED(hr))
{
    // was able to obtain the ID3D11Debug interface
    d3dDebug->...
}
```

> You can also use ``CopyTo`` to perform similar operations to ``As``.

# Clearing a ComPtr
If you wish to release a reference from a particular ComPtr, you can use **Reset**() which sets the ComPtr to nullptr releasing any previous reference.

```cpp
ComPtr<ID3D11Debug> d3dDebug;

...

d3dDebug.Reset();
```

> ``d3dDebug = nullptr;`` has the same effect, but ``Reset`` is a bit more clear.

# Checking for null
Like many smart-pointers, there is an ``operator bool`` overload to let you easily check if a ComPtr is currently set to null which is often useful in assert statements:

```cpp
ComPtr<ID3D11Debug> d3dDebug;

...

if ( !d3dDebug )
    // d3dDebug is currently set to nullptr
...
assert( d3dDebug ); // trigger error in debug builds if d3dDebug is set to null
```

# Constructing, assigning, and copying ComPtr
Remember that if you set a ComPtr to a raw pointer or another ComPtr via the copy constructor or the assignment operator, the behavior is to increase the reference count. This is assuming the original raw pointer or ComPtr will still be calling ``Release``, and the new copy will also be calling ``Release``.

If you want to give a raw pointer to a ComPtr without changing the reference count, you need to use ``Attach``.

If you want to give a pointer in one ComPtr to another ComPtr without changing the reference count (i.e. transferring ownership), you need to use ``Detach`` from the old ComPtr followed by an ``Attach`` to the new one. Alternatively, you can use ``Swap`` to achieve the same thing when one of the ComPtr variables involved is already null.

ComPtr has a move constructor and a move assignment operator both of which effectively perform a ``Swap``.

> When using operator ``&``, ``GetAddressOf`` or ``ReleaseAndGetAddressOf`` the reference count is not changed as typically these are used in contexts where the reference count has already been increased (i.e. newly constructed objects coming from APIs, getter methods, etc.).

# IUnknown methods
ComPtr goes through some lengths to hide the base IUnknown methods ``AddRef``, ``Release``, and ``QueryInterface``. You should instead make use of the various ComPtr facilities to achieve the same effect (as discussed above).

> It is asking for serious trouble if you try to circumvent this by doing something like ``object.Get()->Release()``.

# Platform notes
The bulk of the Windows Runtime Template Library (WRL) is intended to make it easier to use the new WinRT style APIs introduced with Windows Store and Universal Windows Platform apps. It is a pure C++ template library and does not make use of the C++/CX language extensions, which are themselves another way to consume WinRT APIs from C++. You can, however, mix usage of the two easily and most C++/CX programs will use at least ``Microsoft::WRL::ComPtr`` for managing COM objects for non-WinRT APIs such as Direct3D.

Because WRL is a pure C++ template library, it has no particular platform support requirements beyond being in the Windows 8.x or Windows 10 SDKs. You can therefore freely use it in Windows desktop applications built for Windows 7, Windows 8.x, or Windows 10 as well as in Windows Store, Windows phone, Xbox One, and Universal Windows Platform apps.

> The Windows 8.0 SDK and later that includes WRL does not support targeting Windows XP / Windows Server 2003. For these older operating systems you have to make use of the Windows 7.1A SDK instead. See [this blog post](https://walbourn.github.io/visual-studio-2012-update-1/) for more details.

# Remarks

Making use of the older ``_com_ptr_t`` Visual Studio smart-pointer class which was originally intended as a non-ATL option is not recommended.

If using [C++/WinRT projections](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/), you could also choose to use ``winrt::com_ptr`` instead of ``Microsoft::WRL:ComPtr``. It is similar in design, but not identical. See [Microsoft Docs](https://docs.microsoft.com/en-us/uwp/cpp-ref-for-winrt/com-ptr).

Note that WRL is not supported for Managed C++ (``/clr``) projects. In that case you should make use of ATL's ``CComPtr`` instead keeping in mind the difference in behavior of ``operator&``. See [Microsoft Docs](https://docs.microsoft.com/en-us/cpp/atl/reference/ccomptr-class).

# Further reading

[ComPtr on MSDN](https://docs.microsoft.com/en-us/cpp/windows/comptr-class)  
[COM on MSDN](https://docs.microsoft.com/en-us/windows/desktop/com/the-component-object-model)  
[Reference Counting (Direct3D 10)](https://docs.microsoft.com/en-us/windows/desktop/direct3d10/d3d10-graphics-programming-guide-api-features-reference-counting)
