In this lesson we learn the basics of Physically-Based Rendering (PBR) as supported by the *DirectX Tool Kit*.

# What does Physically-based mean?

A full discussion of Physically-Based Rendering (PBR) is beyond the scope of this lesson, so see the references at the end of the page. Instead I'll provide a short motivation of why PBR is useful. The first thing to acknowledge is that traditional computer graphics lighting algorithms are inspired hacks. They work well at providing many useful lighting clues, and have been inexpensive enough to compute on consumer level hardware for decades. These algorithms, however, all have drawbacks. For example, basically anything you render using [Phong shading](https://en.wikipedia.org/wiki/Phong_shading) ends up looking like it's made of smooth plastic. Another challenge is that textures, models, and other assets that look great in some lighting conditions and lighting algorithms don't work at all when moved to a new engine or solution which makes it harder to reuse expensive artwork.

The proponents of PBR rendering have gone back to the foundational [rendering equation](https://en.wikipedia.org/wiki/Rendering_equation) and built new algorithms that in some way better mimic the real physics (such as the *law of conservation of energy*). While there are many ways to formulate a PBR materials & lighting system, the industry has converged on a few workflows. *DirectX Tool Kit* implements the "Disney-style Roughness/Metalness" workflow as it's well-understood, has reasonably good tooling support, and is the one that was chosen for [Khronos' glTF2 asset format](https://www.khronos.org/gltf/) and many modern game engines.

> PBR rendering essentially requires HDR rendering as there's no physical process that clamps light into a 0 to 1 range. As such, be sure you have worked through the [[Using HDR rendering]] tutorial before this one.

## Image-Based Lighting (IBL)

Another important aspect of PBR is that real world lighting is not well modeled by trivial point, directional, or spot light sources. Area lighting or other global illumination systems are expensive and/or complex to implement in real-time systems, so for the purposes of *DirectX Tool Kit*'s PBR implementation we make of [image-based lighting](https://en.wikipedia.org/wiki/Image-based_lighting). Specifically the lighting environment consists of two specially formulated cubemaps. You can add some directional lighting as well, but the interesting interactions of light that convey "metalness" or "roughness" require some advanced lighting environment.

# PBREffect

> **UNDER CONSTRUCTION**

# Rendering a PBR Model

> **UNDER CONSTRUCTION**

**Next lessons:** [[Game controller input]], [[Using the SimpleMath library]], [[Adding the DirectX Tool Kit for Audio]]

# Further Reading