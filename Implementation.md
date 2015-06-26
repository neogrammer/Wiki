# Naming conventions

While the _DirectX Tool Kit_ design is heavily influenced by the XNA Game Studio framework C# object design, it uses C++ conventions consistent with modern Win32 APIs rather than the strict .NET use of [PascalCase](https://en.wikipedia.org/wiki/CamelCase) as enforced by FXCop.

* PascalCase for class names, methods, and functions
* camelCase for class member variables and struct members
* UPPERCASE for enums and preprocessor defines

The library does not generally make use of [Hungarian notation](https://en.wikipedia.org/wiki/Hungarian_notation) which as been deprecated for Win32 C++ APIs for many years, with the exception of a few uses of ``p`` for pointers and ``sz`` for strings.

# SAL annotation
The _DirectX Toolkit_ library makes extensive use of SAL2 annotations (``_In_``, ``_Outptr_opt_``, etc.) which greatly improves the accuracy of the Visual C++ static code analysis (also known as PREFAST). The standard Windows headers #define them all to empty strings if not building with ``/analyze``, so they have no effect on code-generation.

# The pImpl idiom
DirectXTK's implementation makes extensive use of the [pImpl idiom](http://en.wikipedia.org/wiki/Opaque_pointer). This keeps the public headers slim and minimizes inter-module dependencies.

    // SpriteBatch.h public header
    class SpriteBatch
    {
    public:
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
The ``std::function`` is used for callbacks as a general pattern so that client code can provide function pointers, lambdas, functors, etc. To support building with a mix of calling conventions, we need to annotate the ``std::function`` correctly. Due to a bug in VS 2010, this doesn't work correctly so we have to rely on defaults.

    #ifndef DIRECTX_STD_CALLCONV
    #if defined(_MSC_VER) && (_MSC_VER < 1700)
    #define DIRECTX_STD_CALLCONV
    #else
    #define DIRECTX_STD_CALLCONV __cdecl
    #endif
    #endif

When declaring these parameters, we use the macro:

    HRESULT __cdecl SaveWICTextureToFile( /*...*/,
        std::function<void DIRECTX_STD_CALLCONV(IPropertyBag2*)> setCustomProps
            = nullptr );

_Note: ``std::function`` doesn't support using ``__vectorcall`` until VS 2015, so use of ``/Gv`` is difficult in VS 2013 or earlier._

# Default constructors/assignment operators
The C++11 standard includes a more efficient ``=default`` and ``=delete`` construct for dealing with default constructors and assignment operators. This is not supported until VS 2013, so we make use of the following macros:

    #ifndef DIRECTX_CTOR_DEFAULT
    #if defined(_MSC_VER) && (_MSC_VER < 1800)
    #define DIRECTX_CTOR_DEFAULT {}
    #define DIRECTX_CTOR_DELETE ;
    #else
    #define DIRECTX_CTOR_DEFAULT =default;
    #define DIRECTX_CTOR_DELETE =delete;
    #endif
    #endif

For example, to prevent copying we use:

    // Prevent copying.
    SpriteBatch(SpriteBatch const&) DIRECTX_CTOR_DELETE
    SpriteBatch& operator= (SpriteBatch const&) DIRECTX_CTOR_DELETE

_Note that use of ``=default`` and ``=delete`` can improve codegen for derived types._

# DirectXMath Parameter Conventions
The library uses the [DirectXMath](https://msdn.microsoft.com/en-us/library/windows/desktop/ee418728.aspx#Call_Conventions) calling convention types to improve parameter passing of ``XMVECTOR`` and ``XMMATRIX`` types.

To support VS 2012 building with the Windows 8.0 SDK which has DirectXMath 3.03, we have the following adapters in headers that require them:

    namespace DirectX
    {
        #if (DIRECTX_MATH_VERSION < 305) && !defined(XM_CALLCONV)
        #define XM_CALLCONV __fastcall
        typedef const XMVECTOR& HXMVECTOR;
        typedef const XMMATRIX& FXMMATRIX;
        #endif
    ...

# Further Reading
[Dual-use Coding Techniques for Games](http://blogs.msdn.com/b/chuckw/archive/2012/09/17/dual-use-coding-techniques-for-games.aspx)
