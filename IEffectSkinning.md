This abstract interface controls skinning animation matrices. Settings for this interface can influence the choice of shader permutation.

Effects that implement this interface typically require ``BLENDINDICES`` and ``BLENDWEIGHT`` semantic data in the vertex input layout.

# Obtaining the interface
There are two methods used in _DirectX Tool Kit_. For simple cases, just maintain a reference directly to the desired effect class:

```cpp
std::shared_ptr<SkinnedEffect> effect;

...

effect->SetBoneTransforms( bones.get(), boneCount );
```

For more general cases where a number of effect classes can be in use (such as [[Model]] which uses a mix of _BasicEffect_, _DualTextureEffect_, _SkinnedEffect_, and/or _DGSLEffect_), use [Run-Time Type Information](https://en.wikipedia.org/wiki/Run-time_type_information) (RTTI) to obtain the interface.

```cpp
std::shared_ptr<IEffect> effect;

...

auto iskin = dynamic_cast<IEffectSkinning*>( effect.get() );
if ( iskin )
{
    iskin->SetBoneTransforms( bones.get(), boneCount );
}
```

# Skinning
The skinning interface is primarily used to set the per-bone transformation matrices for rendering. This is accomplished through the **SetBoneTransforms** method.  Because the method takes a pointer to ``XMMATRIX``, the memory buffer must be 16-byte aligned. While this is the default of ``new`` and ``malloc`` on 64-bit platforms, this is not true by default for 32-bit platforms. Use of ``_aligned_malloc`` is recommended.

```cpp
struct aligned_deleter { void operator()(void* p) { _aligned_free(p); } };

std::unique_ptr<XMMATRIX[], aligned_deleter> bones(
        reinterpret_cast<XMMATRIX*>( _aligned_malloc(
            sizeof(XMMATRIX) * boneCount, 16 ) ) );

...

// Simple time-based uniform scaling
float s = 1 + sin(time * 1.7f) * 0.5f;
XMMATRIX scale = XMMatrixScaling(s,s,s);

for (size_t j=0; j < boneCount; ++j )
{
    bones[ j ] = scale;
}

effect->SetBoneTransforms(bones.get(), boneCount);
```

Note that the maximum bone count must be <= _MaxBones_ (72).

You can call **ResetBoneTransforms** to reset all per-bone transforms to the identity, which is also the default when the effect is newly created.

Finally, the **SetWeightsPerVertex** method is used as a way to optimize performance of the vertex shader. By default, it assumes that all 4 bones listed in the per-vertex ``BLENDINDICES`` and ``BLENDWEIGHT`` are used by the model. You can use this method to set the value to 1 or 2 which will choose a lighter-weight shader permutation that ignores the extra 3 or 2 bones influences per vertex, or back to the default of 4. The choice of this value is driven entirely by the content.

# Built-in Effect Notes

## BasicEffect, AlphaTestEffect, DualTextureEffect, EnvironmentMapEffect
These built-in effects do not implement skinning animation.

## SkinnedDGSLEffect
This effect implements skinning for the DGSL pipeline.

> In older versions of *DirectX Tool Kit*, skinning was directly implemented by **DGSLEffect** with a ctor parameter. In the current library, the DGSL effect only supports skinning if it's a **SkinnedDGSLEffect** instance.

## SkinnedEffect
See [Built-in effects, permutations, and performance](http://www.shawnhargreaves.com/blog/built-in-effects-permutations-and-performance.html) for performance costs of the various shader permutations.

## SkinnedNormalMapEffect
This built-in effect supports skinning.

# Remark
As an optimization, the skinning effects assume that all the matrices are affine transformations, and that the final column is (0 0 0 1). This means that the value of the last column is effectively ignored when set into the constant buffer containing the bone transformations (i.e. the shaders use ``float4x3``)
