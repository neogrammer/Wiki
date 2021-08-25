In this lesson we learn how to use multistream rendering to implement GPU instancing.

# Setup
First create a new project using the instructions from the earlier lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Input assembler

For these tutorial lessons, we've been providing a single stream of vertex data to the *input assembler*. Generally the most efficient rendering is single vertex buffer with a stride of 16, 32, or 64 bytes, but there are times when arranging the render data in such a layout is expensive. The Direct3D Input Assembler can therefore pull vertex information from up to 16 (or 32) vertex buffers. This provides a lot of freedom in managing your vertex buffers.

For example, if we return to a case from [[Simple rendering]], here is the 'stock' vertex input layout for ``VertexPositionNormalTexture``:

```cpp
const D3D11_INPUT_ELEMENT_DESC c_InputElements[] =
{
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```

This describes a single vertex stream with three elements. We could arrange this into three VBs as follows:

```cpp
// Position in VB#0, NORMAL in VB#1, TEXCOORD in VB#2
const D3D11_INPUT_ELEMENT_DESC c_InputElements[] =
{
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```

To render, we'd need to create an InputLayout object using this with our vertex shader, and then bind the vertex buffers to each slot:

```cpp
ID3D11Buffer* vbs[3] = { m_positionVB.Get(), m_normalVB.Get(), m_texcoordVB.Get() };
UINT strides[3] = { sizeof(float) * 3, sizeof(float) * 3, sizeof(float) * 2 };
UINT offsets[3] = {};
context->IASetVertexBuffers(0, 3, vbs, strides, offsets);
```

Note if we are using ``DrawIndexed``, then the same index value is used to retrieve the 'ith' element from each vertex buffer (i.e. there is only one index per vertex, and all VBs must be at least as long as the highest index value).

# Instancing


> **UNDER CONSTRUCTION**

**Next lessons:** [[Creating custom shaders with DGSL]]

# Further reading

