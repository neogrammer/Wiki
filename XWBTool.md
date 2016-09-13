The XWBTool is a command-line utility for building XACT-style wave banks for use with the _DirectXTK for Audio_ Wave Bank class.

See [[Audio]], [[WaveBank]]

*Note:* The command-line tools in the DirectXTK package are only built by the _DirectXTK_Desktop_201x_ solutions since they are Win32 desktop applications.

# Example usage

Open a [Command Prompt](http://windows.microsoft.com/en-us/windows/command-prompt-faq), and change to the directory containing ``XWBTool.exe`` (i.e. ``...\DirectXTK\XWBTool\bin\Release``)

Enter the following command-line after changing to the appropriate directory:

    xwbtool -o wavebank.xwb Sound.wav Explosion.wav Music.wav

The file _wavebank.xwb_ is generated from the three input ``.wav`` files.

# Options

**-r**
* Input file names can contain wildcard (``?`` or ``*``). If this switch is used, subdirectories are also searched.

**-s**
* Creates as streaming wave bank, otherwise defaults to in-memory wave bank

**-o _filename_**
* Sets output filename for .xwb file. Otherwise, it defaults to the same base name as the first input .wav file

**-h _h-filename_**
* Generates a C/C++ header file with #defines for each of the sounds in the bank matched to their index

**-n**
* Disables the default warning of overwriting an existing ``.xwb`` file

**-c / -nc**
* Forces creation or prevents use of compact wave banks. By default, it will try to use a compact wave bank if possible.

**-f**
* Includes entry friendly name strings in the wave bank for use with 'string' based versions of WaveBank::Play() and WaveBank::CreateInstance() rather than index-based versions.

# Wave bank types
XACT-style wave banks come in two forms: _in-memory_ and _streaming_. The _in-memory_ form is intended to have the whole wave bank loaded into memory at once for use. This allows wave banks to be organized by use and efficiently loaded with minimal memory fragmentation. For use with [[SoundEffectInstance]] and as one-shots with [[WaveBank]]'s Play method, use the in-memory form.

The _streaming_ form is laid out such that each individual wave in the bank is aligned to 4K boundaries for use with async I/O. Only the metadata is loaded into memory, with all the wave data read on-demand from the remainder of the file. See the _XAudio2AsyncStream_ sample code on [MSDN Code Gallery](http://code.msdn.microsoft.com/XAudio2-Win32-Samples-024b3933) or [GitHub](https://github.com/walbourn/directx-sdk-samples) for an example of using the streaming wave bank form.

# Compact wave banks
XACT-style wave banks store meta-data in two forms: standard and compact. In the standard form, the wave-bank metadata is written using a 24-byte record per wave in the bank. In the compact form, they only requires 4-bytes per record. The Compact form requires that all waves in the bank have the same format, and the overall size of the bank's wave data must be less than 8,388,604 (~8 MB) for in-memory wave banks, or 268,433,408 bytes (~256 MBs) for streaming wave banks.

XWBTool will attempt to create a compact wave bank if the input .wav files allow, otherwise it will use the standard form. The ``-c`` and ``-nc`` command line options override this behavior.

# XACT3
The XACT3 GUI and/or the XACTBLD command-line tool in the legacy DirectX SDK (DirectX SDK) can be used to build ``.xwb`` wave banks that are compatible with _DirectXTK for Audio_ if built for Windows (Little-endian) rather than Xbox 360 (Big-endian). XWBTool is just a simplified way to build them.

**Endianness**: _DirectXTK for Audio_ does support loading Big-endian (aka Xbox 360) wave banks since those are the only kind that are generated when using the legacy tools with XMA compression when targeting the Xbox One XDK. This support is limited to XMA and 8-bit PCM data, however, as the wave data itself is not byte-swapped. XWBtool always builds Little-endian wave banks.

[Known Issue: DirectX SDK (June 2010) Setup and the S1023 Error](http://blogs.msdn.com/b/chuckw/archive/2011/12/09/known-issue-directx-sdk-june-2010-setup-and-the-s1023-error.aspx)

[KB2728613](https://support.microsoft.com/en-us/kb/2728613)

[Where is the DirectX SDK?](http://msdn.microsoft.com/en-us/library/windows/desktop/ee663275.aspx)

**Compact wave banks**: The legacy XACT3 ENGINE only supports 'streaming' compact wave banks not 'in-memory' compact wave banks. The XACT3 GUI / XACTBLD toolset will therefore only create 'standard' in-memory wave banks. There is also a known bug in the DirectX SDK (June 2010) version of the XACT3 GUI / XACTBLD toolsets that will not attempt to create a compact wave bank if the wave data size exceeds 2,097,151 (~2 MB).

# Content support
XACT-style wave banks support 8-bit and 16-bit PCM (i.e. not 32-bit IEEE float PCM), ADPCM, xWMA, and XMA2 content.

To compress to ADPCM (a variant of MS-ADPCM) .wav files, use ``adpcmencode.exe`` from the Windows 8.x SDK, Xbox One ADK, Xbox One XDK, or legacy DirectX SDK.

To compress to xWMA .wav files, use ``xwmaencode.exe`` from the Xbox One ADK, Xbox One XDK, or legacy DirectX SDK.

To compress to XMA2 .wav files, use ``xma2encode.exe`` from the Xbox One XDK.

# Note
This tool is also included in the XAudio2 Win32 desktop samples package.

[MSDN Code Gallery](http://code.msdn.microsoft.com/XAudio2-Win32-Samples-024b3933) or [GitHub](https://github.com/walbourn/directx-sdk-samples)
