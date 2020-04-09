//--------------------------------------------------------------------------------------
// File: xwbdump.cpp
//
// XACT3 wave bank file content examination utility
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#include <windows.h>
#include <mmreg.h>

#include <stdio.h>
#include <stdint.h>
#include <memory>

//---------------------------------------------------------------------------------
struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };

using ScopedHandle = std::unique_ptr<void, handle_closer>;

inline HANDLE safe_handle( HANDLE h ) { return (h == INVALID_HANDLE_VALUE) ? nullptr : h; }


//--------------------------------------------------------------------------------------
#pragma pack(push, 1)

#pragma warning( disable : 4201 4203 )

namespace WaveBank
{

static const size_t DVD_SECTOR_SIZE = 2048;
static const size_t DVD_BLOCK_SIZE = DVD_SECTOR_SIZE * 16;

static const size_t ALIGNMENT_MIN = 4;
static const size_t ALIGNMENT_DVD = DVD_SECTOR_SIZE;

static const size_t MAX_DATA_SEGMENT_SIZE = 0xFFFFFFFF;
static const size_t MAX_COMPACT_DATA_SEGMENT_SIZE = 0x001FFFFF;

struct REGION
{
    uint32_t    dwOffset;   // Region offset, in bytes.
    uint32_t    dwLength;   // Region length, in bytes.

    void BigEndian()
    {
        dwOffset = _byteswap_ulong( dwOffset );
        dwLength = _byteswap_ulong( dwLength );
    }
};

struct SAMPLEREGION
{
    uint32_t    dwStartSample;  // Start sample for the region.
    uint32_t    dwTotalSamples; // Region length in samples.

    void BigEndian()
    {
        dwStartSample = _byteswap_ulong( dwStartSample );
        dwTotalSamples = _byteswap_ulong( dwTotalSamples );
    }
};

struct HEADER
{
    static const uint32_t SIGNATURE = 'DNBW';
    static const uint32_t BE_SIGNATURE = 'WBND';
    static const uint32_t VERSION = 44;

    enum SEGIDX
    {
        SEGIDX_BANKDATA = 0,       // Bank data
        SEGIDX_ENTRYMETADATA,      // Entry meta-data
        SEGIDX_SEEKTABLES,         // Storage for seek tables for the encoded waves.
        SEGIDX_ENTRYNAMES,         // Entry friendly names
        SEGIDX_ENTRYWAVEDATA,      // Entry wave data
        SEGIDX_COUNT
    };

    uint32_t    dwSignature;            // File signature
    uint32_t    dwVersion;              // Version of the tool that created the file
    uint32_t    dwHeaderVersion;        // Version of the file format
    REGION      Segments[SEGIDX_COUNT]; // Segment lookup table

    void BigEndian()
    {
        // Leave dwSignature alone as indicator of BE vs. LE

        dwVersion = _byteswap_ulong( dwVersion );
        dwHeaderVersion =_byteswap_ulong( dwHeaderVersion );
        for( size_t j = 0; j < SEGIDX_COUNT; ++j )
        {
            Segments[j].BigEndian();
        }
    }
};

union MINIWAVEFORMAT
{
    static const uint32_t TAG_PCM   = 0x0;
    static const uint32_t TAG_XMA   = 0x1;
    static const uint32_t TAG_ADPCM = 0x2;
    static const uint32_t TAG_WMA   = 0x3;

    static const uint32_t BITDEPTH_8 = 0x0; // PCM only
    static const uint32_t BITDEPTH_16 = 0x1; // PCM only

    static const size_t ADPCM_BLOCKALIGN_CONVERSION_OFFSET = 22;
    
    struct
    {
        uint32_t       wFormatTag      : 2;        // Format tag
        uint32_t       nChannels       : 3;        // Channel count (1 - 6)
        uint32_t       nSamplesPerSec  : 18;       // Sampling rate
        uint32_t       wBlockAlign     : 8;        // Block alignment.  For WMA, lower 6 bits block alignment index, upper 2 bits bytes-per-second index.
        uint32_t       wBitsPerSample  : 1;        // Bits per sample (8 vs. 16, PCM only); WMAudio2/WMAudio3 (for WMA)
    };

    uint32_t           dwValue;

    void BigEndian()
    {
        dwValue = _byteswap_ulong( dwValue );
    }

    WORD BitsPerSample() const
    {
        if (wFormatTag == TAG_XMA)
            return 16; // XMA_OUTPUT_SAMPLE_BITS == 16
        if (wFormatTag == TAG_WMA)
            return 16;
        if (wFormatTag == TAG_ADPCM)
            return 4; // MSADPCM_BITS_PER_SAMPLE == 4

        // wFormatTag must be TAG_PCM (2 bits can only represent 4 different values)
        return (wBitsPerSample == BITDEPTH_16) ? 16 : 8;
    }

    DWORD BlockAlign() const
    {
        switch (wFormatTag)
        {
        case TAG_PCM:
            return wBlockAlign;
            
        case TAG_XMA:
            return (nChannels * 16 / 8); // XMA_OUTPUT_SAMPLE_BITS = 16

        case TAG_ADPCM:
            return (wBlockAlign + ADPCM_BLOCKALIGN_CONVERSION_OFFSET) * nChannels;

        case TAG_WMA:
            {
                static const uint32_t aWMABlockAlign[] =
                {
                    929,
                    1487,
                    1280,
                    2230,
                    8917,
                    8192,
                    4459,
                    5945,
                    2304,
                    1536,
                    1485,
                    1008,
                    2731,
                    4096,
                    6827,
                    5462,
                    1280
                };

                uint32_t dwBlockAlignIndex = wBlockAlign & 0x1F;
                if ( dwBlockAlignIndex < _countof(aWMABlockAlign) )
                    return aWMABlockAlign[dwBlockAlignIndex];
            }
            break;
        }

        return 0;
    }

    DWORD AvgBytesPerSec() const
    {
        switch (wFormatTag)
        {
        case TAG_PCM:
            return nSamplesPerSec * wBlockAlign;
            
        case TAG_XMA:
            return nSamplesPerSec * BlockAlign();

        case TAG_ADPCM:
            {
                uint32_t blockAlign = BlockAlign();
                uint32_t samplesPerAdpcmBlock = AdpcmSamplesPerBlock();
                return blockAlign * nSamplesPerSec / samplesPerAdpcmBlock;
            }
            break;

        case TAG_WMA:
            {
                static const uint32_t aWMAAvgBytesPerSec[] =
                {
                    12000,
                    24000,
                    4000,
                    6000,
                    8000,
                    20000,
                    2500
                };
                // bitrate = entry * 8

                uint32_t dwBytesPerSecIndex = wBlockAlign >> 5;
                if ( dwBytesPerSecIndex < _countof(aWMAAvgBytesPerSec) )
                    return aWMAAvgBytesPerSec[dwBytesPerSecIndex];
            }
            break;
        }

        return 0;
    }

    DWORD AdpcmSamplesPerBlock() const
    {
        uint32_t nBlockAlign = (wBlockAlign + ADPCM_BLOCKALIGN_CONVERSION_OFFSET) * nChannels;
        return nBlockAlign * 2 / (uint32_t)nChannels - 12;
    }

    void AdpcmFillCoefficientTable(ADPCMWAVEFORMAT *fmt) const
    {
        // These are fixed since we are always using MS ADPCM
        fmt->wNumCoef = 7; /* MSADPCM_NUM_COEFFICIENTS */

        static ADPCMCOEFSET aCoef[7] = { { 256, 0}, {512, -256}, {0,0}, {192,64}, {240,0}, {460, -208}, {392,-232} };
        memcpy( &fmt->aCoef, aCoef, sizeof(aCoef) );
    }
};

struct BANKDATA
{
    static const size_t BANKNAME_LENGTH = 64;

    static const uint32_t TYPE_BUFFER = 0x00000000;
    static const uint32_t TYPE_STREAMING = 0x00000001;
    static const uint32_t TYPE_MASK = 0x00000001;

    static const uint32_t FLAGS_ENTRYNAMES = 0x00010000;
    static const uint32_t FLAGS_COMPACT = 0x00020000;
    static const uint32_t FLAGS_SYNC_DISABLED = 0x00040000;
    static const uint32_t FLAGS_SEEKTABLES = 0x00080000;
    static const uint32_t FLAGS_MASK = 0x000F0000;

    uint32_t        dwFlags;                        // Bank flags
    uint32_t        dwEntryCount;                   // Number of entries in the bank
    char            szBankName[BANKNAME_LENGTH];    // Bank friendly name
    uint32_t        dwEntryMetaDataElementSize;     // Size of each entry meta-data element, in bytes
    uint32_t        dwEntryNameElementSize;         // Size of each entry name element, in bytes
    uint32_t        dwAlignment;                    // Entry alignment, in bytes
    MINIWAVEFORMAT  CompactFormat;                  // Format data for compact bank
    FILETIME        BuildTime;                      // Build timestamp

    void BigEndian()
    {
        dwFlags = _byteswap_ulong( dwFlags );
        dwEntryCount = _byteswap_ulong( dwEntryCount );
        dwEntryMetaDataElementSize = _byteswap_ulong( dwEntryMetaDataElementSize );
        dwEntryNameElementSize = _byteswap_ulong( dwEntryNameElementSize );
        dwAlignment = _byteswap_ulong( dwAlignment );
        CompactFormat.BigEndian();
        BuildTime.dwLowDateTime = _byteswap_ulong( BuildTime.dwLowDateTime );
        BuildTime.dwHighDateTime = _byteswap_ulong( BuildTime.dwHighDateTime );
    }
};

struct ENTRY
{
    static const uint32_t FLAGS_READAHEAD = 0x00000001;     // Enable stream read-ahead
    static const uint32_t FLAGS_LOOPCACHE = 0x00000002;     // One or more looping sounds use this wave
    static const uint32_t FLAGS_REMOVELOOPTAIL = 0x00000004;// Remove data after the end of the loop region
    static const uint32_t FLAGS_IGNORELOOP = 0x00000008;    // Used internally when the loop region can't be used
    static const uint32_t FLAGS_MASK = 0x00000008;

    union
    {
        struct
        {
            // Entry flags
            uint32_t                   dwFlags  :  4;

            // Duration of the wave, in units of one sample.
            // For instance, a ten second long wave sampled
            // at 48KHz would have a duration of 480,000.
            // This value is not affected by the number of
            // channels, the number of bits per sample, or the
            // compression format of the wave.
            uint32_t                   Duration : 28;
        };
        uint32_t dwFlagsAndDuration;
    };

    MINIWAVEFORMAT  Format;         // Entry format.
    REGION          PlayRegion;     // Region within the wave data segment that contains this entry.
    SAMPLEREGION    LoopRegion;     // Region within the wave data (in samples) that should loop.

    void BigEndian()
    {
        dwFlagsAndDuration = _byteswap_ulong( dwFlagsAndDuration );
        Format.BigEndian();
        PlayRegion.BigEndian();
        LoopRegion.BigEndian();
    }
};

struct ENTRYCOMPACT
{
    uint32_t       dwOffset            : 21;       // Data offset, in multiplies of the bank alignment
    uint32_t       dwLengthDeviation   : 11;       // Data length deviation, in bytes

    void BigEndian()
    {
        *reinterpret_cast<uint32_t*>( this ) = _byteswap_ulong( *reinterpret_cast<uint32_t*>( this ) );
    }
};

};

#pragma pack(pop)

static_assert( sizeof(WaveBank::REGION)==8, "Mismatch with xact3wb.h" );
static_assert( sizeof(WaveBank::SAMPLEREGION)==8, "Mismatch with xact3wb.h" );
static_assert( sizeof(WaveBank::HEADER)==52, "Mismatch with xact3wb.h" );
static_assert( sizeof(WaveBank::ENTRY)==24, "Mismatch with xact3wb.h" );
static_assert( sizeof(WaveBank::MINIWAVEFORMAT)==4, "Mismatch with xact3wb.h" );
static_assert( sizeof(WaveBank::ENTRY)==24, "Mismatch with xact3wb.h" );
static_assert( sizeof(WaveBank::ENTRYCOMPACT)==4, "Mismatch with xact3wb.h" );
static_assert( sizeof(WaveBank::BANKDATA)==96, "Mismatch with xact3wb.h" );

using namespace WaveBank;

//--------------------------------------------------------------------------------------
uint32_t GetDuration( uint32_t length, const MINIWAVEFORMAT* miniFmt, const uint32_t* seekTable )
{
    switch( miniFmt->wFormatTag )
    {
    case MINIWAVEFORMAT::TAG_ADPCM:
        {
            uint32_t duration = ( length / miniFmt->BlockAlign() ) * miniFmt->AdpcmSamplesPerBlock();
            uint32_t partial = length % miniFmt->BlockAlign();
            if ( partial )
            {
                if ( partial >= ( 7 * miniFmt->nChannels ) )
                    duration += ( partial * 2 / miniFmt->nChannels - 12 );
            }
            return duration;
        }

    case MINIWAVEFORMAT::TAG_WMA:
        if ( seekTable )
        {
            uint32_t seekCount = *seekTable;
            if ( seekCount > 0 )
            {
               return seekTable[ seekCount ] / uint32_t( 2 * miniFmt->nChannels );
            }
        }
        return 0;

    case MINIWAVEFORMAT::TAG_XMA:
        if ( seekTable )
        {
            uint32_t seekCount = *seekTable;
            if ( seekCount > 0 )
            {
               return seekTable[ seekCount ];
            }
        }
        return 0;

    default:
        return ( length * 8 ) / ( miniFmt->BitsPerSample() * miniFmt->nChannels );
    }
}

//--------------------------------------------------------------------------------------
int wmain( int argc, wchar_t *argv[ ], wchar_t *envp[ ]  )
{
    UNREFERENCED_PARAMETER(envp);

    if ( argc < 2 || argc > 2 )
    {
        wprintf(L"Usage: xwbdump <filename.xwb>\n");
        return 0;
    }

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
    ScopedHandle hFile( safe_handle( CreateFile2( argv[1],
                                                  GENERIC_READ,
                                                  FILE_SHARE_READ,
                                                  OPEN_EXISTING,
                                                  nullptr ) ) );
#else
    ScopedHandle hFile( safe_handle( CreateFileW( argv[1],
                                                  GENERIC_READ,
                                                  FILE_SHARE_READ,
                                                  nullptr,
                                                  OPEN_EXISTING,
                                                  FILE_ATTRIBUTE_NORMAL,
                                                  nullptr ) ) );
#endif

    if ( !hFile )
    {
        wprintf( L"ERROR: Failed to open wavebank - %ls\n", argv[1] );
        return 1;
    }

    // Dump Header
    HEADER header;
    DWORD bytes;
    if ( !ReadFile( hFile.get(), &header, sizeof(header), &bytes, nullptr )
         || bytes != sizeof(header) )
    {
        wprintf( L"ERROR: File too small for valid wavebank\n");
        return 1;
    }

    if ( header.dwSignature != HEADER::SIGNATURE && header.dwSignature != HEADER::BE_SIGNATURE )
    {
        wprintf( L"ERROR: File is not a wavebank - %ls\n", argv[1] );
        return 1;
    }

    bool be = ( header.dwSignature == HEADER::BE_SIGNATURE );
    if ( be )
    {
        header.BigEndian();
    }

    if ( header.dwHeaderVersion < HEADER::VERSION )
    {
        wprintf( L"ERROR: Wavebank version is too old %u - %ls\n", header.dwHeaderVersion, argv[1] );
        return 1;
    }

    wprintf( L"WAVEBANK - %ls\n%hs\nHeader: File version %u, Tool version %u\n\tBankData %u, length %u\n\tEntryMetadata %u, length %u\n\tSeekTables %u, length %u\n\tEntryNames %u, length %u\n\tEntryWaveData %u, length %u\n",
           argv[1], (be) ? "BigEndian (Xbox 360 wave bank)" : "LittleEndian (Windows wave bank)", 
           header.dwHeaderVersion, header.dwVersion,
           header.Segments[HEADER::SEGIDX_BANKDATA].dwOffset, header.Segments[HEADER::SEGIDX_BANKDATA].dwLength,
           header.Segments[HEADER::SEGIDX_ENTRYMETADATA].dwOffset, header.Segments[HEADER::SEGIDX_ENTRYMETADATA].dwLength,
           header.Segments[HEADER::SEGIDX_SEEKTABLES].dwOffset, header.Segments[HEADER::SEGIDX_SEEKTABLES].dwLength,
           header.Segments[HEADER::SEGIDX_ENTRYNAMES].dwOffset, header.Segments[HEADER::SEGIDX_ENTRYNAMES].dwLength,
           header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwOffset, header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwLength );

    // Bank Data
    BANKDATA bank;

    if ( SetFilePointer( hFile.get(), static_cast<LONG>( header.Segments[HEADER::SEGIDX_BANKDATA].dwOffset ), 0, SEEK_SET ) == INVALID_SET_FILE_POINTER )
    {
        wprintf( L"ERROR: Failed to seek to bank data %u\n", header.Segments[HEADER::SEGIDX_BANKDATA].dwOffset );
        return 1;
    }

    if ( !ReadFile( hFile.get(), &bank, sizeof(bank), &bytes, nullptr )
         || bytes != sizeof(bank) )
    {
        wprintf( L"ERROR: Failed reading bank data\n");
        return 1;
    }

    if ( be )
        bank.BigEndian();

    wprintf( L"Bank Data:\n\tFlags %08X\n", bank.dwFlags );

    wprintf( L"\t\t%ls\n", ( bank.dwFlags & BANKDATA::TYPE_STREAMING ) ? L"Streaming" : L"In-memory" );

    if ( bank.dwFlags & BANKDATA::FLAGS_ENTRYNAMES)
    {
        wprintf( L"\t\tFLAGS_ENTRYNAMES\n" );
    }
    if ( bank.dwFlags & BANKDATA::FLAGS_COMPACT)
    {
        wprintf( L"\t\tFLAGS_COMPACT\n" );

        if ( !( bank.dwFlags & BANKDATA::TYPE_STREAMING ) )
        {
            wprintf( L"WARNING: XACT only supports streaming with compact wavebanks\n");
        }
    }
    if ( bank.dwFlags & BANKDATA::FLAGS_SYNC_DISABLED)
    {
        wprintf( L"\t\tFLAGS_SYNC_DISABLED\n" );
    }
    if ( bank.dwFlags & BANKDATA::FLAGS_SEEKTABLES)
    {
        wprintf( L"\t\tFLAGS_SEEKTABLES\n" );
    }

    if ( *bank.szBankName )
        wprintf( L"\tName \"%hs\"\n", bank.szBankName );

    wprintf( L"\tEntry metadata size %u\n\tEntry name element size %u\n\tEntry alignment %u\n",
            bank.dwEntryMetaDataElementSize, bank.dwEntryNameElementSize, bank.dwAlignment );

    if ( bank.dwFlags & BANKDATA::FLAGS_COMPACT )
    {
        if ( bank.dwEntryMetaDataElementSize != sizeof(ENTRYCOMPACT) )
        {
            wprintf( L"ERROR: Compact banks expect a metadata element size of %zu\n", sizeof(ENTRYCOMPACT) );
            return 1;
        }
    }
    else
    {
        if ( bank.dwEntryMetaDataElementSize != sizeof(ENTRY) )
        {
            wprintf( L"ERROR: Banks expect a metadata element size of %zu\n", sizeof(ENTRY) );
            return 1;
        }
    }

    if ( ( bank.dwAlignment < ALIGNMENT_MIN ) || ( bank.dwAlignment > ALIGNMENT_DVD ) )
    {
        wprintf( L"WARNING: XACT expects alignment to be in the range %zu...%zu \n", ALIGNMENT_MIN, ALIGNMENT_DVD );
    }

    if ( ( bank.dwFlags & BANKDATA::TYPE_STREAMING ) && ( bank.dwAlignment < DVD_SECTOR_SIZE ) )
    {
        wprintf( L"WARNING: XACT expects streaming buffers to be aligned to DVD sector size\n");
    }

    SYSTEMTIME st;
    if ( FileTimeToSystemTime( &bank.BuildTime, &st ) )
    {
        SYSTEMTIME lst;
        if ( SystemTimeToTzSpecificLocalTime( nullptr, &st, &lst ) )
        {
            wchar_t szLocalDate[256] = {};
            GetDateFormatW( LOCALE_USER_DEFAULT, DATE_LONGDATE, &lst, nullptr, szLocalDate, 256 );

            wchar_t szLocalTime[256] = {};
            GetTimeFormatW( LOCALE_USER_DEFAULT, 0, &lst, nullptr, szLocalTime, 256 );

            wprintf( L"\tBuild time: %ls, %ls\n", szLocalDate, szLocalTime );
        }
    }

    if ( !bank.dwEntryCount )
    {
        wprintf( L"NOTE: Empty wave bank\n");
        return 0;
    }

    // Entry Names
    DWORD metadataBytes = header.Segments[HEADER::SEGIDX_ENTRYMETADATA].dwLength;
    if ( metadataBytes != ( bank.dwEntryMetaDataElementSize * bank.dwEntryCount ) )
    {
        wprintf( L"ERROR: Mismatch in entries %u and metadata size %u\n", bank.dwEntryCount, metadataBytes );
        return 1;
    }

    wprintf( L"%u entries in wave bank:\n", bank.dwEntryCount );

    std::unique_ptr<char[]> entryNames;

    DWORD namesBytes = header.Segments[HEADER::SEGIDX_ENTRYNAMES].dwLength;

    if ( namesBytes > 0 )
    {
        if ( namesBytes != ( bank.dwEntryNameElementSize * bank.dwEntryCount ) )
        {
            wprintf( L"ERROR: Mismatch in entries %u and entry names size %u\n", bank.dwEntryCount, namesBytes );
        }
        else
        {
            entryNames.reset( new char[ namesBytes ] );

            if ( SetFilePointer( hFile.get(), static_cast<LONG>( header.Segments[HEADER::SEGIDX_ENTRYNAMES].dwOffset ), 0, SEEK_SET ) == INVALID_SET_FILE_POINTER )
            {
                wprintf( L"ERROR: Failed to seek to entry names data %u\n", header.Segments[HEADER::SEGIDX_ENTRYNAMES].dwOffset );
                return 1;
            }

            if ( !ReadFile( hFile.get(), entryNames.get(), namesBytes, &bytes, nullptr )
                            || namesBytes != bytes )
            {
                wprintf( L"ERROR: Failed reading entry names\n");
                return 1;
            }
        }
    }

    // Seek tables
    std::unique_ptr<uint32_t[]> seekTables;

    DWORD seekLen = header.Segments[HEADER::SEGIDX_SEEKTABLES].dwLength;
    if ( seekLen > 0 )
    {
        if ( seekLen < ( bank.dwEntryCount * sizeof(uint32_t) ) )
        {
            wprintf( L"ERROR: Seek table is too small, needs at least %zu bytes; only %u bytes\n", bank.dwEntryCount * sizeof(uint32_t), seekLen );
        }
        else if ( ( seekLen % 4 ) != 0 )
        {
            wprintf( L"ERROR: Seek table should be a multiple of 4 in size (%u bytes)\n", seekLen );
        }
        else
        {
            size_t seekCount = seekLen / 4;

            seekTables.reset( new uint32_t[ seekCount ] );

            if ( SetFilePointer( hFile.get(), static_cast<LONG>( header.Segments[HEADER::SEGIDX_SEEKTABLES].dwOffset ), 0, SEEK_SET ) == INVALID_SET_FILE_POINTER )
            {
                wprintf( L"ERROR: Failed to seek to seek tables %u\n", header.Segments[HEADER::SEGIDX_SEEKTABLES].dwOffset );
                return 1;
            }

            if ( !ReadFile( hFile.get(), seekTables.get(), seekLen, &bytes, nullptr )
                            || seekLen != bytes )
            {
                wprintf( L"ERROR: Failed reading seek tables\n");
                return 1;
            }

            if ( be )
            {
                for( size_t j = 0; j < seekCount; ++j )
                {
                    seekTables[ j ] = _byteswap_ulong( seekTables[ j ] );
                }
            }
        }
    }

    // Entries
    std::unique_ptr<uint8_t[]> entries;

    if ( bank.dwFlags & BANKDATA::FLAGS_COMPACT )
    {
        entries.reset( reinterpret_cast<uint8_t*>( new ENTRYCOMPACT[ bank.dwEntryCount ] ) );
    }
    else
    {
        entries.reset( reinterpret_cast<uint8_t*>( new ENTRY[ bank.dwEntryCount ] ) );
    }

    if ( SetFilePointer( hFile.get(), static_cast<LONG>( header.Segments[HEADER::SEGIDX_ENTRYMETADATA].dwOffset ), 0, SEEK_SET ) == INVALID_SET_FILE_POINTER )
    {
        wprintf( L"ERROR: Failed to seek to entry metadata data %u\n", header.Segments[HEADER::SEGIDX_ENTRYMETADATA].dwOffset );
        return 1;
    }

    if ( !ReadFile( hFile.get(), entries.get(), metadataBytes, &bytes, nullptr )
         || metadataBytes != bytes )
    {
        wprintf( L"ERROR: Failed reading entry metadata\n");
        return 1;
    }

    size_t waveBytes = 0;

    size_t waveLen = header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwLength;

    if ( ( bank.dwFlags & BANKDATA::FLAGS_COMPACT ) && ( waveLen > MAX_COMPACT_DATA_SEGMENT_SIZE * bank.dwAlignment ) )
    {
        wprintf( L"ERROR: data segment too large for a valid compact wavebank" );
        return 1;
    }

    bool hasxma = false;
    for( DWORD j=0; j < bank.dwEntryCount; ++j)
    {
        DWORD dwOffset;
        DWORD dwLength;
        DWORD Duration = 0;
        DWORD DurationCompact = 0;
        const MINIWAVEFORMAT* miniFmt;

        uint32_t* seekTable = nullptr;
        if ( seekTables )
        {
            uint32_t baseOffset = bank.dwEntryCount * sizeof(uint32_t);
            uint32_t offset = seekTables[ j ];
            if ( offset != uint32_t(-1) )
            {
                if ( ( baseOffset + offset ) >= seekLen )
                {
                    wprintf( L"ERROR: Invalid seek table offset entry\n" );
                }
                else
                {
                    seekTable = reinterpret_cast<uint32_t*>( reinterpret_cast<uint8_t*>( seekTables.get() ) + baseOffset + offset );

                    if ( ( ( ( *seekTable + 1 ) * sizeof(uint32_t) ) + baseOffset + offset ) > seekLen )
                    {
                        wprintf( L"ERROR: Too many seek table entries for size of seek tables segment\n");
                    }
                }
            }
        }

        if ( bank.dwFlags & BANKDATA::FLAGS_COMPACT )
        {
            auto& entry = reinterpret_cast<ENTRYCOMPACT*>( entries.get() )[j];

            if ( be )
                entry.BigEndian();

            wprintf( L"  Entry %u\n", j );

            miniFmt = &bank.CompactFormat;
            dwOffset = entry.dwOffset * bank.dwAlignment;

            if ( j < ( bank.dwEntryCount - 1 ) )
            {
                dwLength = ( reinterpret_cast<const ENTRYCOMPACT*>( entries.get() )[j + 1].dwOffset * bank.dwAlignment ) - dwOffset - entry.dwLengthDeviation;
            }
            else
            {
                dwLength = static_cast<DWORD>(waveLen - dwOffset - entry.dwLengthDeviation);
            }

            DurationCompact = GetDuration( dwLength, miniFmt, seekTable );
        }
        else
        {
            auto& entry = reinterpret_cast<ENTRY*>( entries.get() )[j];

            if ( be )
                entry.BigEndian();

            wprintf( L"  Entry %u\n\tFlags %08X\n", j, entry.dwFlags );

            if ( entry.dwFlags & ENTRY::FLAGS_READAHEAD )
            {
                wprintf( L"\tFLAGS_READAHEAD\n");
            }
            if ( entry.dwFlags & ENTRY::FLAGS_LOOPCACHE )
            {
                wprintf( L"\tFLAGS_LOOPCACHE\n");
            }
            if ( entry.dwFlags & ENTRY::FLAGS_REMOVELOOPTAIL )
            {
                wprintf( L"\tFLAGS_REMOVELOOPTAIL\n");
            }
            if ( entry.dwFlags & ENTRY::FLAGS_IGNORELOOP )
            {
                wprintf( L"\tFLAGS_IGNORELOOP\n");
            }

            miniFmt = &entry.Format;
            dwOffset = entry.PlayRegion.dwOffset;
            dwLength = entry.PlayRegion.dwLength;
            Duration = entry.Duration;
            DurationCompact = GetDuration( entry.PlayRegion.dwLength, miniFmt, seekTable );
        }

        if ( entryNames )
        {
            DWORD n = bank.dwEntryNameElementSize * j;

            char name[ 64 ] = {};
            strncpy_s( name, &entryNames[ n ], 64 );

            wprintf( L"\t\"%hs\"\n", name );
        }


        if ( bank.dwFlags & BANKDATA::FLAGS_COMPACT )
        {
            float seconds = float( DurationCompact ) / float( miniFmt->nSamplesPerSec );
            wprintf( L"\tEstDuration %u samples (%f seconds)\n", DurationCompact, seconds );
        }
        else
        {
            float seconds = float( Duration ) / float( miniFmt->nSamplesPerSec );
            wprintf( L"\tDuration %u samples (%f seconds), EstDuration %u\n", Duration, seconds, DurationCompact );
        }

        wprintf( L"\tPlay Region %u, Length %u\n", dwOffset, dwLength );

        if ( !( bank.dwFlags & BANKDATA::FLAGS_COMPACT ) )
        {
            auto& entry = reinterpret_cast<const ENTRY*>( entries.get() )[j];

            if ( entry.LoopRegion.dwTotalSamples > 0 )
            {
                wprintf( L"\tLoop Region %u...%u\n", entry.LoopRegion.dwStartSample, entry.LoopRegion.dwTotalSamples );
            }
        }

        const char* fmtstr = nullptr;
        switch( miniFmt->wFormatTag )
        {
        case MINIWAVEFORMAT::TAG_PCM:   fmtstr = "PCM"; break; 
        case MINIWAVEFORMAT::TAG_ADPCM: fmtstr = "MS ADPCM"; break;
        case MINIWAVEFORMAT::TAG_WMA:   fmtstr = "xWMA"; break;
        case MINIWAVEFORMAT::TAG_XMA:   fmtstr = "XMA"; break;
        }

        wprintf( L"\t%hs %u channels, %u-bit, %u Hz\n\tblockAlign %u, avgBytesPerSec %u\n",
               fmtstr,
               miniFmt->nChannels, miniFmt->BitsPerSample(), miniFmt->nSamplesPerSec,
               miniFmt->BlockAlign(), miniFmt->AvgBytesPerSec() );

        if ( !dwLength )
        {
            wprintf( L"ERROR: Entry length is 0\n");
        }

        if ( dwOffset > waveLen
             || (dwOffset+dwLength) > waveLen )
        {
            wprintf( L"ERROR: Invalid wave data region for entry\n");
        }

        if ( ( dwOffset % bank.dwAlignment ) != 0 )
        {
            wprintf( L"ERROR: Entry offset doesn't match alignment\n");
        }

        if ( seekTable )
        {
            wprintf( L"\tSeek table with %u entries", *seekTable );

            for ( uint32_t k = 0; k < *seekTable; ++k )
            {
                if ( ( k % 6 ) == 0 )
                    wprintf( L"\n\t");

                wprintf( L"%u ", seekTable[ k + 1 ] );
            }

            wprintf( L"\n");
        }

        switch( miniFmt->wFormatTag  )
        {
        case MINIWAVEFORMAT::TAG_XMA:
            hasxma = true;
            if ( ( dwOffset % 2048 ) != 0 )
            {
                wprintf( L"ERROR: XMA2 data needs to be aligned to a 2K boundary\n" );
            }

            if ( !seekTable )
            {
                wprintf( L"ERROR: Missing seek table entry for XMA2 wave\n" );
            }
            break;

        case MINIWAVEFORMAT::TAG_WMA:
            if ( !seekTable )
            {
                wprintf( L"ERROR: Missing seek table entry for xWMA wave\n" );
            }
            break;
        }

        waveBytes += dwLength;
    }

    if ( hasxma )
    {
        if ( ( header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwOffset % 2048 ) != 0 )
        {
            wprintf( L"WARNING: Wave banks containing XMA2 data should have the wave segment offset aligned to a 2K boundary\n" );
        }
    }

    wprintf( L"  Total wave bytes %zu\n", waveBytes );

    if ( waveBytes > waveLen )
    {
        wprintf( L"ERROR: Invalid wave data region\n");
    }

    return 0;
}
