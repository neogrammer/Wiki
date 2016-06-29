This class is used for the Xbox One platform to manage video memory allocations.

# Header
    #include <GraphicsMemory.h>

# Initialization
The graphics memory helper is a singleton. It needs explicit initialization because it requires the device and the backbuffer count provided when you set up your swapchain.

    std::unique_ptr<GraphicsMemory> graphicsMemory;
    graphicsMemory = std::make_unique<GraphicsMemory>(device, backBufferCount);

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr``.

# Present
The graphics memory helper manages memory allocation for 'in-flight' data shared between the CPU and GPU. After each frame is rendered, the application needs to call **Commit** to let the manager know that a frame's worth of video memory has been sent to the GPU. This allows the manager to check to see if a previous frame's data is complete and can be released.

    swapChain->Present(...);

    graphicsMemory->Commit();

# Platform Notes
Use of this class is only required for the Xbox One. If you fail to create the singleton, an exception is thrown when using classes that make use of dynamic memory allocation for constant buffers, dynamic index buffers, and dynamic vertex buffers notably all [[Effects]], [[PrimitiveBatch]], and [[SpriteBatch]].

For other Direct3D 11 platforms including Windows desktop, UWP, Windows 8 Store apps, and Xbox One ADK apps the use of this class is entirely optional. If created, the class performs a 'no-op' and does nothing. The ``Allocate`` class will always return nullptr.
