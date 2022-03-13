|[[DirectXTK]]|[DirectXTK12](https://github.com/microsoft/DirectXTK12/wiki/DirectXTK)|
|---|---|

# Compiler conformance

For Visual C++, the projects make use of the default C++11/C++14 mode rather than ``/std:c++17`` mode, although occasionally run a validation pass to confirm it's fully compatible with C++17 (i.e. avoid all use of ``auto_ptr``, ``random_shuffle``, and other deprecated features). The library does not make use of newer C++17 language & library features such as ``string_view``, ``static_assert`` without a message, etc. although that may change in the future. The projects make use of ``/Wall``, ``/permissive-``, ``/Zc:__cplusplus``, and ``/analyze`` to ensure a high-level of C++ conformance.

For the few cases (such as ``if constexpr`` or ``[[fallthrough]]``) where it's important to have a C++17 feature used for better code-generation, use a ``#if (__cplusplus >= 201703L)`` guard for now.

For clang/LLVM for Windows, there is a ``CMakeList.txt`` provided to validate the code and ensure a high-level of conformance. This primarily means addressing warnings generated using ``/Wall -Wpedantic -Wextra``.

> A few of the implementation modules and headers support *Windows Subsystem for Linux* via GNUC, so in those cases we leverage C++17 mode to get it to build. For example, the ``std::size`` helper is in Visual C++ 2015 Update 3 or later even in the default C++11/C++14 mode, but for GNUC it's only there with C++17 mode enabled.

> Another example is ``std::aligned_alloc`` which is a C++17 portable function for allocated aligned memory, but it's not supported with Windows. Instead you have to continue to use ``_aligned_malloc`` / ``_aligned_free``. For aligned structures, we use DirectXMath's ``XM_ALIGNED_STRUCT(x)`` macro. In DirectXMath 3.15 or later, this supports Visual C++, clang/LLVM for Windows, and GNUC compilers.

# Naming conventions

While the _DirectX Tool Kit_ design is heavily influenced by the XNA Game Studio framework C# object design, it uses C++ conventions consistent with modern Win32 APIs rather than the strict .NET use of [PascalCase](https://en.wikipedia.org/wiki/CamelCase) as enforced by FXCop.

* PascalCase for class names, methods, functions, and enums.
* camelCase for class member variables, struct members
* UPPERCASE for preprocessor defines (and nameless enums)

The library does not generally make use of [Hungarian notation](https://en.wikipedia.org/wiki/Hungarian_notation) which as been deprecated for Win32 C++ APIs for many years, with the exception of a few uses of ``p`` for pointers and ``sz`` for strings.

# Type usage

The use of Standard C++ types is preferred including the fundamental types supplied by the language (i.e. ``int``, ``unsigned int``, ``size_t``, ``ptrdiff_t``, ``bool``, ``true``/``false``, ``char``, ``wchar_t``) with the addition of the C99 fixed width types (i.e. ``uint32_t``, ``uint64_t``, ``intptr_t``, ``uintptr_t``, etc.)

Avoid using Windows "portability" types except when dealing directly with Win32 APIs: ``VOID``, ``UINT``, ``INT``, ``DWORD``, ``FLOAT``, ``BOOL``, ``TRUE``/``FALSE``, ``WCHAR``, ``CONST``, etc.

# Strongly-typed enum bitmask flags

To provide type-safe bitmask flags, we make use of the ``DEFINE_ENUM_FLAG_OPERATORS`` macro (in winnt.h) to make an enum work as you'd expect for bitmask flags based on recommendations in the [C+ standard](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3485.pdf) under *17.5.2.1.3 Bitmask types*.

The impact to client code is minimal, but mostly means you can't pass a ``0`` literal and must use the defined default value like ``WIC_LOADER_DEFAULT``.

We generally use ``enum`` rather than ``enum class`` because it simplifies testing the bitflags. For example, the following works with ``enum``:

```
if ((myFlags & MYFLAGS_BIT2) != 0)
```

To do the same thing with a ``enum class`` requires:

```
if ((myFlags & MYFLAGS_BIT2) == MYFLAGS_BIT2)
```

Direct3D 11 headers do *not* make use of the strongly type enum bitmask flags. For this reason, if you need a parameter such as ``D3D11_BIND_FLAG`` the  proper type is ``unsigned int``.

> Direct3D 12 headers have adopted strongly typed enum bitmask flags. If you need a parameter such as ``D3D12_RESOURCE_FLAGS`` the proper type is ``D3D12_RESOURCE_FLAGS``.

See [this blog post](https://walbourn.github.io/modern-c++-bitmask-types/).

# Unicode

The Modern C++ recommendation is to use [UTF-8 Everywhere](http://utf8everywhere.org/) except where specifically interacting with Win32 or Windows Runtime APIs which require converting to ``wchar_t`` (UTF-16LE).

* Always explicitly call the "W" or "A" versions of Win32 APIs. Prefer to convert from UTF-8 at the point of call.
* Do not rely on the macros to select "W" vs. "A", but always build with ``UNICODE`` and ``_UNICODE`` defined for safety.

Most functions in _DirectX Tool Kit_ take ``wchar_t*`` since they are passed directly along to Win32 types. [[SpriteFont]] provides both UTF-8 ``char*`` and UTF-16LE ``wchar_t*`` methods.

# Error reporting

*DirectX Tool Kit* uses a mix of Win32-style ``HRESULT`` error codes and C++ exception handling. Since the library is modeled after XNA API designs, it uses C++ exception handling for errors in most places for failures that really should never happen at runtime in a debugged program. *DirectX Tool Kit* also leverages a number of Standard C++ Library (STL) classes that all use C++ exceptions for error handling.

``HRESULT`` return codes are used by **DDSTextureLoader**, **ScreenGrab**, and **WICTextureLoader** since that code is kept in sync for use 'standalone' where exception handling may not be desired. ``HRESULT`` is also used in a few of the helpers in ``BufferHelpers.h`` and ``DirectXHelpers.h`` to simplify cut & paste use in other contexts.

For the remainder of the APIs, they use C++ exception handling leveraging C++11 ``exception``, ``stdexcept`` and ``system_error``. In these cases, the method usually returns ``void``.

* For COM API exceptional failures, we throw a custom ``com_exception`` derived from ``std::exception`` implemented in ``PlatformHelpers.h`` via [[DX::ThrowIfFailed|ThrowIfFailed]].

* For cases where you would use ``E_INVALIDARG`` for ``HRESULT``, we throw ``std::invalid_argument`` with a simple what string.

```
throw std::invalid_argument("Invalid texture for Draw");
```

* For Win32 APIs that return an extended error via ``GetLastError``, we use ``std::system_error``.

```
throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "CreateEventEx");
```

* In a few places where improper use is detected, we use ``std::logic_error``, typically as a safety for ``Begin``/``End`` patterns.

* For a few cases where the error is related to a value being out of range is reported via ``std::out_of_range``.

```
if ((sampleRate < XAUDIO2_MIN_SAMPLE_RATE) || (sampleRate > XAUDIO2_MAX_SAMPLE_RATE))
   throw std::out_of_range("Default sample rate is out of range");
```

* For other exceptional failures, throw ``std::runtime_error``.

> In the past we used ``throw std::exception("what string");`` but this pattern is not portable or conformant: the ctor for ``std::exception`` that takes a *what* string is Microsoft-specific extension. You can use ``throw std::exception();``, but this is generally only used in sample code.

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

    SpriteBatch(SpriteBatch&&) noexcept;
    SpriteBatch& operator= (SpriteBatch&&) noexcept;

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

* The move ctor, move operator, and dtor can be ``=default`` in the ``.cpp`` file as the compiler-generated function bodies are sufficient.

```cpp
SpriteBatch::SpriteBatch(SpriteBatch&&) noexcept = default;
SpriteBatch::SpriteBatch& operator= (SpriteBatch&&) noexcept = default;
SpriteBatch::~SpriteBatch() = default;
```

# Calling-conventions
Public methods in the library are explicitly marked ``__cdecl`` to ensure consistent behavior no matter what the client code is using. Internally it is not specified as it's assumed from the default setting except where ``XM_VECTORCALL`` is utilized (see the _DirectXMath_ section below). Public ``inline`` functions do not need to be explicitly marked ``__cdecl``.

> This generally only matters for 32-bit (x86) platforms which have a potential mix of many calling conventions (``__stdcall``, ``__cdecl``, ``__fastcall``, or ``__vectorcall``). For X64, ARM, and ARM64 they all generally just use one calling convention (``__fastcall``). For 64-bit (x64 native), the ``__cdecl`` is interpreted by the compiler as an alias for ``__fastcall`` so it's the same as annotating them with the default calling convention. It's still important to annotate exported public functions for x64 because it's possible to build the client using ``/Gv`` which assumes ``__vectorcall`` if nothing is specified.

> Note that annotation for the calling-convention only has to be on the function declaration/prototype as the definition will pick it up implicilty.

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

Per [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) recommendations, if you declare a copy constructor/assignment operator, you should also defined the move equivalents:

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
