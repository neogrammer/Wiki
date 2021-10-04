This class is part of the [[Model]] hierarchy. The purpose of this structure is to define a hierarchy of transformation matrices used for rigid-body animation, skinning animation, or as metadata for locations associated with a model for runtime insertion of dynamic lights, particle effects, etc.

The use of ModelBone is optional. Therefore the ``Model::bones`` array can be empty, and both ``boneMatrices`` and  ``invBindPoseMatrices`` can be null.

**Related tutorial:** [[Animating using model bones]], [[Using skinned models]]

# Header
```cpp
#include <Model.h>
```

# Initialization
The ModelBone array *bones* in Model is typically created by a Model loader along with the ModelMesh instances that contain the submeshes. By default the loader ignores any frames (SDKMESH) or bones (CMO) found in the file. Providing the ``ModelLoader_IncludeBones`` flag will result in any bones present in the model file being loaded.

The Model *boneMatrices* transformation array is initialized to the local transformation for the given bone. The array length is *bones.size()*.

The Model *invBindPoseMatrices* transform array is the "inverse bind pose" absolute transformation. The array length is *bones.size()*.

> For SDKMESH, if a frame is associated with a mesh, the ModelMesh *boneIndex* entry will be set, otherwise it is set to ``ModelBone::c_Invalid``. This is used for rigid-body animation.

> SDKMESH files can also contain 'influences' which are loaded into ModelMesh *boneInfluences*. Use of influenced-map bones is optional for skinning effect drawing. This allows the model to use more total bones than the maximum bone count per effect of ``IEffectSkinning::MaxBones`` (72). Skinned CMO models are always 'direct-mapped' and have an empty *boneInfluences* array.

# Type aliases

**ModelBone::Collection** is an alias for ``std::vector<ModelBone>``.

**ModelBone::TransformArray** is an alias for ``std::unique_ptr<XMMATRIX[], aligned_deleter>``.

## Transform arrays

**ModelBone::TransformArray** instances are created using the static method ``ModelBone::MakeArray``. This memory is always allocated using ``_aligned_malloc`` with 16-byte alignment to meet the requirements for SIMD.

```cpp
auto bones = ModelBone::MakeArray(model->bones.size());
```

# Data

A ModelBone instance contains a *name* (a wide-character string) typically used for matching up bones with animation keys.

Each bone contains three pointers:

* *parentIndex* is the index of the parent of this bone (or ``ModelBone::c_Invalid`` if it has no parent).
* *childIndex* is the index of the child of this bone (or ``ModelBone::c_Invalid`` if it has no children).
* *siblingIndex* is the index of the next sibling of this bone (or ``ModelBone::c_Invalid`` if it has no siblings).

These pointers are used to form a tree hierarchy. The 0th bone in a Model is the root and should have a *parentIndex* of ``ModelBone::c_Invalid``.

> Note that bones must form an [acyclic graph](https://en.wikipedia.org/wiki/Directed_acyclic_graph).

# Working with bones

Even if bones are loaded and defined in a Model, the standard **Draw** methods ignore model bones. You must explicitly pass the fully computed array of bone transformations to the **Draw** or **DrawSkinned** methods to draw using model bones.

The following Model methods exist to support working with ModelBone transformations:

* **CopyAbsoluteBoneTransformsTo**: This computes the final "absolute" transformations using the local transformations defined in *boneMatrices* and the hierarchy defined in *bones*. The result is placed into the transformation array provided as a parameter which must have *nbones* entries.

* **CopyAbsoluteBoneTransforms**: This computes the final "absolute" transformations using the local transformations provided as the transform array and the hierarchy defined in *bones*. The result is placed into the second transformation array. Both arrays must have at least *nbones* entries.

* **CopyBoneTransformsFrom**: This copies data from the transformation array provided into the *boneMatrices* array in the Model.

* **CopyBoneTransformsTo**: This copies data out of the *boneMatrices* array in the Model into the transformation array provided.

# Remarks

The XNA Game Studio ModelBone design relied on the fact that the Content Pipeline always sorted the bone array in a particular way. This uses a more complex tree graph structure to support SDKMESH and CMO which may not meet these sorting requirements.

There is no explicit *root* bone in a Model in *DirectX Tool Kit*. By convention, the 0th bone is the root.
