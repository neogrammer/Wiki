Audio formats are described by the ``WAVEFORMATEX`` structure, but the polymorphic nature of this format encompasses several other structures as well.

# WAVEFORMATEX

The 'generic' structure of a ``WAVEFORMATEX`` is as follows (18 bytes), and is the modern header structure used to encode audio data. This header can be interpreted as other structures based on the ``wFormatTag`` value, and can encode variable-length header extensions.

```cpp
WORD  wFormatTag;
WORD  nChannels;
DWORD nSamplesPerSec;
DWORD nAvgBytesPerSec;
WORD  nBlockAlign;
WORD  wBitsPerSample;
WORD  cbSize;
```

XAudio2 supports ``WAVE_FORMAT_PCM``, ``WAVE_FORMAT_IEEE_FLOAT``, ``WAVE_FORMAT_ADPCM``, ``WAVE_FORMAT_WMAUDIO2``, and ``WAVE_FORMAT_WMAUDIO3``. On Xbox One, it also supports ``WAVE_FORMAT_XMA2``.

# WAVEFORMAT

The simplest kind of wave format is a partial version of ``WAVEFORMATEX`` (it's only 14 instead of 18 bytes) that was the original polymorphic header for audio. This structure is largely considered deprecated in favor of ``WAVEFORMATEX``, and is only used as part of the structure definition for some older formats.

```cpp
WORD  wFormatTag;
WORD  nChannels;
DWORD nSamplesPerSec;
DWORD nAvgBytesPerSec;
WORD  nBlockAlign;
```

# PCMWAVEFORMAT

The next simplest form is another partial ``WAVEFORMATEX`` version using an older structure (it's only 16 bytes instead of 18 bytes). This is commonly used to encode [PCM](https://wikipedia.org/wiki/Pulse-code_modulation) format data using ``WAVE_FORMAT_PCM`` or ``WAVE_FORMAT_IEEE_FLOAT``, although it's recommended you use a complete ``WAVEFORMATEX`` instead.

```cpp
WAVEFORMAT wf;
WORD       wBitsPerSample;
```

XAudio2 only supports ``wBitsPerSample`` values of 8, 16, 24, or 32 for WAVE_FORMAT_PCM.

XAudio2 only supports a ``wBitsPerSample`` of 32 for WAVE_FORMAT_IEEE_FLOAT.

> Generally for handling legacy ``.wav`` files, you allow the header format structure to be only 16 bytes long for the ``WAVE_FORMAT_PCM`` or ``WAVE_FORMAT_IEEE_FLOAT`` tags and just assume ``cbSize`` is 0.

# ADPCMWAVEFORMAT

A common compressed encoding for audio data is [ADPCM](https://wikipedia.org/wiki/Adaptive_differential_pulse-code_modulation) which is indicated with ``WAVE_FORMAT_ADPCM``.

```cpp
WAVEFORMATEX    wfx;
WORD            wSamplesPerBlock;
WORD            wNumCoef;
ADPCMCOEFSET    aCoef[];
```

XAudio2 only supports Microsoft ADPCM which specifically contains 32 extra bytes (``MSADPCM_FORMAT_EXTRA_BYTES``) in the ``wfx.cbSize``. It has a ``wSamplesPerBlock`` of 4 (``MSADPCM_BITS_PER_SAMPLE``), and uses 7 (``MSADPCM_NUM_COEFFICIENTS``) fixed coefficient pairs:

    { 256, 0 }, { 512, -256 }, { 0, 0 }, { 192, 64 }, { 240, 0 }, { 460, -208 }, { 392, -232 }

# WAVEFORMATEXTENSIBLE

A more complicated version of ``WAVEFORMATEX`` is this extensible structure indicated by ``WAVE_FORMAT_EXTENSIBLE`` with a ``cbSize`` of 22 for a total of 40 bytes. It encodes the format as a GUID allowing for custom audio formats to be easily encoded.

```cpp
WAVEFORMATEX    Format;
union {
    WORD wValidBitsPerSample;
    WORD wSamplesPerBlock;
    WORD wReserved;
} Samples;
DWORD           dwChannelMask;
GUID            SubFormat;
```

The most common use of this format is for encoding multi-channel audio since it provides an explicit ``dwChannelMask``. The standard ``WAVE_FORMAT_`` tags are encoded in a ``WAVEFORMATEXTENSIBLE`` using 'well-known' GUIDs of the following form:

    { formatTag, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 }

Here's example code for finding the standard format tag that handles ``WAVEFORMATEXTENSIBLE``:

```cpp
inline uint32_t GetFormatTag(const WAVEFORMATEX* wfx)
{
    if (wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        if (wfx->cbSize < (sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)))
            return 0;

        static const GUID s_wfexBase = { 0x00000000, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };

        auto wfex = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(wfx);

        if (memcmp(reinterpret_cast<const BYTE*>(&wfex->SubFormat) + sizeof(DWORD),
            reinterpret_cast<const BYTE*>(&s_wfexBase) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD)) != 0)
        {
            return 0;
        }

        return wfex->SubFormat.Data1;
    }
    else
    {
        return wfx->wFormatTag;
    }
}
```

# XMA2WAVEFORMATEX

The Xbox One supports a custom hardware-compressed audio format known as XMA and is indicated using a custom variant of ``WAVEFORMATEX`` with the ``WAVE_FORMAT_XMA2`` (0x166). It has a ``wfx.cbSize`` value of 34 for a total of 52 bytes.

```cpp
WAVEFORMATEX wfx;
WORD  NumStreams;
DWORD ChannelMask;
DWORD SamplesEncoded;
DWORD BytesPerBlock;
DWORD PlayBegin;
DWORD PlayLength;
DWORD LoopBegin;
DWORD LoopLength;
BYTE  LoopCount;
BYTE  EncoderVersion;
WORD  BlockCount;
```

> Older versions of the Xbox supported XMA v1 indicated with ``WAVE_FORMAT_XMA`` (0x165).
