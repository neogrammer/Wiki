# Naming conventions

While the _DirectX Tool Kit_ design is heavily influenced by the XNA Game Studio framework C# object design, it uses C++ conventions consistent with modern Win32 APIs rather than the strict .NET use of [PascalCase](https://en.wikipedia.org/wiki/CamelCase) as enforced by FXCop.

* PascalCase for class names, methods, functions, and enums.
* camelCase for class member variables, struct members
* UPPERCASE for preprocessor defines (and nameless enums)

The library does not generally make use of [Hungarian notation](https://en.wikipedia.org/wiki/Hungarian_notation) which as been deprecated for Win32 C++ APIs for many years, with the exception of a few uses of ``p`` for pointers and ``sz`` for strings.

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

    SpriteBatch(SpriteBatch&& moveFrom);
    SpriteBatch& operator= (SpriteBatch&& moveFrom);

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
The ``std::function`` is used for callbacks as a general pattern so that client code can provide function pointers, lambdas, functors, etc. To support building with a mix of calling conventions, we need to annotate the ``std::function`` correctly.

```cpp
    HRESULT __cdecl SaveWICTextureToFile( /*...*/,
        std::function<void __cdecl(IPropertyBag2*)> setCustomProps
            = nullptr );
```            

# Default constructors/assignment operators
The C++11 standard includes a more efficient ``=default`` and ``=delete`` construct for dealing with special constructors and operators.

To declare a standard copy constructor and copy assignment operators, we use:

```cpp
Rectangle(const Rectangle&) = default;
Rectangle& operator=(const Rectangle&) = default;
```

To prevent copying we use:

```cpp
// Prevent copying.
SpriteBatch(SpriteBatch const&) = delete;
SpriteBatch& operator= (SpriteBatch const&) = delete;
```

> Note that use of ``=default`` can improve codegen for derived types as well.

# DirectXMath Parameter Conventions
The library uses the [DirectXMath](https://docs.microsoft.com/en-us/windows/desktop/dxmath/pg-xnamath-internals#Call_Conventions) calling convention types to improve parameter passing of ``XMVECTOR`` and ``XMMATRIX`` types.

# Further Reading
[Dual-use Coding Techniques for Games](https://aka.ms/Fo3su4)  
[C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
