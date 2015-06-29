This abstract interface allows setting rendering matrices typically communicated to the shader via a dynamic constant buffer. This interface is implemented by all built-in effects, but is not required for all effects (i.e. a valid effect can implement [[IEffect]] and not implement _IEffectMatrices_).

# Obtaining the interface

There are two methods used in _DirectX Tool Kit_. For simple cases, just maintain a reference directly to the desired effect class:

    std::shared_ptr<BasicEffect> effect;

    ...

    effect->SetWorld( world );
    effect->SetView( view );
    effect->SetProjection( projection );

For more general cases where a number of effect classes can be in use (such as [[Model]] which uses a mix of _BasicEffect_, _SkinnedEffect_, and/or _DGSLEffect_), use [Run-Time Type Information](https://en.wikipedia.org/wiki/Run-time_type_information) (RTTI) to obtain the interface.

    std::shared_ptr<IEffect> effect;

    ...

    auto imatrices = dynamic_cast<IEffectMatrices*>( part->effect.get() );
    if ( imatrices )
    {
        imatrices->SetWorld( world );
        imatrices->SetView( view );
        imatrices->SetProjection( projection );
    }

*Note* For the specific case of _IEffectMatrices_, you _could_ try to make the assumption that all effects in use implement _IEffectMatrices _and make use of a ``reinterpret_cast<>`` or old-school C-style cast instead of ``dynamic_cast<>``, however this will not work since ``IEffectMatrices`` is not derived from ``IEffect``. You would have to assume a specific class of effect instead.

# Setting matrices
The matrices used by effects can be left-handed or right-handed coordinates, but are always in row-major form.

As an optimization, most shaders actually consume matrices in column-major form, so effects implementations will transpose them as needed when setting them into the constant buffer.

The effects implementations will lazily update various computations (such as inverses and concatenations of matrices), so that changing only one of the three matrices can be less computationally expensive than changing all three.

Most _BasicEffect_ shaders make no particular distinction between the _view_, and _projection_ matrices. You can, for example, provide the combined ``view*projection`` matrix with **SetProjection** and leave the others identity with the same results.

The _BasicEffect_ shaders require the _world_ matrix to be distinct from the _view_ and _projection_ matrices for doing proper transforms of normals into world coordinates.

Some complex shaders such as _DGSLEffect_ where the HLSL shader is given access to both local->world and local->projected transforms could make a distinction between _view_ and _projection_ as well, which requires the three matrices to be set independently to get correct results.
