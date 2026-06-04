# Gradient reconstruction

Gradient reconstruction is a first-class numerical service of Mohid-NG. It is required for diffusion, reconstruction, source terms, diagnostics and adaptation indicators.

The Block 2 implementation follows the project gradient-reconstruction study {cite}`vasconcellos2026gradient`: gradient operators are compared by accuracy, robustness to Voronoi mesh topology and repeated-evaluation cost after mesh-dependent quantities have been precomputed.

The first production direction is weighted least-squares on cell-centred fields. The default weighting is inverse-distance-squared because the project study identifies it as the most robust choice on irregular Voronoi meshes. Uniform and inverse-distance weights remain available as policies for comparison and diagnostics.

## Extension model

Gradient methods must not be represented by a central enum. Mohid-NG uses a registry/factory model: a method registers a workspace builder under a stable textual name, and the caller selects that name through `GradientOptions`. This keeps new methods local to their own implementation files.

Gradient weightings are policies. A built-in policy may be used, or a caller may provide a named callable policy without changing Mohid-NG core code.

## Initial method families

The target families are:

```text
weighted least-squares by normal equations
QR-based weighted least-squares
classical Green-Gauss
modified Green-Gauss
iterated Green-Gauss
curvilinear reconstruction
limited gradients
```

Block 2 starts with weighted least-squares by normal equations, cached per-cell coefficients and a classical Green-Gauss method. Later methods should reuse the same testing pattern: constant fields must have zero gradient, linear fields must be exact on suitable stencils, and diagnostics must expose failed or ill-conditioned cells.

Adaptation indicators shall not depend exclusively on a single raw gradient norm.

## Adding a method

A new gradient method should provide a `CellGradientWorkspaceBuilder` and register it with `RegisterCellGradientMethod`. The builder receives `MeshView` and `GradientOptions`, builds method-specific per-cell stencils and returns a `CellGradientWorkspace` with method and weighting names filled for diagnostics.

A new weighting does not require changing a central list. Provide a callable policy over `GradientNeighbourGeometry` and wrap it in `GradientWeightingPolicy`, or expose a small named helper similar to `InverseDistanceSquaredWeightingPolicy`.

The workspace exposes `Diagnostics()` so tests and examples can check method name, weighting name, cell count, well-conditioned stencil count, ill-conditioned stencil count and cached coefficient count.`r`n## References

```{bibliography}
:filter: docname in docnames
```
