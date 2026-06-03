# Relationship with VoronoiMeshMaker

Mohid-NG does not generate meshes. It consumes validated Voronoi-based computational mesh packages produced by VoronoiMeshMaker.

VoronoiMeshMaker is responsible for generation, remeshing, adaptation, generator redistribution, connectivity reconstruction, mesh quality metrics, GIS preparation and mesh hierarchy construction.

Mohid-NG is responsible for fields, physical models, finite-volume operators, solvers, diagnostics, outputs, restart, validation and scientific workflows.
