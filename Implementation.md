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

    // SpriteBatch.h public header
    class SpriteBatch
    {
    public:
        SpriteBatch();
        SpriteBatch(SpriteBatch&& moveFrom);
        SpriteBatch& operator= (SpriteBatch&& moveFrom);

        SpriteBatch(SpriteBatchconst&) = delete;
        SpriteBatch& operator=(SpriteBatchconst&) = delete;

        virtual ~SpriteBatch();
        ...
    
    private:
        // Private implementation.
        class Impl;
    
        std::unique_ptr<Impl> pImpl;
    
        // Prevent copying.
        SpriteBatch(SpriteBatch const&) DIRECTX_CTOR_DELETE
        SpriteBatch& operator= (SpriteBatch const&) DIRECTX_CTOR_DELETE
    };

This also allows use to allocate the pImpl class internally using ``_aligned_malloc(x,16);`` so that we can use the DIrectXMath aligned ``XMVECTOR`` and ``XMMATRIX`` types directly in the implementation across all architectures.

# Calling-conventions
The ``std::function`` is used for callbacks as a general pattern so that client code can provide function pointers, lambdas, functors, etc. To support building with a mix of calling conventions, we need to annotate the ``std::function`` correctly.

    HRESULT __cdecl SaveWICTextureToFile( /*...*/,
        std::function<void __cdecl(IPropertyBag2*)> setCustomProps
            = nullptr );

_Note: ``std::function`` doesn't support using ``__vectorcall`` until VS 2015, so use of ``/Gv`` is difficult in VS 2013 or earlier._

# Default constructors/assignment operators
The C++11 standard includes a more efficient ``=default`` and ``=delete`` construct for dealing with default constructors and assignment operators. This is not supported until VS 2013.

For example, to prevent copying we use:

    // Prevent copying.
    SpriteBatch(SpriteBatch const&) = delete;
    SpriteBatch& operator= (SpriteBatch const&) = delete;

_Note that use of ``=default`` and ``=delete`` can improve codegen for derived types._

# DirectXMath Parameter Conventions
The library uses the [DirectXMath](https://msdn.microsoft.com/en-us/library/windows/desktop/ee418728.aspx#Call_Conventions) calling convention types to improve parameter passing of ``XMVECTOR`` and ``XMMATRIX`` types.

# Further Reading
[Dual-use Coding Techniques for Games](http://blogs.msdn.com/b/chuckw/archive/2012/09/17/dual-use-coding-techniques-for-games.aspx)
