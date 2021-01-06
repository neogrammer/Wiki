//--------------------------------------------------------------------------------------
// File: spritefontdump.cpp
//
// DirectX Tool Kit .spritefont file content examination utility
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#include <Windows.h>
#include <dxgiformat.h>

#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <memory>
#include <vector>

#include "DDS.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#pragma warning(disable : 4061)

using namespace DirectX;

//---------------------------------------------------------------------------------
struct handle_closer { void operator()(HANDLE h) noexcept { if (h) CloseHandle(h); } };

using ScopedHandle = std::unique_ptr<void, handle_closer>;

inline HANDLE safe_handle(HANDLE h) noexcept { return (h == INVALID_HANDLE_VALUE) ? nullptr : h; }

//--------------------------------------------------------------------------------------
namespace
{
    const char s_spriteFontMagic[] = "DXTKfont";

    struct Glyph
    {
        uint32_t Character;
        RECT Subrect;
        float XOffset;
        float YOffset;
        float XAdvance;
    };

    struct Metadata
    {
        float lineSpacing;
        uint32_t defaultCharacter;
        uint32_t textureWidth;
        uint32_t textureHeight;
        DXGI_FORMAT textureFormat;
        uint32_t textureStride;
        uint32_t textureRows;
    };

    HRESULT LoadSpriteFont(
        _In_z_ const wchar_t* fileName,
        std::unique_ptr<uint8_t[]>& fileData,
        size_t& fileSize)
    {
        // open the file
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
        ScopedHandle hFile(safe_handle(CreateFile2(fileName,
            GENERIC_READ,
            FILE_SHARE_READ,
            OPEN_EXISTING,
            nullptr)));
#else
        ScopedHandle hFile(safe_handle(CreateFileW(fileName,
            GENERIC_READ,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr)));
#endif
        if (!hFile)
            return HRESULT_FROM_WIN32(GetLastError());

        // Get the file size
        FILE_STANDARD_INFO fileInfo;
        if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        // File is too big for 32-bit allocation, so reject read
        if (fileInfo.EndOfFile.HighPart > 0)
        {
            return E_FAIL;
        }

        // Need at least enough data to fill the magic values, glyph count, spacing, default character, texture metadata
        if (fileInfo.EndOfFile.LowPart < (sizeof(DWORD) * 10))
        {
            return E_FAIL;
        }

        // create enough space for the file data
        fileData.reset(new uint8_t[fileInfo.EndOfFile.LowPart]);
        if (!fileData)
        {
            return E_OUTOFMEMORY;
        }

        // read the data in
        DWORD BytesRead = 0;
        if (!ReadFile(hFile.get(), fileData.get(), fileInfo.EndOfFile.LowPart, &BytesRead, nullptr ))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        if (BytesRead < fileInfo.EndOfFile.LowPart)
        {
            return E_FAIL;
        }

        // spritefont files always start with the same magic values
        auto dwMagicNumber1 = *reinterpret_cast<const uint32_t*>(fileData.get());
        auto dwMagicNumber2 = *reinterpret_cast<const uint32_t*>(fileData.get() + sizeof(uint32_t));
        auto expectedMagicNumber1 = *reinterpret_cast<const uint32_t*>(s_spriteFontMagic);
        auto expectedMagicNumber2 = *reinterpret_cast<const uint32_t*>(&s_spriteFontMagic[sizeof(uint32_t)]);

        if (dwMagicNumber1 != expectedMagicNumber1
            || dwMagicNumber2 != expectedMagicNumber2)
        {
            return E_FAIL;
        }

        fileSize = static_cast<size_t>(fileInfo.EndOfFile.LowPart);

        return S_OK;
    }

 #define ENUM_CASE(x) case x: wprintf( L## #x ); break;

    void PrintDxgiFormat(DXGI_FORMAT format)
    {
        switch (static_cast<int>(format))
        {
        ENUM_CASE(DXGI_FORMAT_R32G32B32A32_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R32G32B32A32_FLOAT)
        ENUM_CASE(DXGI_FORMAT_R32G32B32A32_UINT)
        ENUM_CASE(DXGI_FORMAT_R32G32B32A32_SINT)
        ENUM_CASE(DXGI_FORMAT_R32G32B32_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R32G32B32_FLOAT)
        ENUM_CASE(DXGI_FORMAT_R32G32B32_UINT)
        ENUM_CASE(DXGI_FORMAT_R32G32B32_SINT)
        ENUM_CASE(DXGI_FORMAT_R16G16B16A16_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R16G16B16A16_FLOAT)
        ENUM_CASE(DXGI_FORMAT_R16G16B16A16_UNORM)
        ENUM_CASE(DXGI_FORMAT_R16G16B16A16_UINT)
        ENUM_CASE(DXGI_FORMAT_R16G16B16A16_SNORM)
        ENUM_CASE(DXGI_FORMAT_R16G16B16A16_SINT)
        ENUM_CASE(DXGI_FORMAT_R32G32_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R32G32_FLOAT)
        ENUM_CASE(DXGI_FORMAT_R32G32_UINT)
        ENUM_CASE(DXGI_FORMAT_R32G32_SINT)
        ENUM_CASE(DXGI_FORMAT_R32G8X24_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
        ENUM_CASE(DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_X32_TYPELESS_G8X24_UINT)
        ENUM_CASE(DXGI_FORMAT_R10G10B10A2_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R10G10B10A2_UNORM)
        ENUM_CASE(DXGI_FORMAT_R10G10B10A2_UINT)
        ENUM_CASE(DXGI_FORMAT_R11G11B10_FLOAT)
        ENUM_CASE(DXGI_FORMAT_R8G8B8A8_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R8G8B8A8_UNORM)
        ENUM_CASE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
        ENUM_CASE(DXGI_FORMAT_R8G8B8A8_UINT)
        ENUM_CASE(DXGI_FORMAT_R8G8B8A8_SNORM)
        ENUM_CASE(DXGI_FORMAT_R8G8B8A8_SINT)
        ENUM_CASE(DXGI_FORMAT_R16G16_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R16G16_FLOAT)
        ENUM_CASE(DXGI_FORMAT_R16G16_UNORM)
        ENUM_CASE(DXGI_FORMAT_R16G16_UINT)
        ENUM_CASE(DXGI_FORMAT_R16G16_SNORM)
        ENUM_CASE(DXGI_FORMAT_R16G16_SINT)
        ENUM_CASE(DXGI_FORMAT_R32_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_D32_FLOAT)
        ENUM_CASE(DXGI_FORMAT_R32_FLOAT)
        ENUM_CASE(DXGI_FORMAT_R32_UINT)
        ENUM_CASE(DXGI_FORMAT_R32_SINT)
        ENUM_CASE(DXGI_FORMAT_R24G8_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_D24_UNORM_S8_UINT)
        ENUM_CASE(DXGI_FORMAT_R24_UNORM_X8_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_X24_TYPELESS_G8_UINT)
        ENUM_CASE(DXGI_FORMAT_R8G8_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R8G8_UNORM)
        ENUM_CASE(DXGI_FORMAT_R8G8_UINT)
        ENUM_CASE(DXGI_FORMAT_R8G8_SNORM)
        ENUM_CASE(DXGI_FORMAT_R8G8_SINT)
        ENUM_CASE(DXGI_FORMAT_R16_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R16_FLOAT)
        ENUM_CASE(DXGI_FORMAT_D16_UNORM)
        ENUM_CASE(DXGI_FORMAT_R16_UNORM)
        ENUM_CASE(DXGI_FORMAT_R16_UINT)
        ENUM_CASE(DXGI_FORMAT_R16_SNORM)
        ENUM_CASE(DXGI_FORMAT_R16_SINT)
        ENUM_CASE(DXGI_FORMAT_R8_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_R8_UNORM)
        ENUM_CASE(DXGI_FORMAT_R8_UINT)
        ENUM_CASE(DXGI_FORMAT_R8_SNORM)
        ENUM_CASE(DXGI_FORMAT_R8_SINT)
        ENUM_CASE(DXGI_FORMAT_A8_UNORM)
        ENUM_CASE(DXGI_FORMAT_R1_UNORM)
        ENUM_CASE(DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
        ENUM_CASE(DXGI_FORMAT_R8G8_B8G8_UNORM)
        ENUM_CASE(DXGI_FORMAT_G8R8_G8B8_UNORM)
        ENUM_CASE(DXGI_FORMAT_BC1_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_BC1_UNORM)
        ENUM_CASE(DXGI_FORMAT_BC1_UNORM_SRGB)
        ENUM_CASE(DXGI_FORMAT_BC2_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_BC2_UNORM)
        ENUM_CASE(DXGI_FORMAT_BC2_UNORM_SRGB)
        ENUM_CASE(DXGI_FORMAT_BC3_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_BC3_UNORM)
        ENUM_CASE(DXGI_FORMAT_BC3_UNORM_SRGB)
        ENUM_CASE(DXGI_FORMAT_BC4_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_BC4_UNORM)
        ENUM_CASE(DXGI_FORMAT_BC4_SNORM)
        ENUM_CASE(DXGI_FORMAT_BC5_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_BC5_UNORM)
        ENUM_CASE(DXGI_FORMAT_BC5_SNORM)
        ENUM_CASE(DXGI_FORMAT_B5G6R5_UNORM)
        ENUM_CASE(DXGI_FORMAT_B5G5R5A1_UNORM)
        ENUM_CASE(DXGI_FORMAT_B8G8R8A8_UNORM)
        ENUM_CASE(DXGI_FORMAT_B8G8R8X8_UNORM)
        ENUM_CASE(DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
        ENUM_CASE(DXGI_FORMAT_B8G8R8A8_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
        ENUM_CASE(DXGI_FORMAT_B8G8R8X8_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_B8G8R8X8_UNORM_SRGB)
        ENUM_CASE(DXGI_FORMAT_BC6H_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_BC6H_UF16)
        ENUM_CASE(DXGI_FORMAT_BC6H_SF16)
        ENUM_CASE(DXGI_FORMAT_BC7_TYPELESS)
        ENUM_CASE(DXGI_FORMAT_BC7_UNORM)
        ENUM_CASE(DXGI_FORMAT_BC7_UNORM_SRGB)
        ENUM_CASE(DXGI_FORMAT_AYUV)
        ENUM_CASE(DXGI_FORMAT_Y410)
        ENUM_CASE(DXGI_FORMAT_Y416)
        ENUM_CASE(DXGI_FORMAT_NV12)
        ENUM_CASE(DXGI_FORMAT_P010)
        ENUM_CASE(DXGI_FORMAT_P016)
        ENUM_CASE(DXGI_FORMAT_420_OPAQUE)
        ENUM_CASE(DXGI_FORMAT_YUY2)
        ENUM_CASE(DXGI_FORMAT_Y210)
        ENUM_CASE(DXGI_FORMAT_Y216)
        ENUM_CASE(DXGI_FORMAT_NV11)
        ENUM_CASE(DXGI_FORMAT_AI44)
        ENUM_CASE(DXGI_FORMAT_IA44)
        ENUM_CASE(DXGI_FORMAT_P8)
        ENUM_CASE(DXGI_FORMAT_A8P8)
        ENUM_CASE(DXGI_FORMAT_B4G4R4A4_UNORM)
        case 116: wprintf(L"DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT [Xbox]"); break;
        case 117: wprintf(L"DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT [Xbox]"); break;
        case 118: wprintf(L"DXGI_FORMAT_D16_UNORM_S8_UINT [Xbox]"); break;
        case 119: wprintf(L"DXGI_FORMAT_R16_UNORM_X8_TYPELESS [Xbox]"); break;
        case 120: wprintf(L"DXGI_FORMAT_X16_TYPELESS_G8_UINT [Xbox]"); break;
        case 130: wprintf(L"DXGI_FORMAT_P208"); break;
        case 131: wprintf(L"DXGI_FORMAT_V208"); break;
        case 132: wprintf(L"DXGI_FORMAT_V408"); break;
        case 189: wprintf(L"DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM [Xbox]"); break;
        case 190: wprintf(L"DXGI_FORMAT_R4G4_UNORM [Xbox]"); break;

        default:
            wprintf(L"UNKNOWN");
            break;
        }
    }

    //-------------------------------------------------------------------------------------
    HRESULT SaveDDS(_In_z_ const wchar_t* szFile, const Metadata& metaData, const uint8_t* pixelData )
    {
        if (!szFile || !pixelData)
            return E_INVALIDARG;

        bool bc = false;
        bool ispmalpha = true; // spritefont is pm alpha by default
        DDS_PIXELFORMAT ddpf = {};
        switch (metaData.textureFormat)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:        memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_A8B8G8R8, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_R16G16_UNORM:          memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_G16R16, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_R8G8_UNORM:            memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_A8L8, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_R16_UNORM:             memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_L16, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_R8_UNORM:              memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_L8, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_A8_UNORM:              memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_A8, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_R8G8_B8G8_UNORM:       memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_R8G8_B8G8, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_G8R8_G8B8_UNORM:       memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_G8R8_G8B8, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_BC1_UNORM:             memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_DXT1, sizeof(DDS_PIXELFORMAT)); bc = true;  break;
        case DXGI_FORMAT_BC2_UNORM:             memcpy_s(&ddpf, sizeof(ddpf), ispmalpha ? (&DDSPF_DXT2) : (&DDSPF_DXT3), sizeof(DDS_PIXELFORMAT)); bc = true;  break;
        case DXGI_FORMAT_BC3_UNORM:             memcpy_s(&ddpf, sizeof(ddpf), ispmalpha ? (&DDSPF_DXT4) : (&DDSPF_DXT5), sizeof(DDS_PIXELFORMAT)); bc = true;  break;
        case DXGI_FORMAT_BC4_UNORM:             memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_BC4_UNORM, sizeof(DDS_PIXELFORMAT)); bc = true;  break;
        case DXGI_FORMAT_BC4_SNORM:             memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_BC4_SNORM, sizeof(DDS_PIXELFORMAT)); bc = true;  break;
        case DXGI_FORMAT_BC5_UNORM:             memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_BC5_UNORM, sizeof(DDS_PIXELFORMAT)); bc = true;  break;
        case DXGI_FORMAT_BC5_SNORM:             memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_BC5_SNORM, sizeof(DDS_PIXELFORMAT)); bc = true;  break;
        case DXGI_FORMAT_B5G6R5_UNORM:          memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_R5G6B5, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_B5G5R5A1_UNORM:        memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_A1R5G5B5, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_R8G8_SNORM:            memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_V8U8, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_R8G8B8A8_SNORM:        memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_Q8W8V8U8, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_R16G16_SNORM:          memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_V16U16, sizeof(DDS_PIXELFORMAT)); break;
        case DXGI_FORMAT_B8G8R8A8_UNORM:        memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_A8R8G8B8, sizeof(DDS_PIXELFORMAT)); break; // DXGI 1.1
        case DXGI_FORMAT_B8G8R8X8_UNORM:        memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_X8R8G8B8, sizeof(DDS_PIXELFORMAT)); break; // DXGI 1.1
        case DXGI_FORMAT_B4G4R4A4_UNORM:        memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_A4R4G4B4, sizeof(DDS_PIXELFORMAT)); break; // DXGI 1.2
        case DXGI_FORMAT_YUY2:                  memcpy_s(&ddpf, sizeof(ddpf), &DDSPF_YUY2, sizeof(DDS_PIXELFORMAT)); break; // DXGI 1.2

        // Legacy D3DX formats using D3DFMT enum value as FourCC
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            ddpf.size = sizeof(DDS_PIXELFORMAT); ddpf.flags = DDS_FOURCC; ddpf.fourCC = 116;  // D3DFMT_A32B32G32R32F
            break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            ddpf.size = sizeof(DDS_PIXELFORMAT); ddpf.flags = DDS_FOURCC; ddpf.fourCC = 113;  // D3DFMT_A16B16G16R16F
            break;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            ddpf.size = sizeof(DDS_PIXELFORMAT); ddpf.flags = DDS_FOURCC; ddpf.fourCC = 36;  // D3DFMT_A16B16G16R16
            break;
        case DXGI_FORMAT_R16G16B16A16_SNORM:
            ddpf.size = sizeof(DDS_PIXELFORMAT); ddpf.flags = DDS_FOURCC; ddpf.fourCC = 110;  // D3DFMT_Q16W16V16U16
            break;
        case DXGI_FORMAT_R32G32_FLOAT:
            ddpf.size = sizeof(DDS_PIXELFORMAT); ddpf.flags = DDS_FOURCC; ddpf.fourCC = 115;  // D3DFMT_G32R32F
            break;
        case DXGI_FORMAT_R16G16_FLOAT:
            ddpf.size = sizeof(DDS_PIXELFORMAT); ddpf.flags = DDS_FOURCC; ddpf.fourCC = 112;  // D3DFMT_G16R16F
            break;
        case DXGI_FORMAT_R32_FLOAT:
            ddpf.size = sizeof(DDS_PIXELFORMAT); ddpf.flags = DDS_FOURCC; ddpf.fourCC = 114;  // D3DFMT_R32F
            break;
        case DXGI_FORMAT_R16_FLOAT:
            ddpf.size = sizeof(DDS_PIXELFORMAT); ddpf.flags = DDS_FOURCC; ddpf.fourCC = 111;  // D3DFMT_R16F
            break;

        default:
            break;
        }

        // Create DDS Header
        DDS_HEADER header = {};
        header.size = sizeof(DDS_HEADER);
        header.flags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP;
        header.caps = DDS_SURFACE_FLAGS_TEXTURE;
        header.mipMapCount = 1;
        header.width = metaData.textureWidth;
        header.height = metaData.textureHeight;
        header.depth = 1;

        if (bc)
        {
            header.pitchOrLinearSize = metaData.textureStride * metaData.textureRows;
        }
        else
        {
            header.pitchOrLinearSize = metaData.textureStride;
        }

        memcpy_s(&header.ddspf, sizeof(header.ddspf), &ddpf, sizeof(ddpf));

        // Create file and write header
        ScopedHandle hFile(safe_handle(CreateFileW(szFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr)));
        if (!hFile)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        DWORD bytesWritten;
        uint32_t magic = DDS_MAGIC;
        if (!WriteFile(hFile.get(), &magic, sizeof(magic), &bytesWritten, nullptr))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        if (bytesWritten != sizeof(magic))
        {
            return E_FAIL;
        }

        if (!WriteFile(hFile.get(), &header, sizeof(DDS_HEADER), &bytesWritten, nullptr))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        if (bytesWritten != sizeof(DDS_HEADER))
        {
            return E_FAIL;
        }

        uint32_t textureBytes = metaData.textureStride * metaData.textureRows;
        if (!WriteFile(hFile.get(), pixelData, textureBytes, &bytesWritten, nullptr))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        if (bytesWritten != textureBytes)
        {
            return E_FAIL;
        }

        return S_OK;
    }
}

int __cdecl wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
    UNREFERENCED_PARAMETER(envp);
    if (argc != 2)
    {
        wprintf(L"Usage: %ls [filename]\n", argv[0]);
        return -1;
    }

    std::unique_ptr<uint8_t[]> fileData;
    size_t fileSize = 0;
    HRESULT hr = LoadSpriteFont(argv[1], fileData, fileSize);
    if (FAILED(hr))
    {
        wprintf(L"ERROR: failed to load %ls\n", argv[1]);
        return -1;
    }

    auto ptr = fileData.get() + sizeof(uint32_t) * 2;

    auto glyphCount = reinterpret_cast<const uint32_t*>(ptr);

    if (!*glyphCount)
    {
        wprintf(L"ERROR: Contains 0 glyphs\n");
        return -1;
    }

    fileSize -= sizeof(uint32_t) * 3;
    ptr += sizeof(uint32_t);

    wprintf(L"%ls contains %u glyphs\n", argv[1], *glyphCount);

    if (fileSize < sizeof(Glyph) * *glyphCount)
    {
        wprintf(L"ERROR: Unexpected end of file reading glyph metadata\n");
        return -1;
    }

    std::vector<Glyph> m_glyphs;
    m_glyphs.resize(*glyphCount);
    memcpy(m_glyphs.data(), ptr, sizeof(Glyph) * *glyphCount);

    fileSize -= sizeof(Glyph) * *glyphCount;
    ptr += sizeof(Glyph) * *glyphCount;

    auto metaData = reinterpret_cast<const Metadata*>(ptr);

    if (fileSize < sizeof(Metadata))
    {
        wprintf(L"ERROR: Unexpected end of file reading texture metadata\n");
        return -1;
    }

    fileSize -= sizeof(Metadata);
    ptr += sizeof(Metadata);

    wprintf(L"Characters: ");

    for (auto it : m_glyphs)
    {
        wprintf(L"%u ", it.Character);
    }
    wprintf(L"\n");

    wprintf(L"Line spacing: %f\n", double(metaData->lineSpacing));

    wprintf(L"Default character: %u\n", metaData->defaultCharacter);

    wprintf(L"Sprite texture: %u by %u\nFormat: ", metaData->textureWidth, metaData->textureHeight);
    PrintDxgiFormat(metaData->textureFormat);
    wprintf(L"\n");

    if (fileSize < metaData->textureStride * metaData->textureRows)
    {
        wprintf(L"ERROR: Unexpected end of file reading texture\n");
        return -1;
    }

    // Form dest path
    wchar_t ext[_MAX_EXT] = {};
    wchar_t fname[_MAX_FNAME] = {};
    _wsplitpath_s(argv[1], nullptr, 0, nullptr, 0, fname, _MAX_FNAME, ext, _MAX_EXT);

    wchar_t ddsFileName[MAX_PATH] = {};
    _wmakepath_s(ddsFileName, MAX_PATH, nullptr, nullptr, fname, L".dds");

    if (FAILED(SaveDDS(ddsFileName, *metaData, ptr)))
    {
        wprintf(L"ERROR: Failed writing dds texure %ls\n", ddsFileName);
        return -1;
    }

    wprintf(L"Wrote texture as %ls\n", ddsFileName);

    return 0;
}
