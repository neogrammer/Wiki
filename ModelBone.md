This class is part of the [[Model]] hierarchy. The purpose of this structure is to define a hierarchy of transformation matrices used for rigid-body animation, skinning animation, or as metadata for locations associated with a model for runtime insertion of dynamic lights, particle effects, etc.

The use of ModelBone is optional and the ``Model::bones`` array an therefore be empty.

# Header
```cpp
#include <Model.h>
```

# Initialization
The ModelBone array *bones* is typically created by a Model loader along with the ModelMesh instances that contain the submeshes. By default the loader ignores any frames (SDKMESH) or bones (CMO) found in the file. Providing the ``ModelLoader_IncludeBones`` flag will result in any bones present in the model file being loaded.

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

# Remarks

The XNA Game Studio ModelBone design relied on the fact that the Content Pipeline always sorted the bone array in a particular way. This uses a more complex tree graph structure to support SDKMESH and CMO which may not meet these sorting requirements.

There is no explicit *root* bone in a Model in *DirectX Tool Kit*. By convention, the 0th bone is the root.
