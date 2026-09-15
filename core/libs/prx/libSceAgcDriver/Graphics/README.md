# Indexed graphics execution

`Driver::draw` decodes graphics state, recompiles the vertex and fragment stages, and calls `VulkanDevice::DrawIndexed`. Successful compilation alone does not submit a draw.

## Modules

- `State.cpp`: required register values, render-target layout, viewport, scissors, culling and blending.
- `ShaderValidation.cpp`: compatibility of the already validated SPIR-V with the driver's supported interfaces, descriptors, stage capabilities and push constant ranges. General SPIR-V validation belongs to `Recompile`. The driver does not link SPIRV-Tools.
- `ShaderResources.cpp`: guest buffer descriptors, Vulkan descriptor sets, memory alias checks and writable buffer readback.
- `Resources.cpp`: buffer/image allocation and command batch lifetime.
- `Pipeline.cpp`: shader modules, pipeline layout, render pass, framebuffer and graphics pipeline.
- `Draw.cpp`: index upload, preservation of existing color pixels, barriers, indexed draw, completion and guest-memory writeback.

## Supported contract

- `DRAW_INDEX_OFFSET_2`, uint16/uint32 indices, nonzero index and instance counts, no draw modifiers or primitive restart.
- Triangle lists, strips and fans; fill rasterization; supported cull state; first provoking vertex.
- One vertex stage and one fragment stage. Vertex attributes must be fetched by the recompiled shader through buffer descriptors. Conventional Vulkan vertex input locations are rejected.
- One float4 fragment color output at location zero; matching scalar/vector stage interfaces.
- Vertex descriptors use set 0 and fragment descriptors use set 1. Push constants occupy at most 64 bytes per stage, at offsets 0 and 64 respectively.
- Uniform/storage buffer descriptors with one element, zero stride, raw bounds and GFX10 resource level 1. Partially overlapping descriptors, writable descriptor aliases, render-target aliases and writable index aliases are rejected.
- One linear 2D RGBA8/BGRA8 UNORM or sRGB color target, mip zero, one layer, one sample, no DCC/CMASK compression. Width must be a multiple of 64 pixels so the 256-byte row alignment introduces no unknown padding.
- Viewport transform, negative viewport height, scissor intersection, color write mask and supported single-source blending. No depth/stencil, alpha-to-coverage, custom sample masks or conservative rasterization.

Missing required registers and unsupported active state raise exceptions. Unsupported resources are never replaced with empty buffers or dummy images. The renderer never substitutes a clear for a failed draw.

Primitive restart uses the PS5 user-config register `GE_MULTI_PRIM_IB_RESET_EN` at `0x24B`, not the legacy context register at `0x2A5`. Queue creation/reset initializes it to zero; context-only clear preserves it. This matches `UserConfig::m_primitive_reset_control` and `HwUcSetMultiPrimIbReset` in `raw/source/KytyPS5-main/src/graphics/guest_gpu`, and the user-config bank of the project's AGC register defaults. Missing entries still fail at read time. Legacy context stream-output registers are not required for the supported primitive-generation stage; transform-feedback SPIR-V remains unsupported.

The target's existing contents are uploaded before rendering. A render pass uses LOAD/STORE, and the finished image and writable shader buffers are copied back to guest memory after the GPU fence completes. Queue cleanup on an exceptional completion path must finish before Vulkan objects used by the queue can be destroyed; cleanup does not retry the draw.

This path deliberately creates transient resources per draw. It provides no resource or pipeline cache. It does not establish a tiled VideoOut rendering path: VideoOut currently expects tiled display memory, while this renderer accepts linear color surfaces only. Tiled rendering, textures/samplers, depth/stencil, additional PM4 draw variants and presentation extension compatibility remain separate work.

## Checks

`agc_driver_graphics_tests` contains CPU tests for surface decoding, negative viewports, scissor intersection and rejection of unsupported state/descriptors. It has no dependency on the recompiler implementation, SPIRV-Tools or a live Vulkan device. These tests do not verify GPU execution or frame presentation.

## Register references

Register offsets and fields were checked against Mesa's [GFX10 register definitions](https://chromium.googlesource.com/chromiumos/third_party/mesa/+/refs/heads/stabilize-13982.70.B-chromeos-amd/src/amd/registers/gfx10.json), [GFX10 resource definitions](https://chromium.googlesource.com/chromiumos/third_party/mesa/+/refs/heads/stabilize-13982.70.B-chromeos-amd/src/amd/registers/gfx10-rsrc.json) and [color surface setup](https://chromium.googlesource.com/chromiumos/third_party/mesa/+/refs/heads/stabilize-13982.70.B-chromeos-amd/src/amd/vulkan/radv_device.c). These references describe GFX10 fields; they do not establish support for every PS5-specific register combination.
