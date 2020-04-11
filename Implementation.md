# Compiler conformance

For Visual C++, the projects make use of the default C++11/C++14 mode rather than ``/std:c++17`` mode, although occasionally run a validation pass to confirm it's fully compatible with C++17 (i.e. avoid all use of ``auto_ptr``, ``random_shuffle``, and other deprecated features). The library does not make use of newer C++17 language & library features such as ``string_view``, ``static_assert`` without a message, etc. although that may change in the future. The projects make use of ``/Wall``, ``/permissive-``, ``/Zc:__cplusplus``, and ``/analyze`` to ensure a high-level of C++ conformance.

For clang/LLVM for Windows, there is a ``CMakeList.txt`` provided to validate the code and ensure a high-level of conformance. This primarily means addressing warnings generated using ``/Wall -Wpedantic -Wextra``.

# Naming conventions

While the _DirectX Tool Kit_ design is heavily influenced by the XNA Game Studio framework C# object design, it uses C++ conventions consistent with modern Win32 APIs rather than the strict .NET use of [PascalCase](https://en.wikipedia.org/wiki/CamelCase) as enforced by FXCop.

* PascalCase for class names, methods, functions, and enums.
* camelCase for class member variables, struct members
* UPPERCASE for preprocessor defines (and nameless enums)

The library does not generally make use of [Hungarian notation](https://en.wikipedia.org/wiki/Hungarian_notation) which as been deprecated for Win32 C++ APIs for many years, with the exception of a few uses of ``p`` for pointers and ``sz`` for strings.

# Type usage

The use of Standard C++ types is preferred including the fundamental types supplied by the language (i.e. ``int``, ``unsigned int``, ``size_t``, ``ptrdiff_t``, ``bool``, ``true``/``false``, ``char``, ``wchar_t``) with the addition of the C99 fixed width types (i.e. ``uint32_t``, ``uint64_t``, ``intptr_t``, ``uintptr_t``, etc.)

Avoid using Windows "portability" types except when dealing directly with Win32 APIs: ``VOID``, ``UINT``, ``INT``, ``DWORD``, ``FLOAT``, ``BOOL``, ``TRUE``/``FALSE``, ``WCHAR``, ``CONST``, etc.

# Unicode

The Modern C++ recommendation is to use [UTF-8 Everywhere](http://utf8everywhere.org/) except where specifically interacting with Win32 or Windows Runtime APIs which require converting to ``wchar_t`` (UTF-16LE).

* Always explicitly call the "W" or "A" versions of Win32 APIs. Prefer to convert from UTF-8 at the point of call.
* Do not rely on the macros to select "W" vs. "A", but always build with ``UNICODE`` and ``_UNICODE`` defined for safety.

Most functions in _DirectX Tool Kit_ take ``wchar_t*`` since they are passed directly along to Win32 types. [[SpriteFont]] provides both UTF-8 ``char*`` and UTF-16LE ``wchar_t*`` methods.

# SAL annotation
The _DirectX Toolkit_ library makes extensive use of SAL2 annotations (``_In_``, ``_Outptr_opt_``, etc.) which greatly improves the accuracy of the Visual C++ static code analysis (also known as PREFAST). The standard Windows headers ``#define`` them all to empty strings if not building with ``/analyze``, so they have no effect on code-generation.

# The pImpl idiom
DirectXTK's implementation makes extensive use of the [pImpl idiom](http://en.wikipedia.org/wiki/Opaque_pointer). This keeps the public headers slim and minimizes inter-module dependencies.

```cpp
// SpriteBatch.h public header
class SpriteBatch
{
public:
    SpriteBatch(...) noexcept(false);

    SpriteBatch(SpriteBatch&& moveFrom) noexcept;
    SpriteBatch& operator= (SpriteBatch&& moveFrom) noexcept;

    SpriteBatch(SpriteBatch const&) = delete;
    SpriteBatch& operator=(SpriteBatch const&) = delete;

    virtual ~SpriteBatch();
    ...

private:
    // Private implementation.
    class Impl;

    std::unique_ptr<Impl> pImpl;
};
```

This also allows the implementation to allocate the pImpl class internally using ``_aligned_malloc(x,16);`` so that we can use the DirectXMath aligned ``XMVECTOR`` and ``XMMATRIX`` types directly in the implementation across all architectures.

* The class default constructor can throw an exception since it creates a Impl instance, hence the ``noexcept(false)``.

* The class destructor can't be inline and must be implemented in the ``.cpp`` file since ``unique_ptr`` needs the real ``Impl`` type size.

# Calling-conventions
Public methods in the library are explicitly marked ``__cdecl`` to ensure consistent behavior no matter what the client code is using. Internally it is not specified as it's assumed from the default setting except where ``XM_VECTORCALL`` is utilized (see the _DirectXMath_ section below)

The ``std::function`` is used for callbacks as a general pattern so that client code can provide function pointers, lambdas, functors, etc. To support building with a mix of calling conventions, we need to annotate the ``std::function`` correctly.

```cpp
    HRESULT __cdecl SaveWICTextureToFile( /*...*/,
        std::function<void __cdecl(IPropertyBag2*)> setCustomProps
            = nullptr );
```            

# Default constructors/assignment operators
The C++11 standard includes a more efficient ``= default`` and ``= delete`` construct for dealing with special constructors and operators.

To declare a standard copy constructor and copy assignment operators, we use:

```cpp
Rectangle(const Rectangle&) = default;
Rectangle& operator= (const Rectangle&) = default;
```

To prevent copying we use:

```cpp
// Prevent copying.
SpriteBatch(SpriteBatch const&) = delete;
SpriteBatch& operator= (SpriteBatch const&) = delete;
```

Per [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md) recommendations, if you declare a copy constructor/assignment operator, you should also defined the move equivalents:

```cpp
Rectangle(Rectangle&&) = default;
Rectangle& operator= (Rectangle&&) = default;
```

> Note that use of ``= default`` can improve codegen for derived types as well.

# DirectXMath Parameter Conventions
The library uses the [DirectXMath](https://docs.microsoft.com/en-us/windows/desktop/dxmath/pg-xnamath-internals#Call_Conventions) calling convention types to improve parameter passing of ``XMVECTOR`` and ``XMMATRIX`` types.

# Further Reading
[Dual-use Coding Techniques for Games](https://aka.ms/Fo3su4)  
[C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
