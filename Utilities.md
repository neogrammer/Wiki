For samples, I've created a number of utility headers that help fill in gaps between the [device & swapchain templates](https://github.com/walbourn/directx-vs-templates) and the functionality provided by the *DirectX Tool Kit*. They are all completely optional, but handy. They are referenced in the tutorials and under the class-by-class wiki pages, and here is an index:

<table>
 <tr><td>Header</td><td>Source</td><td>Notes</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/AnimatedTexture.h">AnimatedTexture.h</a></td>
     <td>n/a</td>
     <td>Used for a SpriteFont tutorial. See <a href="/microsoft/DirectXTK/wiki/AnimatedTexture">wiki</a>.</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/ControllerFont.h">ControllerFont.h</a></td>
     <td>n/a</td>
     <td>Helper for using game controller symbols mixed with text. See <a href="/microsoft/DirectXTK/wiki/ControllerFont">wiki</a>.</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/DebugDraw.h">DebugDraw.h</a></td>
     <td><a href="/microsoft/DirectXTK/wiki/DebugDraw.cpp">DebugDraw.cpp</a></td>
     <td>Draws outlined 3D shapes for debug use. See <a href="/microsoft/DirectXTK/wiki/DebugDraw">wiki</a>.</td></tr>
 <tr><td><a href="https://raw.githubusercontent.com/walbourn/directx-vs-templates/master/d3d11game_win32_dr/DeviceResources.h">DeviceResources.h</a></td>
     <td><a href="https://raw.githubusercontent.com/walbourn/directx-vs-templates/master/d3d11game_win32_dr/DeviceResources.cpp">DeviceResources.cpp</a></td>
     <td>Helper for the Direct3D device & swapchain. See <a href="/microsoft/DirectXTK/wiki/DeviceResources">wiki</a>.</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/MSAAHelper.h">MSAAHelper.h</a></td>
     <td><a href="/microsoft/DirectXTK/wiki/MSAAHelper.cpp">MSAAHelper.cpp</a></td>
     <td>Helper for implementing MSAA rendering. See <a href="/microsoft/DirectXTK/wiki/MSAAHelper">wiki</a>.</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/ReadData.h">ReadData.h</a></td>
     <td>n/a</td>
     <td>Helper for loading custom shaders from compiled cso blobs.</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/RenderTexture.h">RenderTexture.h</a></td>
     <td><a href="/microsoft/DirectXTK/wiki/RenderTexture.cpp">RenderTexture.cpp</a></td>
     <td>Helper for implementing render to texture. See <a href="/microsoft/DirectXTK/wiki/RenderTexture">wiki</a>.</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/ScrollingBackground.h">ScrollingBackground.h</a></td>
     <td>n/a</td>
     <td>Used for a SpriteFont tutorial. See <a href="/microsoft/DirectXTK/wiki/ScrollingBackground">wiki</a>.</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/SpriteSheet.h">SpriteSheet.h</a></td>
     <td>n/a</td>
     <td>Used for a SpriteFont tutorial. See <a href="/microsoft/DirectXTK/wiki/SpriteSheet">wiki</a>.</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/SkyboxEffect.h">SkyboxEffect.h</a></td>
     <td><a href="/microsoft/DirectXTK/wiki/SkyboxEffect.cpp">SkyboxEffect.cpp</a></td>
     <td>Simple 3D skybox (requires <a href="/microsoft/DirectXTK/wiki/shaders/SkyboxEffect_VS.hlsl">VS</a>, <a href="/microsoft/DirectXTK/wiki/shaders/SkyboxEffect_PS.hlsl">PS</a>, and <a href="/microsoft/DirectXTK/wiki/shaders/SkyboxEffect_Common.hlsli">hlsli</a> shader files as well). See the <a href="/microsoft/DirectXTK/wiki/Authoring-an-Effect">wiki</a>.</td></tr>
 <tr><td><a href="/microsoft/DirectXTK/wiki/TextConsole.h">TextConsole.h</a></td>
     <td><a href="/microsoft/DirectXTK/wiki/TextConsole.cpp">TextConsole.cpp</a></td>
     <td>Helper for a terminal-style printf text output on a graphics surface using SpriteFont. See <a href="/microsoft/DirectXTK/wiki/TextConsole">wiki</a>.</td>
</table>
