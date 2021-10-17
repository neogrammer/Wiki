In this lesson we learn how to render models using skinned vertices for smooth animations.

# Setup
First create a new project using the instructions from the previous lessons: [[Using DeviceResources]] and
[[Adding the DirectX Tool Kit]] which we will use for this lesson.

# Background
In the [[previous lesson|Animating using model bones]], we learned the use of [[ModelBone]] as general metadata and for doing rigid-body animation of meshes. The other common use for model bones is as a 'skeleton' to control vertices for [vertex skinning](https://en.wikipedia.org/wiki/Skeletal_animation) animation.

Like *XNA Game Studio*, the *DirectX Tool Kit* does not provide a specific implementation of an animation control system. There are numerous approaches, and each have their own strengths and weaknesses. In this lesson, we'll cover the animation system in the Visual Studio Mesh Content Pipeline (``CMO``) and the legacy DirectX SDK sample animation system (``SDKMESH`` v1), providing some basic code for using them. Neither solution is ideal, but they do a good job of demonstrating the techniques and some trade-offs.

# Loading a skinned model
> **UNDER CONSTRUCTION**

# More to explore

* Vertex skinning is also supported by [[SkinnedNormalMapEffect|NormalMapEffect]], [[SkinnedPBREffect|PBREffect]], and [[SkinnedDGSLEffect|DGSLEffect]] using the ``IEffectSkinning`` interface just as with ``SkinnedEffect``.

**Next lesson:** [[Using advanced shaders]]
