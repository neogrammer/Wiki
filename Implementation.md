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

# Compiler versions
_DirectX Tool Kit_ supports a number of different versions of Visual C++. This requires some  macro tricks in the code.

## Calling-conventions
The ``std::function`` is used for callbacks as a general pattern so that client code can provide function pointers, lambdas, functors, etc. To support building with a mix of calling conventions, we need to annotate the ``std::function`` correctly. Due to a bug in VS 2010, this doesn't work correctly so we have to rely on defaults.

    #ifndef DIRECTX_STD_CALLCONV
    #if defined(_MSC_VER) && (_MSC_VER < 1700)
    #define DIRECTX_STD_CALLCONV
    #else
    #define DIRECTX_STD_CALLCONV __cdecl
    #endif
    #endif

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

# Further Reading
[Dual-use Coding Techniques for Games](http://blogs.msdn.com/b/chuckw/archive/2012/09/17/dual-use-coding-techniques-for-games.aspx)
