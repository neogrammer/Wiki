[![xwbtool.exe](https://img.shields.io/github/downloads/Microsoft/DirectXTK/latest/xwbtool.exe?style=for-the-badge)](https://github.com/microsoft/DirectXTK/releases/latest/download/xwbtool.exe)

The XWBTool is a command-line utility for building XACT-style wave banks for use with the _DirectXTK for Audio_ Wave Bank class.

See [[Audio]], [[WaveBank]]

> The command-line tools in the DirectXTK package are only built by the _DirectXTK_Desktop_201x_ solutions since they are Win32 desktop applications.

**Related tutorial**: [[Making use of wave banks]]

# Synax

``xwbtool.exe`` uses the following command syntax:

    xwbtool [-r] [-s] [-o <filename>] [-h <filename>] [-l] [-y] [-c | -nc] [-f] [-af]
    [-flist <filename>] <file-name(s)>

The file-name parameter indicates the ``.wav`` file(s) to add to the wave bank.

# Optional Switches Description

**-r**: Input file names can contain wildcard characters (``?`` or ``*``). If this switch is used, subdirectories are also searched.

**-s**: Creates as streaming wave bank, otherwise defaults to in-memory wave bank

**-o _filename_**: Sets output filename for ``.xwb`` file. Otherwise, it defaults to the same base name as the first input ``.wav`` file

**-h _filename_**: Generates a C/C++ header file with ``#define`` symbols for each of the sounds in the bank matched to their index

**-l**: Forces the output path & filename to all lower-case. *Windows file system is case-insensitive by default, but some programs like git are case-sensitive*.

**-y**: overwrite existing output file if any. By default, the tool will not overwrite a file if it exists.

**-c / -nc**: Forces creation or prevents use of compact wave banks. By default, it will try to use a compact wave bank if possible.

**-f**: Includes entry friendly name strings in the wave bank for use with 'string' based versions of WaveBank::Play() and WaveBank::CreateInstance() rather than index-based versions.

**-af**: Indicates that streaming wave banks should use 4K alignment rather than DVD (2K) alignment. This is needed to support Advanced Format (4K native) disks with non-buffered I/O reads if 512e emulation is disabled. See below for more information.

**-flist _filename_**: Uses the provided filename as a text file containing a list of input files (one per line). Ignores lines that begin with ``#`` (used for comments). Does not support providing additional command-line arguments or the use of filename wildcards.

# Example usage

Open a [Command Prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq), and change to the directory containing ``XWBTool.exe`` (i.e. ``...\DirectXTK\XWBTool\bin\Release``)

Enter the following command-line after changing to the appropriate directory:

    xwbtool -o wavebank.xwb Sound.wav Explosion.wav Music.wav

The file _wavebank.xwb_ is generated from the three input ``.wav`` files.

    xwbtool -o test.xwb C:\Windows\Media\*.wav

The file _test.xwb_ is generated from all the ``.wav`` files in the ``C:\Windows\Media`` directory.

# Wave bank types
XACT-style wave banks come in two forms: _in-memory_ and _streaming_. The _in-memory_ form is intended to have the whole wave bank loaded into memory at once for use. This allows wave banks to be organized by use and efficiently loaded with minimal memory fragmentation. For use with [[SoundEffectInstance]] and as one-shots with [[WaveBank]]'s Play method, use the in-memory form.

The _streaming_ form is laid out such that each individual wave in the bank is aligned to 4K boundaries for use with async I/O. Only the metadata is loaded into memory, with all the wave data read on-demand from the remainder of the file. See the _XAudio2AsyncStream_ sample code on [GitHub](https://github.com/walbourn/directx-sdk-samples) for an example of using the streaming wave bank form.

# Compact wave banks
XACT-style wave banks store meta-data in two forms: standard and compact. In the standard form, the wave-bank metadata is written using a 24-byte record per wave in the bank. In the compact form, they only requires 4-bytes per record. The Compact form requires that all waves in the bank have the same format, and the overall size of the bank's wave data must be less than 8,388,604 (~8 MB) for in-memory wave banks, or 268,433,408 bytes (~256 MBs) for streaming wave banks.

XWBTool will attempt to create a compact wave bank if the input .wav files allow, otherwise it will use the standard form. The ``-c`` and ``-nc`` command line options override this behavior.

# Advanced Format disks
For streaming wave banks, the layout of the file is purposely designed to support non-buffered asynchronous I/O for the wave data. By default the ``-s`` switch will align the wave data to 2048 bytes which is consistent with DVD sector alignment, and has traditionally been a multiple for all known HDD sector sizes. With the advent of newer Advanced Format (4k native) HDDs, 4096 byte alignment is becoming the norm. By using ``-s -af``, the generated streaming wave bank will use 4K sector alignment.

See [Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/w8cookbook/advanced-format--4k--disk-compatibility-update) for more information, and the 512e emulation in place for many systems.

Compact wave banks (``-c``) are not compatible with Advanced Format sector alignment (``-af``), so can't be used together.

# XACT3
The XACT3 GUI and/or the XACTBLD command-line tool in the legacy DirectX SDK (DirectX SDK) can be used to build ``.xwb`` wave banks that are compatible with _DirectXTK for Audio_ if built for Windows (Little-endian) rather than Xbox 360 (Big-endian). XWBTool is just a simplified way to build them.

**Endianness**: _DirectXTK for Audio_ does support loading Big-endian (aka Xbox 360) wave banks since those are the only kind that are generated when using the legacy tools with XMA compression when targeting the Xbox One XDK. This support is limited to XMA and 8-bit PCM data, however, as the wave data itself is not byte-swapped. XWBtool always builds Little-endian wave banks.

[Known Issue: DirectX SDK (June 2010) Setup and the S1023 Error](https://walbourn.github.io/known-issue-directx-sdk-june-2010-setup-and-the-s1023-error/)

[KB2728613](https://support.microsoft.com/en-us/kb/2728613)

[Where is the DirectX SDK?](https://docs.microsoft.com/en-us/windows/desktop/directx-sdk--august-2009-)

**Compact wave banks**: The legacy XACT3 ENGINE only supports 'streaming' compact wave banks not 'in-memory' compact wave banks. The XACT3 GUI / XACTBLD toolset will therefore only create 'standard' in-memory wave banks. There is also a known bug in the DirectX SDK (June 2010) version of the XACT3 GUI / XACTBLD toolsets that will not attempt to create a compact wave bank if the wave data size exceeds 2,097,151 (~2 MB).

# Content support
XACT-style wave banks support 8-bit and 16-bit PCM (i.e. not 32-bit IEEE float PCM), ADPCM, xWMA, and XMA2 content.

To compress to ADPCM (a variant of MS-ADPCM) .wav files, use ``adpcmencode.exe`` from the Windows SDK, Xbox One XDK, or legacy DirectX SDK.

To compress to xWMA .wav files, use ``xwmaencode.exe`` from the Microsoft GDK with Xbox extensions, Xbox One XDK, or legacy DirectX SDK.

To compress to XMA2 .wav files, use ``xma2encode.exe`` from the Microsoft GDK with Xbox extensions or the Xbox One XDK.

# Note
This tool is also included in the XAudio2 Win32 desktop samples package.

[MSDN Code Gallery](http://code.msdn.microsoft.com/XAudio2-Win32-Samples-024b3933) or [GitHub](https://github.com/walbourn/directx-sdk-samples)
