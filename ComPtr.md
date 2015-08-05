``Microsoft::WRL::ComPtr`` is a C++ template smart-pointer for COM objects that is used extensively in WinRT programming. It works in Win32 desktop applications as well. It is similar to ATL's ``CComPtr`` with some useful improvements. ``Microsoft::WRL:::ComPtr`` is in the Windows 8.x SDK and Windows 10 SDK, which, unlike ATL, is available when using the Express versions of Visual Studio. It is used extensively in _DirectX Tool Kit_ to properly handle COM reference counting maintenance.

See [MSDN](http://msdn.microsoft.com/en-us/library/br244983.aspx)

    #include <wrl.h>
    // or
    #include <wrl/client.h>

Whenever you need to obtain a raw pointer from a ComPtr, use **Get**()

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    
    device.Get()

Generally, if you are creating a fresh ComPtr and then using a COM creation function or factory, you can use **GetAddressOf**() since you know the ComPtr is initially null.

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    
    hr = D3D11CreateDevice( ..., device.GetAddressOf(), ... );
    // or
    hr = D3D11CreateDevice( ..., &device, ... );

If you are using a ComPtr as a class member, then you should use **ReleaseAndGetAddressOf**() to ensure any existing object reference is properly released. The overload for ``operator&`` does the same thing as **ReleaseAndGetAddressOf**(), but the explicit use of the method name is clearer in code.

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    
    hr = D3D11CreateDevice( ..., m_device.ReleaseAndGetAddressOf(), ... );
    // or
    hr = D3D11CreateDevice( ..., &m_device, ... );

_Note that ATL's CComPtr asserted that the object was always null for ``operator&``, which had a potential to cause resource leaks._

Because ``operator&`` maps to **ReleaseAndGetAddressOf**(), when calling Direct3D functions that take a pointer to an array of inputs and you are passing a single pointer, use the explicit **GetAddressOf**() method instead:

    context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(),
        m_depthStencilView.Get());
    // or
    auto rt = m_renderTargetView.Get();
    context->OMSetRenderTargets(1, &rt, m_depthStencilView.Get());

# Parameters
Keep in mind is that passing ComPtr variables by value will result in incrementing/decrementing the reference count. You can avoid this by either passing the smart-pointer by const reference or passing a raw pointer instead. Raw pointers are generally the more flexible option, but require the use of **Get**() at the call-site.

    void func( const Microsoft::WRL::ComPtr<T>& param );
    // or
    void func( T* param );

When passing ComPtr variables to constructors, you usually want the new class to increase the reference count, which is handled automatically by assigning the raw pointer to a new ``ComPtr<T>`` or by copying one ``ComPtr<T>`` to another.

# Namespaces
In keeping with C++ best practice, you should use fully-qualified names in header files.

    Microsoft::WRL::ComPtr<T> variable;

In .cpp files, you can add the following to your module to make it less verbose to use ComPtr:

    using Microsoft::WRL::ComPtr;
    
    ComPtr<T> variable;

# Obtaining new interfaces
ComPtr provides a much simpler syntax for doing ``QueryInterface`` calls on COM Objects

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    
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

# Clearing a ComPtr
If you wish to release a reference from a particular ComPtr, you can use **Reset**() which sets the ComPtr to nullptr releasing any previous reference.

    ComPtr<ID3D11Debug> d3dDebug;
    
    ...
    
    d3dDebug.Reset();

# Platform notes
The bulk of the Windows Runtime Template Library (WRL) is intended to make it easier to use the new WinRT style APIs introduced with Windows Store and the universal Windows app platform. It is a pure C++ template library and does not make use of the C++/CX language extensions, which are themselves another way to consume WinRT APIs from C++. You can, however, mix usage of the two easily and most C++/CX programs will use at least ``Microsoft::WRL::ComPtr`` for managing COM objects for non-WinRT APIs such as Direct3D.

Because WRL is a pure C++ template library, it has no particular platform support requirements beyond being in the Windows 8.x or Windows 10 SDKs. You can therefore freely use it in Windows desktop applications built for Windows Vista, Windows 7, Windows 8.x, or Windows 10 as well as in Windows Store, Windows phone, Xbox One, and universal Windows apps.
