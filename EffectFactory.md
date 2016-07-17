This is a helper object primarily used by the [[Model]] loader implementations to provide sharing of material instances of [[Effects]] and textures. This can be used standalone as well, and allows access to any existing ‘materials’ definitions already created.

It uses a simple case-sensitive string-based (wide-character) map for finding effect and texture instances that have already been created by the factory, which avoid duplication of texture and effect resources in complex models and scenes.

# Header
    #include <Effects.h>

# Initialization
The **EffectFactory** and **DGSLEffectFactory** constructor require a Direct3D 11 device.

    std::unique_ptr<EffectFactory> fxFactory;
    fxFactory = std::make_unique<EffectFactory>( device );

-or-

    std::unique_ptr<DGSLEffectFactory> fxFactory;
    fxFactory = std::make_unique<DGSLEffectFactory>( device );

For exception safety, it is recommended you make use of the C++ [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) pattern and use a ``std::unique_ptr`` or ``std::shared_ptr``

# Creating effects
Fill out the EffectInfo structure, then call **CreateEffect** to obtain an [[Effects]] instance. If the _info.name_ string is provided then any already created effect from the factory that has the same name will be returned as a shared instance rather than a new instance created. If there is a name match, then all the other parameters in the EffectInfo are ignored.  Otherwise a new effect is created from the provided EffectInfo parameters, and CreateTexture is called automatically if the _info.diffuseTexture_, _info.specularTexture_, and/or _info.normalTexture_ strings are provided. Remember that the use of a texture or _info.perVertexColor_ will result in a varying the input layout requirements for the resulting Effect.

    EffectFactory::EffectInfo info;
    info.name = L”default”;
    info.alpha = 1.f;
    info.ambientColor = XMFLOAT3( 0.2f, 0.2f, 0.2f);
    info.diffuseColor = XMFLOAT3( 0.8f, 0.8f, 0.8f );

    auto effect = fxFactory->CreateEffect( info, deviceContext );

The standard factory will create instances of [[BasicEffect]]. If _info.enableSkinning_ is true, it returns [[SkinnedEffect]] instances instead. If _info.enableDualTexture_ is true, it returns a [[DualTextureEffect]] instance. If _info.enableNormalMaps_ is true--and you haven't called **EnableNormalMapEffect(false)**--, then it returns a [[NormalMapEffect]] instance. They are kept in distinct 'sharing' lists since they have different input layout requirements.

# Creating DGSL Effects
The **DGSLEffectFactory** extends the standard EffectFactory with support for the Visual Studio Shader Designer (DGSL) system used by ``.CMO`` files. It creates instances of [[DGSLEffect]]. It also supports sharing the pixel shader instances required for DGSL shaders through the **CreatePixelShader** method.

    DGSLEffectFactory::DGSLEffectInfo info;
    info.name = L”default”;
    info.alpha = 1.f;
    info.ambientColor = XMFLOAT3( 0.2f, 0.2f, 0.2f);
    info.diffuseColor = XMFLOAT3( 0.8f, 0.8f, 0.8f );
    info.pixelShader = L"lambert.dgsl.cso";
    auto effect = fxFactory->CreateDGSLEffect( info, deviceContext );

Because Visual Studio Shader Designer (DGSL) ``.DGSL.CSO`` files only support Feature Level 10.0+, the _DGSLEffectFactory_ automatically uses a ``.CSO`` of the same name instead which is assumed to be a manually created fall-back shader compiled for Feature Level 9.x. This is the same technique as is used by the [Visual Studio 3D Starter Kit](http://code.msdn.microsoft.com/windowsapps/Visual-Studio-3D-Starter-455a15f1).

*Note*: You can use the _DGSLEffectFactory_ with SDKMESH models as long as they include the required TANGENT Information in their vertex decls. These models do not define DGSL pixel shaders as part of their material definition, so these models will always use the default materials (_Unlit_, _Lambert_, or _Phong_).

The DGSLEffect built-in supports the three default materials for all feature levels without requiring any ``.CSO`` files: _Unlit_, _Lambert_, and _Phong_.

_DGSLEffect_ instances with and without skinning enable are kept in distinct 'sharing' lists since they have different input layout requirements.

# Creating textures
**CreateTexture** assumes the name given is the filename of the texture to load. If the name string provided matches an already returned texture from the factory, then the existing shader resource view is returned rather than a new texture instance being created. If the name contains the extension ``.dds``, then [[DDSTextureLoader]] is used to create the texture and return the shader resource view, otherwise it will attempt to use [[WICTextureLoader]].  The device context is not used by the DDSTextureLoader, only by the WICTextureLoader or can be null to skip auto-gen of mipmaps.

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    fxFactory->CreateTexture( L”stone.dds”, nullptr, srv.GetAddressOf() );

_Note: On the Windows phone 8 platform, WIC is not supported or used. Only ``DDS`` textures are supported on this platform._

# Directories
The **CreateTexture** and **CreatePixelShader** methods both assume the name given is the filename. By default, this is a relative path to the current directory. To cause the factory to look in a specific directory path, use **SetDirectory**.

    fxFactory->SetDirectory( L".\\Assets" );

# Interfaces
The _EffectFactory_ and _DGSLEffectFactory_ are concrete implementations of the _IEffectFactory_ interface, and provide a default implementation and caching policy. This allows the developer to create their own custom version of the Effect Factory by deriving from _IEffectFactory_, which can be used with Model loaders. This could be used for alternative caching policies, locating textures in packed archives, substituting special [[Effects]] triggered material name, etc.

# Sharing
You can control the sharing cache with two methods that are implemented for both _EffectFactory_ and _DGSLEffectFactory_.

This method sets the sharing mode which defaults to true. By setting it to false, **CreateEffect**, **CreateTexture**, and **CreatePixelShader** calls will always return a new instance rather than returning a cached instance.

    fxFactory->SetSharing( false );

This method clears the sharing cache, which might not release all the instances if they are referenced by other objects.

    fxFactory->ReleaseCache();

# Threading model
Creation of resources is fully asynchronous, so you can create many effects and textures at the same time. **CreateEffect** and **CreateTexture** take an optional immediate device context for use when loading WIC-based textures to make use of auto-generated mipmaps. Since use of a device context is not ‘free threaded’, an internal lock is used to keep multiple instances of the WIC loader from being used at the same time, but the user must still take precautions to ensure other users of the immediate context or Present do not occur while loading WIC textures and setting up auto-gen mipmaps.

_Note: This is yet another reason we recommend using ``DDS`` textures for all your assets._
