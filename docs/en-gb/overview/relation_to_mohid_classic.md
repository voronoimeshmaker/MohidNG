# Relation to the original MOHID system

Mohid-NG is an independent new-generation project. It is conceptually motivated by the original MOHID ecosystem, but its numerical and software architecture is different.

The original MOHID system is based on a mature water-modelling framework with structured or logically structured grid assumptions. Mohid-NG shall instead use only Voronoi-based computational meshes.

The intended relation is therefore equation-level compatibility, not discretisation-level compatibility. For each ported physical model, the continuous formulation shall be documented separately from its Mohid-NG Voronoi finite-volume discretisation.
