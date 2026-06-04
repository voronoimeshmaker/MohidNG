# Relation to VoronoiMeshMaker

VoronoiMeshMaker is responsible for all mesh-related implementation. Mohid-NG does not generate meshes internally.

VoronoiMeshMaker shall provide validated Voronoi mesh packages containing, at minimum, topology, geometry, boundary patches, physical regions, quality metrics, CRS metadata and optional adaptation metadata.

Mohid-NG shall consume these packages, validate their completeness and use them for finite-volume assembly, diagnostics and output.

```{admonition} Boundary between projects
:class: voronoimeshmaker
If an operation changes points, cells, faces, connectivity, geometric quality, remeshing hierarchy or mesh adaptation state, it belongs to VoronoiMeshMaker. If an operation advances equations, fields, residuals, solvers or diagnostics over an already supplied mesh package, it belongs to Mohid-NG.
```
