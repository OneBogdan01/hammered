# Alloy UI - demos

Small, self-contained demos for **Alloy**, the 2D SDF UI renderer in the Hammered engine. The module itself lives in [`modules/hm_alloy_ui`](../../modules/hm_alloy);

> Full writeup: **[A Quick 2D SDF Renderer with Tiled Rendering](https://tycro-games.github.io/posts/A-quick-2D-SDF-Renderer-with-Tiled-Rendering/)**

## Demos

- **`hardcoded_shapes`**:first SDF shapes written straight into the fragment shader.
- **`shapes`**: a dynamic scene of shapes uploaded through a GPU storage buffer.
- **`tiled_debug_shapes`**: the tiled-rendering optimization, with a per-tile heatmap debug view.

I shall refactor the engine module toward something better, since the demos tend to have a lot of repeating code that I plan to move to the module itself.
